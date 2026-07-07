#pragma once

#include "Projectile.h"
#include <vector>
#include "../Particles/ParticleEmitter.h"

class MagicMissileProjectile : public Projectile
{
public:
    MagicMissileProjectile(const sf::Texture& texture, const sf::IntRect& rect1, const sf::IntRect& rect2, sf::Vector2f startPosition, sf::Vector2f velocity, float duration, float power, float areaMultiplier, const std::string& hitVfxName, int penetration = 1);
    ~MagicMissileProjectile() override;

    void Update(float dt) override;
    void Draw(sf::RenderTarget& target) const override;

private:
    static constexpr float FRAME_TIME = 0.1f;
    std::vector<sf::IntRect> m_frames;
    
    vs::ParticleEmitter* m_emitter = nullptr;
    int m_currentFrameIndex;
    float m_animationTimer;
    float m_baseScaleX;
    float m_baseScaleY;
};
