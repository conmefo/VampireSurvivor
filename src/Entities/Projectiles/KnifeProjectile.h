#pragma once

#include "Projectile.h"

class KnifeProjectile : public Projectile
{
public:
    KnifeProjectile(const sf::Texture& texture, const sf::IntRect& rect, sf::Vector2f startPosition, sf::Vector2f velocity, float duration, float power, float areaMultiplier, const std::string& hitVfxName, int penetration = 1);
    ~KnifeProjectile() override = default;

    void Update(float dt) override;
};
