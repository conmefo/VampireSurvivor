#pragma once

#include "Projectile.h"
#include "../../Components/TrailRenderer.h"
#include <unordered_map>
#include <memory>

class ProjectileManager;

class RunetracerProjectile : public Projectile
{
public:
    RunetracerProjectile(const sf::Texture& texture, const sf::IntRect& textureRect, 
                         sf::Vector2f startPosition, sf::Vector2f velocity, 
                         float duration, float power, float areaMultiplier,
                         const std::string& hitVfxName, int penetration,
                         ProjectileManager* projManager);
    ~RunetracerProjectile() override = default;

    void Update(float dt) override;
    void Draw(sf::RenderTarget& target) const override;
    bool HasHitEnemy(void* enemyId) const override;
    virtual void OnHitEnemy(void* enemyId) override;
    virtual bool IsExpired() const override;

    static vs::ParticleEmitterConfig* s_tuningConfig;

private:
    bool m_isFadingOut = false;
    float m_fadeOutTimer = 0.5f;
    float m_fadeDuration = 0.5f;
    ProjectileManager* m_projManager;
    std::unordered_map<void*, float> m_enemyHitTimers;
    std::unique_ptr<TrailRenderer> m_trailRenderer;
    float m_particleSpawnTimer;
    float m_colorHue;
};
