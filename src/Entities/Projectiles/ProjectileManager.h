#pragma once

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Projectile.h"
#include "../../Core/Resources/TextureAtlas.h"
#include "../Particles/ParticleManager.h"

#include <functional>

struct CollisionTarget
{
    sf::FloatRect bounds;
    void* entityId;
};

struct DelayedAction
{
    float timer;
    std::function<void()> action;
};

class ProjectileManager
{
public:
    ProjectileManager();
    ~ProjectileManager() = default;

    void Initialize(vs::ParticleManager* pm) { m_particleManager = pm; }
    vs::ParticleManager* GetParticleManager() const { return m_particleManager; }

    void SetViewBounds(const sf::FloatRect& bounds) { m_viewBounds = bounds; }
    const sf::FloatRect& GetViewBounds() const { return m_viewBounds; }

    void Update(float dt);
    void Draw(sf::RenderTarget& target) const;

    void SpawnProjectile(TextureAtlas& atlas, const std::string& frameName, sf::Vector2f position, sf::Vector2f velocity, float duration, float power, float areaMultiplier, const std::string& hitVfxName);
    void AddProjectile(std::unique_ptr<Projectile> projectile);

    void QueueDelayedAction(float delay, std::function<void()> action);

    std::vector<std::pair<Projectile*, void*>> CheckCollisions(const std::vector<CollisionTarget>& targets);

private:
    std::vector<std::unique_ptr<Projectile>> m_projectiles;
    std::vector<DelayedAction> m_delayedActions;
    vs::ParticleManager* m_particleManager = nullptr;
    sf::FloatRect m_viewBounds;
};
