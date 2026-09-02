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

    float offsetDistance = 65.0f * area;
    float offsetX = strikeLeft ? -offsetDistance : offsetDistance;
    
    // Y offset positioning: Backward strikes (!isForward) render higher up, and subsequent index >= 2 strikes stagger higher
    float offsetY = 0.0f;
    if (!isForward)
    {
        offsetY -= 15.0f * area;
    }

    if(projectileIndex >= 2)
    {
        offsetY -= 40.0f * (static_cast<float>(projectileIndex / 2) * area);
    }

    sf::Vector2f relativeOffset(offsetX, offsetY);

    // Flipped sprite configurations
    bool flipX = strikeLeft;
    bool flipY = !isForward; // Vertically flip backward slashes for backhand arc shape

    auto projectile = std::make_unique<WhipProjectile>(
        *data.texture,
        data.rect,
        &player,
        relativeOffset,
        duration,
        power,
        area,
        m_profile.GetHitVFX(),
        flipX,
        flipY
    );

    projectile->SetKnockback(m_profile.GetKnockback());
    projManager.AddProjectile(std::move(projectile));
}
