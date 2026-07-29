#include "WhipProjectile.h"
#include "../Player.h"
#include <algorithm>

WhipProjectile::WhipProjectile(const sf::Texture& texture, const sf::IntRect& textureRect, Player* player, sf::Vector2f relativeOffset, float duration, float power, float areaMultiplier, const std::string& hitVfxName, bool flipX, bool flipY)
    : Projectile(texture, textureRect, player ? (player->GetPosition() + relativeOffset) : sf::Vector2f(0.0f, 0.0f), sf::Vector2f(0.0f, 0.0f), duration, power, areaMultiplier, hitVfxName)
    , m_player(player)
    , m_relativeOffset(relativeOffset)
    , m_startOffset(relativeOffset)
    , m_maxDuration(duration)
{
    // Center origin of m_sprite
    sf::FloatRect bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    
    if (m_player)
    {
        m_sprite.setPosition(m_player->GetPosition() + m_relativeOffset);
    }

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

    // Set initial size
    m_sprite.setScale(m_baseScaleX * 0.6f, m_baseScaleY * 0.6f);
}

void WhipProjectile::Update(float dt)
{
    // Call base class Update to decrement base class m_duration
    Projectile::Update(dt);

    m_elapsedTime += dt;

    if (m_player)
    {
        // Follow player position dynamically so slash doesn't float static in the world
        m_sprite.setPosition(m_player->GetPosition() + m_relativeOffset);
    }

    // Phase 1: Fast Scale Up (0.0s to 0.08s) -> Linear scaling from 0.6x to 1.0x, opacity remains at max (235.0f).
    // Phase 2: Fade Out (0.08s to 0.20s) -> Scale remains at 1.0x, opacity fades from 235.0f to 0.
    float scaleFactor = 1.0f;
    float alphaFactor = 235.0f;

    if (m_elapsedTime < 0.08f)
    {
        float ratio = m_elapsedTime / 0.08f;
        // Linear scale-up
        scaleFactor = 0.6f + 0.4f * ratio;
        alphaFactor = 235.0f;
    }
    else
    {
        scaleFactor = 1.0f;
        // Fade out over the remaining 0.12s (0.08s to 0.20s)
        float ratio = (m_elapsedTime - 0.08f) / 0.12f;
        ratio = std::max(0.0f, std::min(1.0f, ratio));
        alphaFactor = 235.0f * (1.0f - ratio);
    }

    m_sprite.setScale(m_baseScaleX * scaleFactor, m_baseScaleY * scaleFactor);
    m_sprite.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alphaFactor)));
}

void WhipProjectile::Draw(sf::RenderTarget& target) const
{
    // Draw using the base class draw if not expired
    if(m_duration > 0.0f)
    {
        target.draw(m_sprite);
    }
}
