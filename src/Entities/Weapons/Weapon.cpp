#include "Weapon.h"
#include <limits>

#include "../Enemy/EnemyPool.h"

Weapon::Weapon(const WeaponProfile& profile)
    : m_profile(profile)
    , m_cooldownTimer(0.0f)
{
}

sf::Vector2f Weapon::GetTargetPosition(EnemyPool& enemyPool, sf::Vector2f playerPosition, sf::Vector2f playerDirection)
{
    // Default implementation: Aim for Nearest Enemy
    sf::Vector2f targetPosition = playerPosition;
    float minSqDist = std::numeric_limits<float>::max();
    bool found = false;

    for (auto* enemy : enemyPool.GetActiveEnemies())
    {
        if (enemy && enemy->IsAlive())
        {
            sf::Vector2f diff = enemy->GetPosition() - playerPosition;
            float sqDist = diff.x * diff.x + diff.y * diff.y;
            if (sqDist < minSqDist)
            {
                minSqDist = sqDist;
                targetPosition = enemy->GetPosition();
                found = true;
            }
        }
    }

    if (!found)
    {
        // Fallback: ApplyPlayerFacingVelocity logic (aim straight ahead if no enemies)
        targetPosition = playerPosition + playerDirection * 100.0f; 
    }

    return targetPosition;
}

void Weapon::Update(float dt, ProjectileManager& projManager, TextureAtlas& atlas, sf::Vector2f playerPosition, sf::Vector2f playerDirection, EnemyPool& enemyPool)
{
    if(m_cooldownTimer > 0.0f)
    {
        m_cooldownTimer -= dt;
    }

    if(m_cooldownTimer <= 0.0f)
    {
        int amount = m_profile.GetAmount();
        float repeatSec = static_cast<float>(m_profile.GetRepeatInterval()) / 1000.0f;

        // Calculate target once per burst
        sf::Vector2f targetPosition = GetTargetPosition(enemyPool, playerPosition, playerDirection);

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

const WeaponProfile& Weapon::GetProfile() const
{
    return m_profile;
}
