#include "ParticleEmitter.h"
#include <cmath>
#include <random>

namespace vs
{

namespace
{
    float RandomFloat(float min, float max)
    {
        if (min > max) std::swap(min, max);
        if (min == max) return min; // Avoid distribution issues if equal
        
        static std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> dist(min, max);
        return dist(rng);
    }
}

ParticleEmitter::ParticleEmitter(const ParticleEmitterConfig& config, const sf::Vector2f& position, const sf::IntRect& textureRect)
    : m_config(config), m_position(position), m_previousPosition(position), m_textureRect(textureRect)
{
}

void ParticleEmitter::Update(float dt)
{
    m_elapsedTime += dt;

    if (!m_hasBursted && m_config.burstCount > 0)
    {
        for (int i = 0; i < m_config.burstCount; ++i)
        {
            EmitParticle(m_position);
        }
        m_hasBursted = true;
    }

    // Handle Emission
    if (m_config.looping || m_elapsedTime < m_config.duration)
    {
        if (m_config.emissionRate > 0.0f)
        {
            m_emissionTimer += dt;
            float emitInterval = 1.0f / m_config.emissionRate;
            
            while (m_emissionTimer >= emitInterval)
            {
                m_emissionTimer -= emitInterval;
                
                float t = 1.0f;
                if (dt > 0.0f) {
                    t = 1.0f - (m_emissionTimer / dt);
                }
                sf::Vector2f spawnPos = m_previousPosition + (m_position - m_previousPosition) * t;
                EmitParticle(spawnPos);
            }
        }
    }

    // Sync live tuning RGB colors to startColor
    m_config.startColor.r = static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, m_config.colorR)));
    m_config.startColor.g = static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, m_config.colorG)));
    m_config.startColor.b = static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, m_config.colorB)));

    // Update Particles
    for (auto it = m_particles.begin(); it != m_particles.end(); )
    {
        it->lifetime += dt;
        
        if (!it->IsAlive())
        {
            it = m_particles.erase(it);
        }
        else
        {
            // Apply physics
            it->velocity.y += m_config.gravityModifier * dt * 500.0f;
            if (m_config.damping > 0.0f)
            {
                it->velocity -= it->velocity * m_config.damping * dt;
            }

            it->position += it->velocity * dt;
            
            float t = it->GetNormalizedLifetime();
            
            // Interpolate color
            sf::Uint8 r = static_cast<sf::Uint8>(m_config.startColor.r + t * (m_config.endColor.r - m_config.startColor.r));
            sf::Uint8 g = static_cast<sf::Uint8>(m_config.startColor.g + t * (m_config.endColor.g - m_config.startColor.g));
            sf::Uint8 b = static_cast<sf::Uint8>(m_config.startColor.b + t * (m_config.endColor.b - m_config.startColor.b));
            sf::Uint8 a = static_cast<sf::Uint8>(m_config.startColor.a + t * (m_config.endColor.a - m_config.startColor.a));
            it->color = sf::Color(r, g, b, a);
            
            // Interpolate size
            float startSize = it->initialSize; // Initial randomized size
            // If endSize is meant to scale proportionally, we can just interpolate.
            // For simplicity, we just fade from initial random size to config.endSize
            it->size = startSize + t * (m_config.endSize - startSize);
            
            it->rotation += it->rotationSpeed * dt;
            
            ++it;
        }
    }
}

bool ParticleEmitter::IsFinished() const
{
    return (!m_config.looping && m_elapsedTime >= m_config.duration && m_particles.empty());
}

void ParticleEmitter::EmitParticle(const sf::Vector2f& spawnPos)
{
    Particle p;
    p.lifetime = 0.0f;
    p.maxLifetime = m_config.startLifetime;
    p.textureRect = m_textureRect;
    p.color = sf::Color(
        static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, m_config.colorR))),
        static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, m_config.colorG))),
        static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, m_config.colorB))),
        static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, m_config.colorA)))
    );
    
    // Initial size is a random value between minSize and maxSize
    p.initialSize = RandomFloat(m_config.minSize, m_config.maxSize);
    p.size = p.initialSize;
    
    if (m_config.randomRotation)
    {
        p.rotation = RandomFloat(0.0f, 360.0f);
        p.rotationSpeed = RandomFloat(-180.0f, 180.0f);
    }
    else
    {
        p.rotation = 0.0f;
        p.rotationSpeed = 0.0f;
    }

    // Calculate position offset based on shape radius and spread angle
    float radius = RandomFloat(0.0f, m_config.shapeRadius);
    
    // The spawn offset should be a full circle spread, decoupled from the velocity spread angle.
    // This allows shapeRadius to act as the "head" size and shapeAngle to act as the "tail" spread.
    float offsetAngle = RandomFloat(0.0f, 360.0f) * (3.14159265f / 180.0f);
    
    sf::Vector2f offset(std::cos(offsetAngle) * radius, std::sin(offsetAngle) * radius);
    p.position = spawnPos + offset;

    // Direction based on emissionDirection or offset/random
    sf::Vector2f dir;
    if (m_config.emissionDirection != sf::Vector2f(0.0f, 0.0f))
    {
        // Calculate random spread angle based on emissionDirection
        float baseAngle = std::atan2(m_config.emissionDirection.y, m_config.emissionDirection.x);
        float spread = RandomFloat(-m_config.shapeAngle / 2.0f, m_config.shapeAngle / 2.0f) * (3.14159265f / 180.0f);
        float finalAngle = baseAngle + spread;
        dir = sf::Vector2f(std::cos(finalAngle), std::sin(finalAngle));
    }
    else if (radius > 0.001f)
    {
        dir = offset / radius; // Normalize
    }
    else
    {
        float dirAngle = RandomFloat(0.0f, 360.0f) * (3.14159265f / 180.0f);
        dir = sf::Vector2f(std::cos(dirAngle), std::sin(dirAngle));
    }

    p.velocity = dir * m_config.startSpeed;
    
    m_particles.push_back(p);
}

} // namespace vs
