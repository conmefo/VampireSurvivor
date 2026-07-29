#include "../Player.h"
#include "Weapon.h"
#include <limits>

#include "../Enemy/EnemyPool.h"

Weapon::Weapon(const WeaponProfile& profile)
    : m_profile(profile)
    , m_cooldownTimer(0.0f)
{
}

sf::Vector2f Weapon::GetTargetPosition(EnemyPool& enemyPool, Player& player)
{
    // Default implementation: Aim for Nearest Enemy
    sf::Vector2f targetPosition = player.GetPosition();
    float minSqDist = std::numeric_limits<float>::max();
    bool found = false;

    for (auto* enemy : enemyPool.GetActiveEnemies())
    {
        if (enemy && enemy->IsAlive())
        {
            sf::Vector2f diff = enemy->GetPosition() - player.GetPosition();
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
        targetPosition = player.GetPosition() + player.GetFacingDirection() * 100.0f; 
    }

    return targetPosition;
}

float g_AxeAmountOverride = -1.0f;
float g_WhipAmountOverride = -1.0f;

void Weapon::Update(float dt, ProjectileManager& projManager, TextureAtlas& atlas, Player& player, EnemyPool& enemyPool)
{
    if(m_cooldownTimer > 0.0f)
    {
        m_cooldownTimer -= dt;
    }

    if(m_cooldownTimer <= 0.0f)
    {
        int amount = m_profile.GetAmount();
        
        // Tuning override
        if (m_profile.GetId() == "AXE" && g_AxeAmountOverride > 0.0f)
        {
            amount = static_cast<int>(g_AxeAmountOverride);
        }
        else if (m_profile.GetId() == "WHIP" && g_WhipAmountOverride > 0.0f)
        {
            amount = static_cast<int>(g_WhipAmountOverride);
        }

        float repeatSec = static_cast<float>(m_profile.GetRepeatInterval()) / 1000.0f;
        if (m_profile.GetId() == "WHIP")
        {
            repeatSec = 0.09f; // Rapid-fire 90ms delay between whip slashes
        }

        // Calculate target once per burst
        sf::Vector2f targetPosition = GetTargetPosition(enemyPool, player);

        for (int i = 0; i < amount; ++i)
        {
            float delay = static_cast<float>(i) * repeatSec;
            
            if (delay > 0.0f)
            {
                projManager.QueueDelayedAction(delay, [this, &projManager, &atlas, &player, targetPosition, i]() {
                    this->FireOne(projManager, atlas, player, targetPosition, i);
                });
            }
            else
            {
                FireOne(projManager, atlas, player, targetPosition, i);
            }
        }
        
        m_cooldownTimer = static_cast<float>(m_profile.GetInterval()) / 1000.0f;
    }
}

const WeaponProfile& Weapon::GetProfile() const
{
    return m_profile;
}
