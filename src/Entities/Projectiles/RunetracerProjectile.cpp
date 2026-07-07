#include "RunetracerProjectile.h"
#include "ProjectileManager.h"
#include <cmath>

RunetracerProjectile::RunetracerProjectile(const sf::Texture& texture, const sf::IntRect& textureRect, sf::Vector2f startPosition, sf::Vector2f velocity, float duration, float power, float areaMultiplier, const std::string& hitVfxName, int penetration, ProjectileManager* projManager, const vs::ParticleEmitterConfig* config)
    : Projectile(texture, textureRect, startPosition, velocity, duration, power, areaMultiplier, hitVfxName, penetration)
    , m_projManager(projManager)
    , m_particleSpawnTimer(0.0f)
    , m_colorHue(0.0f)
{
    m_penetration = penetration;
    m_sprite.setOrigin(textureRect.width / 2.0f, textureRect.height / 2.0f);

    m_trailRenderer = std::make_unique<TrailRenderer>(0.8f, 15.0f, 2.0f); // defaults
    if (m_trailRenderer)
    {
        if (config)
        {
            m_sprite.setScale(config->weaponScaleX, config->weaponScaleY);
            m_trailRenderer->SetWidth(config->trailWidth);
            m_trailRenderer->SetFadeStartRatio(config->trailFadeStart);
            m_trailRenderer->SetMaxLifetime(config->trailLength);
            m_trailRenderer->SetBaseColor(sf::Color(
                static_cast<sf::Uint8>(config->colorR),
                static_cast<sf::Uint8>(config->colorG),
                static_cast<sf::Uint8>(config->colorB),
                static_cast<sf::Uint8>(config->colorA)
            ));
        }
        else
        {
            // Fallback defaults if config is missing
            m_sprite.setScale(1.8f, 1.8f);
            m_trailRenderer->SetWidth(3.1f);
            m_trailRenderer->SetFadeStartRatio(0.5f);
            m_trailRenderer->SetMaxLifetime(2.2f);
            m_trailRenderer->SetBaseColor(sf::Color(211, 215, 209, 157));
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
        
        // Let the trail renderer naturally fade out as it stops updating or its points die out
    }

    // 3. Screen bounds bouncing logic
    if (m_projManager)
    {
        sf::FloatRect bounds = m_projManager->GetViewBounds();
        
        // Apply the 16:10 dimmed bar constraints (96px on each side)
        bounds.left += 96.0f;
        bounds.width -= 192.0f;

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
