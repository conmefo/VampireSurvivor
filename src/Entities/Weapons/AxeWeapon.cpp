#include "../Player.h"
#include "AxeWeapon.h"
#include "../Projectiles/AxeProjectile.h"
#include <cmath>

float g_AxeInitialSpeed = 3.5f;
float g_AxeAmountOffset = 0.0f;

AxeWeapon::AxeWeapon(const WeaponProfile& profile)
    : Weapon(profile)
{
}

void AxeWeapon::FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex)
{
    AssetTextureData data = atlas.GetTextureData(m_profile.GetFrameName());
    if(!data.texture) return;

    // Determine facing direction (1 for right, -1 for left)
    sf::Vector2f playerDir = player.GetFacingDirection();
    float facingDir = (playerDir.x < 0.0f) ? -1.0f : 1.0f;

    // Base angle straight up (-90 deg)
    // When multiple axes fire, center them or expand in facing direction:
    // If facing right (+1), offset goes right; if facing left (-1), offset goes left
    float speedMultiplier = m_profile.GetSpeed();
    if (speedMultiplier <= 0.0f) speedMultiplier = 1.0f;

    float unitySpeed = g_AxeInitialSpeed * speedMultiplier; 

    // Directional spread step (7.5 deg base step)
    float stepAngle = (45.0f / unitySpeed);
    float angleDegrees = -90.0f + facingDir * stepAngle * (static_cast<float>(projectileIndex) + g_AxeAmountOffset);
    float angleRadians = angleDegrees * 0.017453292f;

    // Calculate X and Y velocity in Unity space
    // X velocity direction is explicitly driven by facingDir
    float unityXVel = std::abs(unitySpeed * std::cos(angleRadians)) * facingDir;
    if (projectileIndex > 0) {
        unityXVel += facingDir * (static_cast<float>(projectileIndex) * 0.5f);
    }
    float unityYVel = std::abs(unitySpeed * std::sin(angleRadians)); // Upward velocity positive in Unity

    // The genius Ghidra clamp: all axes get exactly 6.0 upward velocity so they hang in the air for the exact same time!
    if (unityYVel > 6.0f) {
        unityYVel = 6.0f;
    }

    // Convert Unity velocity to SFML pixels/sec (100x scale)
    // Unity positive Y is UP. SFML positive Y is DOWN. So we invert Y.
    sf::Vector2f velocity;
    velocity.x = unityXVel * 100.0f;
    velocity.y = -unityYVel * 100.0f;
    
    // Axe usually lasts until it falls off screen (gravity pulls it down rapidly)
    float duration = 4.0f; 
    float power = m_profile.GetPower();
    float area = m_profile.GetArea();
    
    // In original Unity game, projectile spawns at middle of player's lowest edge (feet)
    sf::Vector2f spawnPosition = player.GetBottomPosition();

    auto proj = std::make_unique<AxeProjectile>(*data.texture, data.rect, spawnPosition, velocity, duration, power, area, m_profile.GetHitVFX(), m_profile.GetPenetrating());
    proj->SetParticleManager(projManager.GetParticleManager());
    
    projManager.AddProjectile(std::move(proj));
}
