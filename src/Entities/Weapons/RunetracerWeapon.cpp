#include "RunetracerWeapon.h"
#include "../Projectiles/RunetracerProjectile.h"
#include "../Enemy/EnemyPool.h"
#include <random>
#include <limits>

RunetracerWeapon::RunetracerWeapon(const WeaponProfile& profile)
    : Weapon(profile)
{
}

void RunetracerWeapon::Update(float dt, ProjectileManager& projManager, TextureAtlas& atlas, sf::Vector2f playerPosition, sf::Vector2f playerDirection, EnemyPool& enemyPool)
{
    if(m_cooldownTimer > 0.0f)
    {
        m_cooldownTimer -= dt;
    }

    if(m_cooldownTimer <= 0.0f)
    {
        int amount = m_profile.GetAmount();
        float repeatSec = static_cast<float>(m_profile.GetRepeatInterval()) / 1000.0f;

        const auto& activeEnemies = enemyPool.GetActiveEnemies();
        std::vector<EnemyBase*> validEnemies;
        for (auto* enemy : activeEnemies)
        {
            if (enemy && enemy->IsAlive())
            {
                validEnemies.push_back(enemy);
            }
        }

        sf::Vector2f targetPosition = playerPosition + playerDirection * 100.0f; // Default

        if (!validEnemies.empty())
        {
            // Closest enemy for the entire burst
            float minSqDist = std::numeric_limits<float>::max();
            for (auto* enemy : validEnemies)
            {
                sf::Vector2f diff = enemy->GetPosition() - playerPosition;
                float sqDist = diff.x * diff.x + diff.y * diff.y;
                if (sqDist < minSqDist)
                {
                    minSqDist = sqDist;
                    targetPosition = enemy->GetPosition();
                }
            }
        }

        for (int i = 0; i < amount; ++i)
        {
            float delay = static_cast<float>(i) * repeatSec;

            if (delay > 0.0f)
            {
                projManager.QueueDelayedAction(delay, [this, &projManager, &atlas, playerPosition, playerDirection, targetPosition, i]() {
                    this->FireOne(projManager, atlas, playerPosition, playerDirection, targetPosition, i);
                });
            }
            else
            {
                FireOne(projManager, atlas, playerPosition, playerDirection, targetPosition, i);
            }
        }
        
        m_cooldownTimer = static_cast<float>(m_profile.GetInterval()) / 1000.0f;
    }
}

void RunetracerWeapon::FireOne(ProjectileManager& projManager, TextureAtlas& atlas, sf::Vector2f playerPosition, sf::Vector2f playerDirection, sf::Vector2f targetPosition, int projectileIndex)
{
    AssetTextureData data = atlas.GetTextureData(m_profile.GetFrameName());
    if(!data.texture) return;

    sf::Vector2f dir = targetPosition - playerPosition;
    float baseAngleRadians = 0.0f;
    
    if (dir.x == 0 && dir.y == 0)
    {
        dir = playerDirection;
        if(dir.x == 0 && dir.y == 0) 
        {
             dir = sf::Vector2f(1.0f, 0.0f);
        }
    }
    
    baseAngleRadians = std::atan2(dir.y, dir.x);

    // Authentic Spread Array from Ghidra
    static const int spreadAngles[] = { 0, 10, -10, 20, -20, 30, -30, 40, -40 };
    int spreadAnglesCount = sizeof(spreadAngles) / sizeof(spreadAngles[0]);
    
    int angleOffset = spreadAngles[projectileIndex % spreadAnglesCount];
    
    // Math.PI / 180 is approx 0.017453292f
    float finalAngleRadians = baseAngleRadians + static_cast<float>(angleOffset) * 0.017453292f;

    // Convert back to directional vector
    dir.x = std::cos(finalAngleRadians);
    dir.y = std::sin(finalAngleRadians);

    float speed = m_profile.GetSpeed() * 250.0f; // Base speed pixel multiplier
    sf::Vector2f velocity = dir * speed;
    
    // Hardcoded default duration of 2.25s per the WEAPON_DATA.json we found
    float duration = 2.25f;

    float power = m_profile.GetPower();
    float area = m_profile.GetArea();
    
    sf::Vector2f spawnPosition = playerPosition;

    const vs::ParticleEmitterConfig* trailConfig = nullptr;
    if (projManager.GetParticleManager())
    {
        trailConfig = &projManager.GetParticleManager()->GetConfig("runetracerTrail");
    }

    auto proj = std::make_unique<RunetracerProjectile>(*data.texture, data.rect, spawnPosition, velocity, duration, power, area, m_profile.GetHitVFX(), m_profile.GetPenetrating(), &projManager, trailConfig);
    projManager.AddProjectile(std::move(proj));
}
