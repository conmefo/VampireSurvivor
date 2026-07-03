#include "RunetracerProjectile.h"
#include "ProjectileManager.h"
#include <cmath>

RunetracerProjectile::RunetracerProjectile(ProjectileManager* projManager, const sf::Texture& texture, const sf::IntRect& textureRect, sf::Vector2f startPosition, sf::Vector2f velocity, float duration, float power, float areaMultiplier, const std::string& hitVfxName, int penetration)
    : Projectile(texture, textureRect, startPosition, velocity, duration, power, areaMultiplier, hitVfxName, penetration)
    , m_projManager(projManager)
    , m_particleSpawnTimer(0.0f)
    , m_colorHue(0.0f)
{
    m_sprite.setOrigin(textureRect.width / 2.0f, textureRect.height / 2.0f);
}

void RunetracerProjectile::Update(float dt)
{
    // 1. Move projectile
    m_sprite.move(m_velocity * dt);
    m_duration -= dt;

    // 2. Cooldown for hit enemies
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

    // 3. Bounce off screen boundaries
    if (m_projManager)
    {
        sf::FloatRect bounds = m_projManager->GetViewBounds();
        sf::FloatRect myBounds = GetGlobalBounds();

        if (myBounds.left < bounds.left && m_velocity.x < 0)
        {
            m_velocity.x = -m_velocity.x;
        }
        else if (myBounds.left + myBounds.width > bounds.left + bounds.width && m_velocity.x > 0)
        {
            m_velocity.x = -m_velocity.x;
        }

        if (myBounds.top < bounds.top && m_velocity.y < 0)
        {
            m_velocity.y = -m_velocity.y;
        }
        else if (myBounds.top + myBounds.height > bounds.top + bounds.height && m_velocity.y > 0)
        {
            m_velocity.y = -m_velocity.y;
        }
    }

    // 5. Particle Trail
    m_particleSpawnTimer -= dt;
    if (m_particleSpawnTimer <= 0.0f && m_particleManager)
    {
        m_particleSpawnTimer = 1.0f / 30.0f; 
        
        // Emulating Unity's TrailRenderer is complex without a ribbon renderer, 
        // but we can spawn a short-lived static particle here if needed.
    }
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
