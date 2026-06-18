#include "MagicMissileProjectile.h"
#include <cmath>
#include <algorithm>
#include <cstdlib> // For rand()

MagicMissileProjectile::MagicMissileProjectile(const sf::Texture& texture, const sf::IntRect& rect1, const sf::IntRect& rect2, sf::Vector2f startPosition, sf::Vector2f velocity, float duration, float power, float areaMultiplier, const std::string& hitVfxName, int penetration)
    : Projectile(texture, rect1, startPosition, velocity, duration, power, areaMultiplier, hitVfxName, penetration)
    , m_currentFrameIndex(0)
    , m_animationTimer(0.0f)
    , m_particleSpawnTimer(0.0f)
{
    m_frames.push_back(rect1);
    m_frames.push_back(rect2);

    // Center origin
    sf::FloatRect bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);

    // Force the weapon sprite to be 27x27 pixels, respecting area multiplier
    m_sprite.setScale((45.0f / bounds.width) * areaMultiplier, (29.0f / bounds.height) * areaMultiplier);

    // Compute angle of velocity for rotation
    float angle = std::atan2(velocity.y, velocity.x) * 180.0f / 3.14159265f;
    m_sprite.setRotation(angle);
}

void MagicMissileProjectile::Update(float dt)
{
    Projectile::Update(dt);

    m_animationTimer += dt;
    if(m_animationTimer >= FRAME_TIME)
    {
        m_animationTimer -= FRAME_TIME;
        m_currentFrameIndex = (m_currentFrameIndex + 1) % 2;
        m_sprite.setTextureRect(m_frames[m_currentFrameIndex]);
    }

    // Update particles
    for (auto& p : m_particles) {
        p.life -= dt;
        p.position += p.velocity * dt;
    }
    m_particles.erase(std::remove_if(m_particles.begin(), m_particles.end(), [](const Particle& p) { return p.life <= 0.0f; }), m_particles.end());

    // Spawn new particles based on time, using a randomized interval to avoid uniform distance
    m_particleSpawnTimer -= dt;
    sf::Vector2f currentPos = m_sprite.getPosition();
    
    while (m_particleSpawnTimer <= 0.0f) {
        // Make the interval stable enough so they never spawn too close and touch
        float randomInterval = PARTICLE_SPAWN_INTERVAL * (0.8f + (static_cast<float>(rand() % 40) / 100.0f));
        m_particleSpawnTimer += randomInterval;
        
        // Spawn a small cluster of 1 to 3 particles per interval
        int clusterSize = 1 + (rand() % 3);
        for (int i = 0; i < clusterSize; ++i) {
            // Tighter initial scatter (-1.0f to 1.0f)
            float offsetX = (static_cast<float>(rand() % 20) / 10.0f) - 1.0f;
            float offsetY = (static_cast<float>(rand() % 20) / 10.0f) - 1.0f;
            sf::Vector2f spawnPos = currentPos + sf::Vector2f(offsetX, offsetY);
            
            // Randomly drift leftward/rightward with a much higher velocity so they travel further
            float velX = (static_cast<float>(rand() % 500) / 10.0f) - 25.0f; // -25.0 to 25.0 px/s
            float velY = (static_cast<float>(rand() % 500) / 10.0f) - 25.0f; // -25.0 to 25.0 px/s
            sf::Vector2f particleVel(velX, velY);
            
            // Slightly random lifetime so they don't all vanish exactly together
            float randomLife = PARTICLE_LIFETIME * (0.8f + (static_cast<float>(rand() % 40) / 100.0f));
            
            m_particles.push_back({spawnPos, particleVel, randomLife, randomLife});
        }
    }
}

void MagicMissileProjectile::Draw(sf::RenderTarget& target) const
{
    // Draw particles first so they appear behind the main projectile
    // Reduced base size to 2x2 to make them very small
    sf::RectangleShape particleShape(sf::Vector2f(2.0f, 2.0f));
    particleShape.setOrigin(1.0f, 1.0f);
    
    // Create render states for additive blending
    sf::RenderStates states;
    states.blendMode = sf::BlendAdd;

    for (const auto& p : m_particles) {
        particleShape.setPosition(p.position);
        
        // ratio goes from 1.0 (new) to 0.0 (dead)
        float ratio = std::max(0.0f, p.life / p.maxLife);
        
        // Shrink slightly as they fade
        float scale = 0.5f + (0.5f * ratio);
        particleShape.setScale(scale, scale);
        
        // Pure Cyan, alpha driven by lifetime
        sf::Color color = sf::Color::Cyan;
        
        // Mix a little bit of white when newly spawned
        color.r = static_cast<sf::Uint8>(100 * ratio);
        
        // Fade out opacity
        color.a = static_cast<sf::Uint8>(255 * ratio);
        particleShape.setFillColor(color);
        
        target.draw(particleShape, states);
    }

    // Draw main projectile
    Projectile::Draw(target);
}
