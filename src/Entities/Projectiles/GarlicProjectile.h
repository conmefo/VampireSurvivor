#pragma once

#include "Projectile.h"

class GarlicProjectile : public Projectile
{
public:
    GarlicProjectile(const sf::Texture& texture, const sf::IntRect& textureRect, sf::Vector2f startPosition, float duration, float power, float areaMultiplier, const std::string& hitVfxName, int penetration = -1);
    ~GarlicProjectile() override = default;

    void Update(float dt) override;
    void Draw(sf::RenderTarget& target) const override;
    void OnHitEnemy(void* enemyId) override;

    // Follows the player position exactly
    void SetCenterPosition(const sf::Vector2f& pos);
};
