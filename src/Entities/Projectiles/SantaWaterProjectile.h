#pragma once

#include "Projectile.h"
#include "ProjectileManager.h"

class SantaWaterProjectile : public Projectile
{
public:
    SantaWaterProjectile(ProjectileManager* projManager, TextureAtlas* atlas, const sf::Texture& texture, const sf::IntRect& textureRect, sf::Vector2f playerPosition, sf::Vector2f targetPosition, float duration, float power, float areaMultiplier, float hitBoxDelay, const std::string& hitVfxName);
    ~SantaWaterProjectile() override = default;

    void Update(float dt) override;

private:
    ProjectileManager* m_projManager;
    TextureAtlas* m_atlas;
    sf::Vector2f m_startPosition;
    sf::Vector2f m_targetPosition;
    float m_flightTimer;
    float m_zoneDuration;
    float m_areaMultiplier;
    float m_hitBoxDelay;
    bool m_hasLanded;

    static constexpr float FLIGHT_DURATION = 0.75f;
    static constexpr float SPIN_DURATION = 0.6f;
    static constexpr float DROP_HEIGHT_OFFSET = 500.0f;
};
