#include "LightningRingWeapon.h"
#include "../Player.h"
#include "../Projectiles/LightningRingProjectile.h"
#include "../Enemy/EnemyPool.h"
#include <random>
#include <cmath>

LightningRingWeapon::LightningRingWeapon(const WeaponProfile& profile)
    : Weapon(profile)
{
}

void LightningRingWeapon::Update(float dt, ProjectileManager& projManager, TextureAtlas& atlas, Player& player, EnemyPool& enemyPool)
{
    m_enemyPool = &enemyPool;
    Weapon::Update(dt, projManager, atlas, player, enemyPool);
}

sf::Vector2f LightningRingWeapon::GetTargetPosition(EnemyPool& enemyPool, Player& player)
{
    // Filter active, alive enemies within screen viewport
    const auto& activeEnemies = enemyPool.GetActiveEnemies();
    std::vector<EnemyBase*> visibleEnemies;

    sf::FloatRect viewBounds;
    // Standard viewport padding allowance
    sf::Vector2f playerPos = player.GetPosition();
    viewBounds = sf::FloatRect(playerPos.x - 640.0f, playerPos.y - 360.0f, 1280.0f, 720.0f);

    for (auto* enemy : activeEnemies)
    {
        if (enemy && enemy->IsAlive())
        {
            if (viewBounds.contains(enemy->GetPosition()))
            {
                visibleEnemies.push_back(enemy);
            }
        }
    }

    static std::mt19937 gen(std::random_device{}());

    // Primary Target: Pick random active visible enemy on screen
    if (!visibleEnemies.empty())
    {
        std::uniform_int_distribution<size_t> dist(0, visibleEnemies.size() - 1);
        return visibleEnemies[dist(gen)]->GetPosition();
    }

    // Fallback Target: Random position within a radius around player (e.g. 100 - 400px)
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159265359f);
    std::uniform_real_distribution<float> radiusDist(100.0f, 350.0f);

    float angle = angleDist(gen);
    float radius = radiusDist(gen);

    return sf::Vector2f(
        playerPos.x + std::cos(angle) * radius,
        playerPos.y + std::sin(angle) * radius
    );
}

void LightningRingWeapon::FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex)
{
    // Use Lightning3 texture specifically as requested
    AssetTextureData data = atlas.GetTextureData("Lightning3");
    if (!data.texture || data.rect.width == 0)
    {
        data = atlas.GetTextureData("Lightning1");
    }
    if (!data.texture || data.rect.width == 0)
    {
        data = atlas.GetTextureData("HolyWater");
    }
    if (!data.texture) return;

    float effectivePower = m_profile.GetPower() * player.GetMightMultiplier();
    float effectiveArea = m_profile.GetArea() * player.GetAreaMultiplier();

    // Use global tuning duration slider value
    float duration = g_LightningRingDuration;

    auto projectile = std::make_unique<LightningRingProjectile>(
        &projManager,
        &atlas,
        m_enemyPool,
        *data.texture,
        data.rect,
        targetPosition,
        duration,
        effectivePower,
        effectiveArea,
        m_profile.GetHitVFX()
    );

    projManager.AddProjectile(std::move(projectile));
}
