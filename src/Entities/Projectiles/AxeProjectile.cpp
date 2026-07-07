#include "AxeProjectile.h"

AxeProjectile::AxeProjectile(const sf::Texture& texture, const sf::IntRect& textureRect, sf::Vector2f startPosition, sf::Vector2f velocity, float duration, float power, float areaMultiplier, const std::string& hitVfxName, int penetration)
    : Projectile(texture, textureRect, startPosition, velocity, duration, power, areaMultiplier, hitVfxName, penetration)
    , m_rotationSpeed(360.0f) // 360 degrees per second
{
    // Center origin so it rotates correctly
    m_sprite.setOrigin(textureRect.width / 2.0f, textureRect.height / 2.0f);
}

float g_AxeGravity = 803.9f;

void AxeProjectile::Update(float dt)
{
    // Apply gravity to Y velocity (Y is down in SFML, so adding gravity pulls it down)
    m_velocity.y += g_AxeGravity * dt;

    // Apply continuous rotation
    m_sprite.rotate(m_rotationSpeed * dt);

    // Call base update to move sprite and handle lifetime
    Projectile::Update(dt);
}
