#pragma once

#include "Projectile.h"

class AxeProjectile : public Projectile
{
public:
    AxeProjectile(const sf::Texture& texture, const sf::IntRect& textureRect, sf::Vector2f startPosition, sf::Vector2f velocity, float duration, float power, float areaMultiplier, const std::string& hitVfxName, int penetration = -1);
    
    void Update(float dt) override;

private:
    float m_rotationSpeed; // Degrees per second
};

extern float g_AxeGravity;
