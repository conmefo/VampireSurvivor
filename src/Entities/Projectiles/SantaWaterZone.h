#pragma once

#include "Projectile.h"

class TextureAtlas;

class SantaWaterZone : public Projectile
{
public:
    SantaWaterZone(TextureAtlas* atlas, const sf::Texture& texture, const sf::IntRect& textureRect, sf::Vector2f targetPosition, float duration, float power, float areaMultiplier, float hitBoxDelay, const std::string& hitVfxName);
    ~SantaWaterZone() override = default;

    void Update(float dt) override;
    void Draw(sf::RenderTarget& target) const override;
    bool IsExpired() const override;

    void OnHitEnemy(void* enemyId) override;

private:
    float m_areaMultiplier;
    float m_hitBoxDelay;
    float m_tickTimer;
    sf::CircleShape m_blueCircleOverlay;
    TextureAtlas* m_atlas = nullptr;
    bool m_zoneExpired = false;

    std::unique_ptr<vs::ParticleEmitter> m_flameEmitter;

    static constexpr float BASE_RADIUS = 32.0f;
    static constexpr float POOL_SCALE_MULTIPLIER = 2.0f;
};
