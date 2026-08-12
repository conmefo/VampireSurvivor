#include "../Player.h"
#include "MagicMissileWeapon.h"
#include "../Projectiles/MagicMissileProjectile.h"
#include <iostream>
#include <cmath>

MagicMissileWeapon::MagicMissileWeapon(const WeaponProfile& profile)
    : Weapon(profile)
    , m_lastFacingDir(1.0f, 0.0f)
{
}

float g_MagicWandSpeed = 175.0f;
float g_MagicWandScale = 0.45f;

void MagicMissileWeapon::FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex)
{
    (void)projectileIndex;

    // Update last non-zero direction
    if(player.GetFacingDirection() != sf::Vector2f(0.0f, 0.0f))
    {
        m_lastFacingDir = player.GetFacingDirection();
    }

    sf::Vector2f targetDir = m_lastFacingDir;
    if(targetPosition != player.GetPosition())
    {
        sf::Vector2f diff = targetPosition - player.GetPosition();
        float len = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        if(len > 0.0001f)
        {
            targetDir = diff / len;
        }
    }

    std::string frame1 = "ProjectileHoly1";
    std::string frame2 = "ProjectileHoly1";

    if(m_profile.GetBulletType() == "HOLY_MISSILE")
    {
        frame1 = "ProjectileHoly1";
        frame2 = "ProjectileHoly1";
    }

    AssetTextureData data1 = atlas.GetTextureData(frame1);
    AssetTextureData data2 = atlas.GetTextureData(frame2);

    if(!data1.texture)
    {
        std::cerr << "MagicMissileWeapon: Failed to find projectile frames in atlas!\n";
        return;
    }

    float speed = g_MagicWandSpeed * m_profile.GetSpeed() * player.GetProjectileSpeedMultiplier();
    sf::Vector2f velocity = targetDir * speed;

    constexpr float LIFETIME = 5.0f;
    float power = m_profile.GetPower();
    float area = m_profile.GetArea();
    int penetration = m_profile.GetPenetrating();

    // Add subtle randomized spawn offset jitter so burst missiles don't stack on top of each other
    float jitterScale = std::min(1.0f, static_cast<float>(projectileIndex) * 0.5f);
    float jitterX = (((float)rand() / (float)RAND_MAX) - 0.5f) * 16.0f * jitterScale;
    float jitterY = (((float)rand() / (float)RAND_MAX) - 0.5f) * 16.0f * jitterScale;
    sf::Vector2f spawnPos = player.GetPosition() + sf::Vector2f(jitterX, jitterY);

    auto projectile = std::make_unique<MagicMissileProjectile>(
        *data1.texture,
        data1.rect,
        data2.rect,
        spawnPos,
        velocity,
        LIFETIME,
        power,
        area,
        "Default", // Matches ID 1 (HitStar2 + Shockwave3)
        penetration
    );

    projManager.AddProjectile(std::move(projectile));
}
