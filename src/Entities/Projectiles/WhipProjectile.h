#pragma once

#include "Projectile.h"
#include "../../Core/Animation/Tweener.h"

class Player;

class WhipProjectile : public Projectile
{
public:
    WhipProjectile(const sf::Texture& texture, const sf::IntRect& textureRect, Player* player, sf::Vector2f relativeOffset, float duration, float power, float areaMultiplier, const std::string& hitVfxName, bool flipX, bool flipY);
    ~WhipProjectile() override = default;

    void Update(float dt) override;
    void Draw(sf::RenderTarget& target) const override;

private:
    Player* m_player;
    sf::Vector2f m_relativeOffset;
    sf::Vector2f m_startOffset;
    float m_maxDuration;
    float m_baseScaleX;
    float m_baseScaleY;
    float m_elapsedTime = 0.0f;
};
