#include "../Player.h"
#include "AxeWeapon.h"
#include "../Projectiles/AxeProjectile.h"
#include <cmath>

float g_AxeInitialSpeed = 2.8f;
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

    // Angle = -90.0 + FacingDir * (45.0 / Speed) * index
    float speedMultiplier = m_profile.GetSpeed();
    if (speedMultiplier <= 0.0f) speedMultiplier = 1.0f; // Prevent division by zero

    // The Ghidra math calculates Angle = -90.0 + Dir * (45.0 / Speed) * index
    // fVar11 (Speed) is the Axe's base speed, which is 6.0 Unity units/sec.
    // This gives a 7.5 degree step per Axe, keeping them tightly grouped.
    float unitySpeed = g_AxeInitialSpeed * speedMultiplier; 

    float angleDegrees = -90.0f + facingDir * (45.0f / unitySpeed) * (static_cast<float>(projectileIndex) + g_AxeAmountOffset);
    float angleRadians = angleDegrees * 0.017453292f;

    // Calculate X and Y velocity in Unity space
    float unityXVel = unitySpeed * std::cos(angleRadians);
    float unityYVel = unitySpeed * std::sin(angleRadians) * -1.0f; // Unity Angle -90 means down, so flip it

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
    
    sf::Vector2f spawnPosition = player.GetPosition();
    // Default Axe spawn is slightly above player center
    spawnPosition.y -= 20.0f;

    auto proj = std::make_unique<AxeProjectile>(*data.texture, data.rect, spawnPosition, velocity, duration, power, area, m_profile.GetHitVFX(), m_profile.GetPenetrating());
    proj->SetParticleManager(projManager.GetParticleManager());
    
    projManager.AddProjectile(std::move(proj));
}
