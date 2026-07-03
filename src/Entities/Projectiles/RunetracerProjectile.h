#pragma once

#include "Projectile.h"
#include <unordered_map>

class ProjectileManager;

class RunetracerProjectile : public Projectile
{
public:
    RunetracerProjectile(ProjectileManager* projManager, const sf::Texture& texture, const sf::IntRect& textureRect, sf::Vector2f startPosition, sf::Vector2f velocity, float duration, float power, float areaMultiplier, const std::string& hitVfxName, int penetration = -1);
    ~RunetracerProjectile() override = default;

    void Update(float dt) override;
    bool HasHitEnemy(void* enemyId) const override;
    void OnHitEnemy(void* enemyId) override;

private:
    ProjectileManager* m_projManager;
    std::unordered_map<void*, float> m_enemyHitTimers;
    float m_particleSpawnTimer;
    float m_colorHue;
};
