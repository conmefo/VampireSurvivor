#include "RunetracerProjectile.h"
#include "ProjectileManager.h"
#include <cmath>

RunetracerProjectile::RunetracerProjectile(const sf::Texture& texture, const sf::IntRect& textureRect, sf::Vector2f startPosition, sf::Vector2f velocity, float duration, float power, float areaMultiplier, const std::string& hitVfxName, int penetration, ProjectileManager* projManager, const vs::ParticleEmitterConfig* config, sf::Color customColor)
    : Projectile(texture, textureRect, startPosition, velocity, duration, power, areaMultiplier, hitVfxName, penetration)
    , m_projManager(projManager)
    , m_particleSpawnTimer(0.0f)
    , m_colorHue(0.0f)
    , m_customColor(customColor)
{
    m_penetration = penetration;
    m_sprite.setOrigin(textureRect.width / 2.0f, textureRect.height / 2.0f);

    // Diamond sprite always remains clean silver/gray (untinted)
    m_sprite.setColor(sf::Color::White);

    m_trailRenderer = std::make_unique<TrailRenderer>(0.8f, 15.0f, 2.0f); // defaults
    if (m_trailRenderer)
    {
        if (config)
        {
            // Smaller diamond gem head scale (0.8x Area)
            m_sprite.setScale(0.8f * areaMultiplier, 0.8f * areaMultiplier);
            // Thinner trail width (1.2f Area)
            m_trailRenderer->SetWidth(1.2f * areaMultiplier);
            
            // Much longer trail (1.5s lifetime)
            m_trailRenderer->SetFadeStartRatio(0.0f);
            m_trailRenderer->SetMaxLifetime(1.5f);
            
            // Slightly more visible trail color (blend 50% of target color with 50% white)
            sf::Color softTrailColor(
                static_cast<sf::Uint8>(0.5f * 255.0f + 0.5f * customColor.r),
                static_cast<sf::Uint8>(0.5f * 255.0f + 0.5f * customColor.g),
                static_cast<sf::Uint8>(0.5f * 255.0f + 0.5f * customColor.b)
            );
            
            // Set base alpha to 165.0f to match original game trails
            m_baseAlpha = 165.0f;
            softTrailColor.a = static_cast<sf::Uint8>(m_baseAlpha);
            
            m_trailRenderer->SetBaseColor(softTrailColor);
            m_customColor = softTrailColor;
        }
        else
        {
            // Fallback defaults if config is missing
            m_sprite.setScale(0.8f * areaMultiplier, 0.8f * areaMultiplier);
            m_trailRenderer->SetWidth(1.2f * areaMultiplier);
            m_trailRenderer->SetFadeStartRatio(0.0f);
            m_trailRenderer->SetMaxLifetime(1.5f);
            
            sf::Color softTrailColor(
                static_cast<sf::Uint8>(0.5f * 255.0f + 0.5f * customColor.r),
                static_cast<sf::Uint8>(0.5f * 255.0f + 0.5f * customColor.g),
                static_cast<sf::Uint8>(0.5f * 255.0f + 0.5f * customColor.b)
            );
            
            m_baseAlpha = 165.0f;
            softTrailColor.a = 165;
            
            m_trailRenderer->SetBaseColor(softTrailColor);
            m_customColor = softTrailColor;
        }
    }
}

void RunetracerProjectile::Update(float dt)
{
    // 1. Tick hit timers for enemies
    for (auto it = m_enemyHitTimers.begin(); it != m_enemyHitTimers.end(); )
    {
        it->second -= dt;
        if (it->second <= 0.0f)
        {
            it = m_enemyHitTimers.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // 2. Base update (moves projectile based on velocity, decrements lifetime)
    Projectile::Update(dt);

    if (m_duration <= 0.0f && !m_isFadingOut)
    {
        m_isFadingOut = true;
    }

    if (m_isFadingOut)
    {
        m_fadeOutTimer -= dt;
        float fadeRatio = std::max(0.0f, m_fadeOutTimer / m_fadeDuration);
        
        // Diamond always starts fading from 255 alpha, regardless of trail tuning opacity
        float startAlpha = 255.0f;
        float alpha = startAlpha * fadeRatio;
        
        sf::Color c = m_sprite.getColor();
        c.a = static_cast<sf::Uint8>(alpha);
        m_sprite.setColor(c);
        
        // Synchronize trail opacity fade with the diamond head
        if (m_trailRenderer)
        {
            sf::Color trailColor = m_customColor;
            trailColor.a = static_cast<sf::Uint8>(m_baseAlpha * fadeRatio);
            m_trailRenderer->SetBaseColor(trailColor);
        }
    }

    // 3. Screen bounds bouncing logic
    if (m_projManager)
    {
        sf::FloatRect bounds = m_projManager->GetViewBounds();
        
        // Determine current view ratio (zoom level) based on design ViewWidth (1920.0f)
        float zoomRatio = bounds.width / 1920.0f;
        float horizontalBarOffset = 96.0f * zoomRatio;
        
        // Apply the 16:10 dimmed bar constraints scaled to world coordinates
        bounds.left += horizontalBarOffset;
        bounds.width -= (horizontalBarOffset * 2.0f);

        sf::Vector2f pos = m_sprite.getPosition();

        bool hitWall = false;
        if (pos.x < bounds.left)
        {
            pos.x = bounds.left;
            m_velocity.x = -m_velocity.x;
            hitWall = true;
        }
        else if (pos.x > bounds.left + bounds.width)
        {
            pos.x = bounds.left + bounds.width;
            m_velocity.x = -m_velocity.x;
            hitWall = true;
        }

        if (pos.y < bounds.top)
        {
            pos.y = bounds.top;
            m_velocity.y = -m_velocity.y;
            hitWall = true;
        }
        else if (pos.y > bounds.top + bounds.height)
        {
            pos.y = bounds.top + bounds.height;
            m_velocity.y = -m_velocity.y;
            hitWall = true;
        }

        if (hitWall)
        {
            m_sprite.setPosition(pos);
            
            // Re-calculate rotation to point in the new direction
            if (m_velocity.x != 0.0f || m_velocity.y != 0.0f)
            {
                float angle = std::atan2(m_velocity.y, m_velocity.x) * 180.0f / 3.14159265359f;
                m_sprite.setRotation(angle);
            }
        }
    }

    // 4. Update Trail
    if (m_trailRenderer)
    {
        m_trailRenderer->Update(dt, m_sprite.getPosition());
    }
}

void RunetracerProjectile::Draw(sf::RenderTarget& target) const
{
    if (m_trailRenderer)
    {
        target.draw(*m_trailRenderer);
    }
    target.draw(m_sprite);
}

bool RunetracerProjectile::HasHitEnemy(void* enemyId) const
{
    return m_enemyHitTimers.find(enemyId) != m_enemyHitTimers.end();
}

void RunetracerProjectile::OnHitEnemy(void* enemyId)
{
    // hitBoxDelay is 500ms
    m_enemyHitTimers[enemyId] = 0.5f;
}

bool RunetracerProjectile::IsExpired() const
{
    if(m_penetration == 0)
    {
        return true;
    }
    return m_isFadingOut && m_fadeOutTimer <= 0.0f;
}
