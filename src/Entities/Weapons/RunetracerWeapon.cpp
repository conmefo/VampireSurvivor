#include "../Player.h"
#include "RunetracerWeapon.h"
#include "../Projectiles/RunetracerProjectile.h"
#include "../Enemy/EnemyPool.h"
#include "../../Core/Audio/IAudioService.h"
#include <random>
#include <limits>

RunetracerWeapon::RunetracerWeapon(const WeaponProfile& profile)
    : Weapon(profile)
{
}

void RunetracerWeapon::Update(float dt, ProjectileManager& projManager, TextureAtlas& atlas, Player& player, EnemyPool& enemyPool)
{
    if(m_cooldownTimer > 0.0f)
    {
        m_cooldownTimer -= dt;
    }

    if(m_cooldownTimer <= 0.0f)
    {
        // Audio: play weapon fire SFX
        if (m_audioService && m_fireSfxId != SfxID::None)
        {
            PlaySoundOptions opts;
            opts.position = player.GetPosition();
            opts.isSpatial = true;
            opts.priority = AudioPriority::High;
            m_audioService->PlaySfx(m_fireSfxId, opts);
        }

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

        sf::Vector2f targetPosition = player.GetPosition() + player.GetFacingDirection() * 100.0f; // Default

        if (!validEnemies.empty())
        {
            // Closest enemy for the entire burst
            float minSqDist = std::numeric_limits<float>::max();
            for (auto* enemy : validEnemies)
            {
                sf::Vector2f diff = enemy->GetPosition() - player.GetPosition();
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

void RunetracerWeapon::FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex)
{
    AssetTextureData data = atlas.GetTextureData(m_profile.GetFrameName());
    if(!data.texture) return;

    sf::Vector2f dir = targetPosition - player.GetPosition();
    float baseAngleRadians = 0.0f;
    
    if (dir.x == 0 && dir.y == 0)
    {
        dir = player.GetFacingDirection();
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
    
    sf::Vector2f spawnPosition = player.GetBottomPosition();

    const vs::ParticleEmitterConfig* trailConfig = nullptr;
    if (projManager.GetParticleManager())
    {
        trailConfig = &projManager.GetParticleManager()->GetConfig("runetracerTrail");
    }

    // Predefined palette matching original game gem colors (including original silver)
    static const sf::Color colors[] = {
        sf::Color(255, 255, 255),   // Original Silver
        sf::Color(255, 90, 90),     // Pink/Red
        sf::Color(204, 255, 255),   // Cyan/Blue
        sf::Color(90, 255, 90),     // Green
        sf::Color(255, 204, 90),    // Yellow/Orange
        sf::Color(204, 90, 255)     // Purple
    };
    static const int colorsCount = sizeof(colors) / sizeof(colors[0]);

    // Randomize the color selection so that single projectile launches are diverse
    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, colorsCount - 1);
    sf::Color projectileColor = colors[dist(gen)];

    auto proj = std::make_unique<RunetracerProjectile>(*data.texture, data.rect, spawnPosition, velocity, duration, power, area, m_profile.GetHitVFX(), m_profile.GetPenetrating(), &projManager, trailConfig, projectileColor);
    projManager.AddProjectile(std::move(proj));
}
