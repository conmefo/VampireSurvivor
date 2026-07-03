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

        for (int i = 0; i < amount; ++i)
        {
            float delay = static_cast<float>(i) * repeatSec;
            
            sf::Vector2f targetPosition = playerPosition + playerDirection * 100.0f; // Default

            if (!validEnemies.empty())
            {
                if (i == 0)
                {
                    // Closest enemy
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
                else
                {
                    // Random enemy
                    static std::mt19937 gen(std::random_device{}());
                    std::uniform_int_distribution<size_t> dist(0, validEnemies.size() - 1);
                    targetPosition = validEnemies[dist(gen)]->GetPosition();
                }
            }

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
    
    float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if(length != 0)
    {
        dir.x /= length;
        dir.y /= length;
    }
    else
    {
        dir = playerDirection;
        if(dir.x == 0 && dir.y == 0) 
        {
             dir = sf::Vector2f(1.0f, 0.0f);
        }
    }

    float speed = m_profile.GetSpeed() * 250.0f; // Base speed pixel multiplier
    sf::Vector2f velocity = dir * speed;
    
    // Hardcoded default duration of 2.25s per the WEAPON_DATA.json we found
    float duration = 2.25f;

    float power = m_profile.GetPower();
    float area = m_profile.GetArea();
    
    sf::Vector2f spawnPosition = playerPosition;

    auto proj = std::make_unique<RunetracerProjectile>(&projManager, *data.texture, data.rect, spawnPosition, velocity, duration, power, area, m_profile.GetHitVFX(), m_profile.GetPenetrating());
    projManager.AddProjectile(std::move(proj));
}
