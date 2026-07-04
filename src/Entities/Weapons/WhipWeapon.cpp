#include "../Player.h"
#include "WhipWeapon.h"
#include "../Projectiles/WhipProjectile.h"
#include <iostream>

WhipWeapon::WhipWeapon(const WeaponProfile& profile)
    : Weapon(profile)
    , m_facingLeft(false) // Default to facing right
{
}

void WhipWeapon::FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex)
{
    (void)targetPosition;

    // Update last facing direction
    if(player.GetFacingDirection().x < 0.0f)
    {
        m_facingLeft = true;
    }
    else if(player.GetFacingDirection().x > 0.0f)
    {
        m_facingLeft = false;
    }

    float area = m_profile.GetArea();
    float power = m_profile.GetPower();
    // Default duration of the whip slash is 0.15s - 0.2s
    float duration = 0.2f;

    // Use "slash" from vfx_atlas
    AssetTextureData data = atlas.GetTextureData("slash");
    if(!data.texture)
    {
        std::cerr << "WhipWeapon: Failed to find 'slash' texture slice in atlas!\n";
        return;
    }

    bool isForward = (projectileIndex % 2 == 0);
    bool strikeLeft = isForward ? m_facingLeft : !m_facingLeft;

    float offsetDistance = 85.0f * area;
    float offsetX = strikeLeft ? -offsetDistance : offsetDistance;
    
    // Stagger vertical height for higher amount of strikes (e.g. index 2 and 3 strike higher)
    float offsetY = 0.0f;
    if(projectileIndex >= 2)
    {
        offsetY = -40.0f * (static_cast<float>(projectileIndex / 2) * area);
    }

    sf::Vector2f spawnPos = player.GetPosition() + sf::Vector2f(offsetX, offsetY);

    // Flipped sprite configurations
    bool flipX = strikeLeft;
    bool flipY = false;

    auto projectile = std::make_unique<WhipProjectile>(
        *data.texture,
        data.rect,
        spawnPos,
        duration,
        power,
        area,
        m_profile.GetHitVFX(),
        flipX,
        flipY
    );

    projManager.AddProjectile(std::move(projectile));
}
