#include "RunetracerProjectile.h"
#include "ProjectileManager.h"
#include <cmath>

vs::ParticleEmitterConfig* RunetracerProjectile::s_tuningConfig = nullptr;

RunetracerProjectile::RunetracerProjectile(const sf::Texture& texture, const sf::IntRect& textureRect, sf::Vector2f startPosition, sf::Vector2f velocity, float duration, float power, float areaMultiplier, const std::string& hitVfxName, int penetration, ProjectileManager* projManager)
    : Projectile(texture, textureRect, startPosition, velocity, duration, power, areaMultiplier, hitVfxName, penetration)
    , m_projManager(projManager)
    , m_particleSpawnTimer(0.0f)
    , m_colorHue(0.0f)
{
    m_sprite.setOrigin(textureRect.width / 2.0f, textureRect.height / 2.0f);

    m_trailRenderer = std::make_unique<TrailRenderer>(0.8f, 15.0f, 2.0f); // defaults
    m_trailRenderer->SetBaseColor(sf::Color(0, 255, 255, 255));
}

void RunetracerProjectile::Update(float dt)
{
    // Apply live tuning if available
    if (s_tuningConfig)
    {
        m_sprite.setScale(s_tuningConfig->weaponScaleX, s_tuningConfig->weaponScaleY);
        if (m_trailRenderer)
        {
            m_trailRenderer->SetWidth(s_tuningConfig->trailWidth);
            m_trailRenderer->SetFadeStartRatio(s_tuningConfig->trailFadeStart);
            m_trailRenderer->SetMaxLifetime(s_tuningConfig->trailLength);
            m_trailRenderer->SetBaseColor(sf::Color(
                static_cast<sf::Uint8>(s_tuningConfig->colorR),
                static_cast<sf::Uint8>(s_tuningConfig->colorG),
                static_cast<sf::Uint8>(s_tuningConfig->colorB),
                255
            ));
        }
    }

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
        float alpha = 255.0f * std::max(0.0f, m_fadeOutTimer / m_fadeDuration);
        
        sf::Color c = m_sprite.getColor();
        c.a = static_cast<sf::Uint8>(alpha);
        m_sprite.setColor(c);
        
        // Let the trail renderer naturally fade out as it stops updating or its points die out
    }

    // 3. Screen bounds bouncing logic
    if (m_projManager)
    {
        sf::FloatRect bounds = m_projManager->GetViewBounds();
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
