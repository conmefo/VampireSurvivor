#pragma once

#include "Projectile.h"
#include <vector>

class MagicMissileProjectile : public Projectile
{
public:
    MagicMissileProjectile(const sf::Texture& texture, const sf::IntRect& rect1, const sf::IntRect& rect2, sf::Vector2f startPosition, sf::Vector2f velocity, float duration, float power, float areaMultiplier, const std::string& hitVfxName, int penetration = 1);
    ~MagicMissileProjectile() override = default;

    void Update(float dt) override;
    void Draw(sf::RenderTarget& target) const override;

private:
    struct Particle {
        sf::Vector2f position;
        sf::Vector2f velocity;
        float life;
        float maxLife;
    };

    static constexpr float FRAME_TIME = 0.1f;
    static constexpr float PARTICLE_LIFETIME = 0.43f; // Much longer tail
    static constexpr float PARTICLE_SPAWN_INTERVAL = 0.03f; // Decreased so particles spawn closer together

    std::vector<sf::IntRect> m_frames;
    int m_currentFrameIndex;
    float m_animationTimer;
    
    std::vector<Particle> m_particles;
    float m_particleSpawnTimer;
};
