#include "KnifeWeapon.h"
#include "../Player.h"
#include "../Projectiles/KnifeProjectile.h"
#include <iostream>
#include <cmath>

KnifeWeapon::KnifeWeapon(const WeaponProfile& profile)
    : Weapon(profile)
    , m_lastFacingDir(1.0f, 0.0f)
{
}

sf::Vector2f KnifeWeapon::GetTargetPosition(EnemyPool& enemyPool, Player& player)
{
    (void)enemyPool; // Unused for Knife
    
    sf::Vector2f facingDir = player.GetFacingDirection();
    if(facingDir.x != 0.0f || facingDir.y != 0.0f)
    {
        m_lastFacingDir = facingDir;
    }
    
    // Always aim straight in the last faced direction
    return player.GetPosition() + m_lastFacingDir * 100.0f;
}

void KnifeWeapon::FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex)
{
    (void)projectileIndex;

    sf::Vector2f targetDir = m_lastFacingDir;
    
    // Safety check just in case targetPosition is passed dynamically
    if(targetPosition != player.GetPosition())
    {
        sf::Vector2f diff = targetPosition - player.GetPosition();
        float len = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        if(len > 0.0001f)
        {
            targetDir = diff / len;
        }
    }

    // Offset the spawn position based on the projectile index if needed
    // In standard VS, they spawn at the player center and just fire straight.
    sf::Vector2f spawnPos = player.GetPosition();

    AssetTextureData data = atlas.GetTextureData("ProjectileKnife3");

    if(!data.texture)
    {
        std::cerr << "KnifeWeapon: Failed to find projectile frame ProjectileKnife3 in atlas!\n";
        return;
    }

    constexpr float BASE_PROJECTILE_SPEED = 300.0f; // Knife is fast
    float speed = BASE_PROJECTILE_SPEED * m_profile.GetSpeed() * player.GetProjectileSpeedMultiplier();
    sf::Vector2f velocity = targetDir * speed;

    constexpr float LIFETIME = 3.0f;
    float power = m_profile.GetPower();
    float area = m_profile.GetArea();
    int penetration = m_profile.GetPenetrating();

    auto projectile = std::make_unique<KnifeProjectile>(
        *data.texture,
        data.rect,
        spawnPos,
        velocity,
        LIFETIME,
        power,
        area,
        "Default",
        penetration
    );

    projectile->SetKnockback(m_profile.GetKnockback());
    projManager.AddProjectile(std::move(projectile));
}
