#include "AxeProjectile.h"

AxeProjectile::AxeProjectile(const sf::Texture& texture, const sf::IntRect& textureRect, sf::Vector2f startPosition, sf::Vector2f velocity, float duration, float power, float areaMultiplier, const std::string& hitVfxName, int penetration)
    : Projectile(texture, textureRect, startPosition, velocity, duration, power, areaMultiplier, hitVfxName, penetration)
    , m_rotationSpeed(360.0f) // 360 degrees per second
{
    // Center origin so it rotates correctly
    m_sprite.setOrigin(textureRect.width / 2.0f, textureRect.height / 2.0f);
}

float g_AxeGravity = 625.0f;
float g_AxeShadowDelay = 0.027f; // User tuned exact shadow delay: 0.027s
float g_AxeScale = 1.4f;        // User tuned scale: 1.4x

void AxeProjectile::Update(float dt)
{
    // Apply gravity
    m_velocity.y += g_AxeGravity * dt;

    // Apply continuous rotation
    m_sprite.rotate(m_rotationSpeed * dt);

    // Apply uniform scale slider
    m_sprite.setScale(g_AxeScale, g_AxeScale);

    // Call base update to move sprite and handle lifetime
    Projectile::Update(dt);

    // Update shadow trail component with continuous float delay
    m_shadowTrail.SetDelay(g_AxeShadowDelay);
    m_shadowTrail.Update(m_sprite.getPosition(), m_sprite.getRotation(), m_sprite.getScale(), dt);
}

void AxeProjectile::Draw(sf::RenderTarget& target) const
{
    // Render time-interpolated shadow trail component behind main sprite
    m_shadowTrail.Draw(target, m_sprite);

    // Render main axe sprite on top
    target.draw(m_sprite);
}
