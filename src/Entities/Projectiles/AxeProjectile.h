#pragma once

#include "Projectile.h"

#include "../../Components/ProjectileShadowTrailComponent.h"

class AxeProjectile : public Projectile
{
public:
    AxeProjectile(const sf::Texture& texture, const sf::IntRect& textureRect, sf::Vector2f startPosition, sf::Vector2f velocity, float duration, float power, float areaMultiplier, const std::string& hitVfxName, int penetration = -1);
    
    void Update(float dt) override;
    void Draw(sf::RenderTarget& target) const override;

private:
    ProjectileShadowTrailComponent m_shadowTrail;
    float m_rotationSpeed; // Degrees per second
};

extern float g_AxeGravity;
extern float g_AxeShadowDelay;
extern float g_AxeScale;
