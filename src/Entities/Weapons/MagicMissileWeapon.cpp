#include "../Player.h"
#include "MagicMissileWeapon.h"
#include "../Projectiles/MagicMissileProjectile.h"
#include <iostream>
#include <cmath>

MagicMissileWeapon::MagicMissileWeapon(const WeaponProfile& profile)
    : Weapon(profile)
    , m_lastFacingDir(1.0f, 0.0f)
{
}

void MagicMissileWeapon::FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex)
{
    (void)projectileIndex;

    // Update last non-zero direction
    if(player.GetFacingDirection() != sf::Vector2f(0.0f, 0.0f))
    {
        m_lastFacingDir = player.GetFacingDirection();
    }

    sf::Vector2f targetDir = m_lastFacingDir;
    if(targetPosition != player.GetPosition())
    {
        sf::Vector2f diff = targetPosition - player.GetPosition();
        float len = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        if(len > 0.0001f)
        {
            targetDir = diff / len;
        }
    }

    std::string frame1 = "ProjectileHoly1";
    std::string frame2 = "ProjectileHoly1";

    if(m_profile.GetBulletType() == "HOLY_MISSILE")
    {
        frame1 = "ProjectileHoly1";
        frame2 = "ProjectileHoly1";
    }

    AssetTextureData data1 = atlas.GetTextureData(frame1);
    AssetTextureData data2 = atlas.GetTextureData(frame2);

    if(!data1.texture)
    {
        std::cerr << "MagicMissileWeapon: Failed to find projectile frames in atlas!\n";
        return;
    }

    // Increased projectile speed to 200 for normal gameplay testing
    constexpr float BASE_PROJECTILE_SPEED = 280.0f;
    float speed = BASE_PROJECTILE_SPEED * m_profile.GetSpeed();
    sf::Vector2f velocity = targetDir * speed;

    constexpr float LIFETIME = 5.0f;
    float power = m_profile.GetPower();
    float area = m_profile.GetArea();
    int penetration = m_profile.GetPenetrating();

    auto projectile = std::make_unique<MagicMissileProjectile>(
        *data1.texture,
        data1.rect,
        data2.rect,
        player.GetPosition(),
        velocity,
        LIFETIME,
        power,
        area,
        "Default", // Matches ID 1 (HitStar2 + Shockwave3)
        penetration
    );

    projManager.AddProjectile(std::move(projectile));
}
