#include "SongOfManaWeapon.h"
#include "../Projectiles/SongOfManaProjectile.h"
#include "../../Entities/Player.h"
#include <cmath>
#include <algorithm>
#include <iostream>

SongOfManaWeapon::SongOfManaWeapon(const WeaponProfile& profile)
    : Weapon(profile)
{
}

void SongOfManaWeapon::Update(float dt, ProjectileManager& projManager, TextureAtlas& atlas, Player& player, EnemyPool& enemyPool)
{
    if (m_cooldownTimer > 0.0f)
    {
        m_cooldownTimer -= dt;
    }

    if (m_cooldownTimer <= 0.0f)
    {
        sf::Vector2f targetPosition = GetTargetPosition(enemyPool, player);
        FireOne(projManager, atlas, player, targetPosition, 0);

        float activeDurationSec = m_profile.GetDuration() * player.GetDurationMultiplier();
        float intervalSec = static_cast<float>(m_profile.GetInterval()) / 1000.0f;
        float totalCycleSec = std::max(activeDurationSec, intervalSec);
        m_cooldownTimer = std::max(0.1f, totalCycleSec * player.GetCooldownMultiplier());
    }
}

void SongOfManaWeapon::FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex)
{
    std::string frameName = m_profile.GetFrameName();
    if (frameName.empty()) {
        frameName = "Song.png";
    }

    AssetTextureData data = atlas.GetTextureData(frameName);
    if (!data.texture)
    {
        data = atlas.GetTextureData("Song.png");
    }

    if (!data.texture)
    {
        return;
    }

    sf::Vector2f playerPos = player.GetPosition();
    float activeDurationSec = m_profile.GetDuration() * player.GetDurationMultiplier();
    float fadeTimeSec = g_SongOfManaFadeTime;
    float power = m_profile.GetPower() * player.GetMightMultiplier();
    float area = m_profile.GetArea() * player.GetAreaMultiplier();

    auto projectile = std::make_unique<SongOfManaProjectile>(
        *data.texture,
        data.rect,
        playerPos,
        activeDurationSec,
        fadeTimeSec,
        power,
        area,
        m_profile.GetHitVFX()
    );

    // Fetch 2Spell4 color variant frames from atlas
    std::vector<AssetTextureData> particleFrames;
    const std::vector<std::string> vfxNames = {
        "2Spell4", "2Spell4Blue", "2Spell4Orange", "2Spell4Purple", "2Spell4Red"
    };

    for (const auto& vfxName : vfxNames)
    {
        AssetTextureData pData = atlas.GetTextureData(vfxName);
        if (pData.texture)
        {
            particleFrames.push_back(pData);
        }
    }

    projectile->SetParticleAtlasFrames(particleFrames);
    projectile->SetOwnerPlayer(&player);
    projectile->SetHitBoxDelay(m_profile.GetHitBoxDelay());
    projectile->SetRepeatInterval(m_profile.GetRepeatInterval() > 0.0f ? m_profile.GetRepeatInterval() : 500.0f);

    projManager.AddProjectile(std::move(projectile));
}
