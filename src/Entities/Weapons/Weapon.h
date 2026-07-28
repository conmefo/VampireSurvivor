#pragma once

class Player;

#include "../../Core/Data/WeaponProfile.h"
#include "../Projectiles/ProjectileManager.h"
#include <SFML/System/Vector2.hpp>

class EnemyPool;

class Weapon
{
public:
    explicit Weapon(const WeaponProfile& profile);
    virtual ~Weapon() = default;

    virtual void Update(float dt, ProjectileManager& projManager, TextureAtlas& atlas, Player& player, EnemyPool& enemyPool);
    virtual void Draw(sf::RenderTarget& target) const {}

    const WeaponProfile& GetProfile() const;

protected:
    virtual sf::Vector2f GetTargetPosition(EnemyPool& enemyPool, Player& player);
    virtual void FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex) = 0;

    WeaponProfile m_profile;
    float m_cooldownTimer;
};
