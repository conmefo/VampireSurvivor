#pragma once

#include "Weapon.h"

class RunetracerWeapon : public Weapon
{
public:
    explicit RunetracerWeapon(const WeaponProfile& profile);
    ~RunetracerWeapon() override = default;

    void Update(float dt, ProjectileManager& projManager, TextureAtlas& atlas, sf::Vector2f playerPosition, sf::Vector2f playerDirection, EnemyPool& enemyPool) override;

protected:
    void FireOne(ProjectileManager& projManager, TextureAtlas& atlas, sf::Vector2f playerPosition, sf::Vector2f playerDirection, sf::Vector2f targetPosition, int projectileIndex) override;
};
