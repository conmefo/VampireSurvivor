#pragma once

#include "Projectile.h"
#include "../../Components/TrailRenderer.h"
#include <unordered_map>
#include <memory>
#include "../Particles/ParticleEmitter.h"

class ProjectileManager;

class RunetracerProjectile : public Projectile
{
public:
    RunetracerProjectile(const sf::Texture& texture, const sf::IntRect& textureRect, 
                         sf::Vector2f startPosition, sf::Vector2f velocity, 
                         float duration, float power, float areaMultiplier,
                         const std::string& hitVfxName, int penetration,
                         ProjectileManager* projManager, const vs::ParticleEmitterConfig* config = nullptr,
                         sf::Color customColor = sf::Color::White);
    ~RunetracerProjectile() override = default;

    void Update(float dt) override;
    void Draw(sf::RenderTarget& target) const override;
    bool HasHitEnemy(void* enemyId) const override;
    virtual void OnHitEnemy(void* enemyId) override;
    virtual bool IsExpired() const override;

private:
    bool m_isFadingOut = false;
    float m_fadeOutTimer = 0.25f;
    float m_fadeDuration = 0.25f;
    ProjectileManager* m_projManager;
    std::unordered_map<void*, float> m_enemyHitTimers;
    std::unique_ptr<TrailRenderer> m_trailRenderer;
    float m_particleSpawnTimer;
    float m_colorHue;
    sf::Color m_customColor;
    float m_baseAlpha = 187.0f;
};
