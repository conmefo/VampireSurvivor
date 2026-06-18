#include "WhipProjectile.h"

WhipProjectile::WhipProjectile(const sf::Texture& texture, const sf::IntRect& textureRect, sf::Vector2f startPosition, float duration, float power, float areaMultiplier, const std::string& hitVfxName, bool flipX, bool flipY)
    : Projectile(texture, textureRect, startPosition, sf::Vector2f(0.0f, 0.0f), duration, power, areaMultiplier, hitVfxName)
    , m_maxDuration(duration)
    , m_scaleTween(0.8f, 1.2f, duration, MathUtils::EaseType::Quad, MathUtils::EaseMode::Out)
    , m_alphaTween(255.0f, 0.0f, duration, MathUtils::EaseType::Quad, MathUtils::EaseMode::Out)
{
    // The base class constructor initialized m_sprite, m_duration, etc.
    // Center origin of m_sprite
    sf::FloatRect bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    m_sprite.setPosition(startPosition);

    m_baseScaleX = areaMultiplier;
    m_baseScaleY = areaMultiplier;

    if(flipX)
    {
        m_baseScaleX = -m_baseScaleX;
    }
    if(flipY)
    {
        m_baseScaleY = -m_baseScaleY;
    }

    m_sprite.setScale(m_baseScaleX * 0.8f, m_baseScaleY * 0.8f);

    m_scaleTween.Start();
    m_alphaTween.Start();
}

void WhipProjectile::Update(float dt)
{
    // Call base class Update to decrement base class m_duration
    Projectile::Update(dt);

    m_scaleTween.Update(dt);
    m_alphaTween.Update(dt);

    float scaleVal = m_scaleTween.GetValue();
    float alphaVal = m_alphaTween.GetValue();

    m_sprite.setScale(m_baseScaleX * scaleVal, m_baseScaleY * scaleVal);
    m_sprite.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alphaVal)));
}

void WhipProjectile::Draw(sf::RenderTarget& target) const
{
    // Draw using the base class draw if not expired
    if(m_duration > 0.0f)
    {
        target.draw(m_sprite);
    }
}
