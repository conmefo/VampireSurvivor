#include "SantaWaterWeapon.h"
#include "../Player.h"
#include "../Projectiles/SantaWaterProjectile.h"
#include <cmath>
#include <random>
#include <algorithm>

SantaWaterWeapon::SantaWaterWeapon(const WeaponProfile& profile)
    : Weapon(profile)
    , m_angleIndex(0)
    , m_radiusIndex(0)
{
    m_shuffledAngles.resize(SEQUENCE_COUNT);
    m_shuffledRadii.resize(SEQUENCE_COUNT);

    const float minRadius = VIEWPORT_MIN_DIM * RADIUS_MIN_RATIO;
    const float maxRadius = VIEWPORT_MIN_DIM * RADIUS_MAX_RATIO;

    for(int i = 0; i < SEQUENCE_COUNT; i++)
    {
        m_shuffledAngles[i] = (static_cast<float>(i) * 2.0f * 3.14159265f) / SEQUENCE_COUNT;
        m_shuffledRadii[i] = minRadius + (static_cast<float>(i) / SEQUENCE_COUNT) * (maxRadius - minRadius);
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(m_shuffledAngles.begin(), m_shuffledAngles.end(), g);
    std::shuffle(m_shuffledRadii.begin(), m_shuffledRadii.end(), g);
}

void SantaWaterWeapon::ReshuffleTargets()
{
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(m_shuffledRadii.begin(), m_shuffledRadii.end(), g);
}

void SantaWaterWeapon::OnLevelUp()
{
    // Matches original HolyWaterWeapon::LevelUp — reshuffle drop positions each level
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(m_shuffledAngles.begin(), m_shuffledAngles.end(), g);
    std::shuffle(m_shuffledRadii.begin(), m_shuffledRadii.end(), g);
}

sf::Vector2f SantaWaterWeapon::GetTargetPosition(EnemyPool& enemyPool, Player& player)
{
    float angle = m_shuffledAngles[m_angleIndex];
    m_angleIndex = (m_angleIndex + 1) % SEQUENCE_COUNT;

    float radius = m_shuffledRadii[m_radiusIndex];
    m_radiusIndex = (m_radiusIndex + 1) % SEQUENCE_COUNT;

    sf::Vector2f playerPos = player.GetPosition();
    sf::Vector2f targetPos(
        playerPos.x + std::cos(angle) * radius,
        playerPos.y + std::sin(angle) * radius
    );

    return targetPos;
}

void SantaWaterWeapon::FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex)
{
    // Clamp target position inside visible camera view bounds with margin
    const sf::FloatRect viewBounds = projManager.GetViewBounds();
    if(viewBounds.width > 0.0f && viewBounds.height > 0.0f)
    {
        float margin = 50.0f;
        targetPosition.x = std::clamp(targetPosition.x, viewBounds.left + margin, viewBounds.left + viewBounds.width - margin);
        targetPosition.y = std::clamp(targetPosition.y, viewBounds.top + margin, viewBounds.top + viewBounds.height - margin);
    }

    AssetTextureData data = atlas.GetTextureData("HolyWater");
    if(!data.texture || data.rect.width == 0 || data.rect.height == 0)
    {
        data = atlas.GetTextureData("ProjectileBottle");
    }

    if(data.texture)
    {
        float effectiveDuration = m_profile.GetDuration() * player.GetDurationMultiplier();
        float effectivePower = m_profile.GetPower() * player.GetMightMultiplier();
        float effectiveArea = m_profile.GetArea() * player.GetAreaMultiplier();

        auto bottle = std::make_unique<SantaWaterProjectile>(
            &projManager,
            &atlas,
            *data.texture,
            data.rect,
            player.GetPosition(),
            targetPosition,
            effectiveDuration,
            effectivePower,
            effectiveArea,
            m_profile.GetHitBoxDelay(),
            m_profile.GetHitVFX()
        );

        bottle->SetKnockback(m_profile.GetKnockback());
        projManager.AddProjectile(std::move(bottle));
    }
}
