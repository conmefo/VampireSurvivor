#pragma once

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Projectile.h"
#include "../../Core/Resources/TextureAtlas.h"

struct CollisionTarget
{
    sf::FloatRect bounds;
    void* entityId;
};

class ProjectileManager
{
public:
    ProjectileManager();
    ~ProjectileManager() = default;

    void Update(float dt);
    void Draw(sf::RenderTarget& target) const;

    void SpawnProjectile(TextureAtlas& atlas, const std::string& frameName, sf::Vector2f position, sf::Vector2f velocity, float duration, float power, float areaMultiplier, const std::string& hitVfxName);
    void AddProjectile(std::unique_ptr<Projectile> projectile);

    std::vector<std::pair<Projectile*, void*>> CheckCollisions(const std::vector<CollisionTarget>& targets);

private:
    std::vector<std::unique_ptr<Projectile>> m_projectiles;
};
