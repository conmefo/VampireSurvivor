#pragma once

#include "Projectile.h"
#include "../../Core/Animation/Tweener.h"

class WhipProjectile : public Projectile
{
public:
    WhipProjectile(const sf::Texture& texture, const sf::IntRect& textureRect, sf::Vector2f startPosition, float duration, float power, float areaMultiplier, const std::string& hitVfxName, bool flipX, bool flipY);
    ~WhipProjectile() override = default;

    void Update(float dt) override;
    void Draw(sf::RenderTarget& target) const override;

private:
    float m_maxDuration;
    float m_baseScaleX;
    float m_baseScaleY;

    Tweener m_scaleTween;
    Tweener m_alphaTween;
};
