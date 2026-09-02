#include "../Player.h"
#include "FireballWeapon.h"
#include "../Projectiles/FireballProjectile.h"
#include <random>

#include "../Enemy/EnemyPool.h"

FireballWeapon::FireballWeapon(const WeaponProfile& profile)
    : Weapon(profile)
{
}

sf::Vector2f FireballWeapon::GetTargetPosition(EnemyPool& enemyPool, Player& player)
{
    const auto& activeEnemies = enemyPool.GetActiveEnemies();
    std::vector<EnemyBase*> validEnemies;
    for (auto* enemy : activeEnemies)
    {
        if (enemy && enemy->IsAlive())
        {
            validEnemies.push_back(enemy);
        }
    }

    if (!validEnemies.empty())
    {
        static std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<size_t> dist(0, validEnemies.size() - 1);
        return validEnemies[dist(gen)]->GetPosition();
    }

    // Fallback: Default facing right (1, 0) if player facing is zero
    sf::Vector2f facing = player.GetFacingDirection();
    if (facing.x == 0.0f && facing.y == 0.0f)
    {
        facing = sf::Vector2f(1.0f, 0.0f);
    }
    return player.GetPosition() + facing * 100.0f;
}

float g_FireWandSpeed = 139.06f;
float g_FireWandScale = 1.0f;

void FireballWeapon::FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex)
{
    if (projectileIndex == 0)
    {
        m_burstTargetPosition = targetPosition;
    }
    else
    {
        targetPosition = m_burstTargetPosition;
    }

    // Authentic Unity prefab proof (FireballProjectile.prefab m_Size: {x: 0.31, y: 0.16}) maps to ProjectileFireball2
    AssetTextureData data = atlas.GetTextureData("ProjectileFireball2");
    if(!data.texture) return;

    sf::Vector2f dir = targetPosition - player.GetPosition();
    
    // Normalize target direction
    float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if(length != 0)
    {
        dir.x /= length;
        dir.y /= length;
    }
    else
    {
        dir = player.GetFacingDirection();
        if(dir.x == 0 && dir.y == 0) 
        {
             dir = sf::Vector2f(1.0f, 0.0f);
        }
    }

    // Authentic Fire Wand Angle Offsets (in degrees)
    static const int angleOffsets[] = {
        0, 5, -5, 10, -10, 15, -15, 20, -20, 25, -25, 
        30, -30, 35, -35, 40, -40, 45, -45
    };
    int numOffsets = sizeof(angleOffsets) / sizeof(angleOffsets[0]);
    int offsetDegrees = angleOffsets[projectileIndex % numOffsets];

    // Convert base direction to radians, add offset, and convert back to vector
    float baseAngleRad = std::atan2(dir.y, dir.x);
    float finalAngleRad = baseAngleRad + (offsetDegrees * 3.14159265359f / 180.0f);
    
    sf::Vector2f finalDir(std::cos(finalAngleRad), std::sin(finalAngleRad));

    float speed = g_FireWandSpeed * m_profile.GetSpeed() * player.GetProjectileSpeedMultiplier();
    sf::Vector2f velocity = finalDir * speed;
    
    // Hardcode duration to 5.0s for Fireball (or read from profile if added)
    float duration = 5.0f;
    if(duration == 0.0f) duration = 3.0f; // Default duration
    
    float power = m_profile.GetPower();
    float area = m_profile.GetArea();
    
    sf::Vector2f spawnPosition = player.GetPosition();

    auto proj = std::make_unique<FireballProjectile>(&projManager, &atlas, *data.texture, data.rect, spawnPosition, velocity, duration, power, area, m_profile.GetHitVFX());
    proj->SetKnockback(m_profile.GetKnockback());
    projManager.AddProjectile(std::move(proj));
}
