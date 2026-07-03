#pragma once

#include "../../Core/Data/WeaponProfile.h"
#include "../Projectiles/ProjectileManager.h"
#include <SFML/System/Vector2.hpp>

class EnemyPool;

class Weapon
{
public:
    explicit Weapon(const WeaponProfile& profile);
    virtual ~Weapon() = default;

    virtual void Update(float dt, ProjectileManager& projManager, TextureAtlas& atlas, sf::Vector2f playerPosition, sf::Vector2f playerDirection, EnemyPool& enemyPool);

    const WeaponProfile& GetProfile() const;

protected:
    virtual sf::Vector2f GetTargetPosition(EnemyPool& enemyPool, sf::Vector2f playerPosition, sf::Vector2f playerDirection);
    virtual void FireOne(ProjectileManager& projManager, TextureAtlas& atlas, sf::Vector2f playerPosition, sf::Vector2f playerDirection, sf::Vector2f targetPosition, int projectileIndex) = 0;

    WeaponProfile m_profile;
    float m_cooldownTimer;
};
