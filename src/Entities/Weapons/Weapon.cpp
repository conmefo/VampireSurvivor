#include "../Player.h"
#include "Weapon.h"
#include <limits>

#include "../Enemy/EnemyPool.h"
#include "../../Core/Audio/IAudioService.h"

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
float g_SantaWaterAmountOverride = -1.0f;

void Weapon::Update(float dt, ProjectileManager& projManager, TextureAtlas& atlas, Player& player, EnemyPool& enemyPool)
{
    if(m_cooldownTimer > 0.0f)
    {
        m_cooldownTimer -= dt;
    }

    if(m_cooldownTimer <= 0.0f)
    {
        // Audio: play weapon fire SFX once per burst
        if (m_audioService && m_fireSfxId != SfxID::None)
        {
            PlaySoundOptions opts;
            opts.position = player.GetPosition();
            opts.isSpatial = true;
            opts.priority = AudioPriority::High;
            m_audioService->PlaySfx(m_fireSfxId, opts);
        }

        int amount = m_profile.GetAmount() + player.GetBonusAmount();
        
        // Tuning override
        if (m_profile.GetId() == "AXE" && g_AxeAmountOverride > 0.0f)
        {
            amount = static_cast<int>(g_AxeAmountOverride);
        }
        else if (m_profile.GetId() == "WHIP" && g_WhipAmountOverride > 0.0f)
        {
            amount = static_cast<int>(g_WhipAmountOverride);
        }
        else if (m_profile.GetId() == "HOLYWATER" && g_SantaWaterAmountOverride > 0.0f)
        {
            amount = static_cast<int>(g_SantaWaterAmountOverride);
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
                projManager.QueueDelayedAction(delay, [this, &projManager, &atlas, &player, &enemyPool, i]() {
                    sf::Vector2f realTimeTarget = this->GetTargetPosition(enemyPool, player);
                    this->FireOne(projManager, atlas, player, realTimeTarget, i);
                });
            }
            else
            {
                sf::Vector2f realTimeTarget = GetTargetPosition(enemyPool, player);
                FireOne(projManager, atlas, player, realTimeTarget, i);
            }
        }
        
        float baseCooldown = static_cast<float>(m_profile.GetInterval()) / 1000.0f;
        m_cooldownTimer = std::max(0.1f, baseCooldown * player.GetCooldownMultiplier());
    }
}

const WeaponProfile& Weapon::GetProfile() const
{
    return m_profile;
}

void Weapon::SetLevelDeltas(const std::vector<WeaponLevelDelta>& deltas)
{
    m_levelDeltas = deltas;
}

void Weapon::SetObserver(IWeaponLevelObserver* observer)
{
    m_observer = observer;
}

bool Weapon::CanLevelUp() const
{
    return m_profile.GetCurrentLevel() - 1 < static_cast<int>(m_levelDeltas.size());
}

bool Weapon::CanLevelDown() const
{
    return m_profile.GetCurrentLevel() > 1;
}

void Weapon::LevelDown()
{
    if(!CanLevelDown())
    {
        return;
    }

    // Revert the delta that was applied at (currentLevel - 1) → index (currentLevel - 2)
    const WeaponLevelDelta& delta = m_levelDeltas[m_profile.GetCurrentLevel() - 2];
    m_profile.RevertDelta(delta);
}

void Weapon::LevelUp()
{
    if(!CanLevelUp())
    {
        return;
    }

    const WeaponLevelDelta& delta = m_levelDeltas[m_profile.GetCurrentLevel() - 1];
    m_profile.ApplyDelta(delta);

    // Notify observer if this level triggers an evolution
    if(!delta.addEvolvedWeapon.empty() && m_observer)
    {
        m_observer->OnWeaponEvolution(delta.addEvolvedWeapon);
    }

    // Weapon-specific hook (e.g. SantaWaterWeapon reshuffles drop positions)
    OnLevelUp();
}
