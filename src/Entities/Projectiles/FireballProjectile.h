#pragma once

#include "Projectile.h"
#include "ProjectileManager.h"

class FireballProjectile : public Projectile
{
public:
    FireballProjectile(ProjectileManager* projManager, TextureAtlas* atlas, const sf::Texture& texture, const sf::IntRect& textureRect, sf::Vector2f startPosition, sf::Vector2f velocity, float duration, float power, float areaMultiplier, const std::string& hitVfxName, int penetration = -1);
    ~FireballProjectile() override;
    void Update(float dt) override;

private:
    ProjectileManager* m_projManager;
    TextureAtlas* m_atlas;
    vs::ParticleEmitter* m_trailEmitter = nullptr;
    float m_areaMultiplier;
    bool m_hasExploded = false;

    void Explode();
};
