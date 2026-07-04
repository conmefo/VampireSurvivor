#pragma once

class Player;

#include "Weapon.h"

class RunetracerWeapon : public Weapon
{
public:
    explicit RunetracerWeapon(const WeaponProfile& profile);
    ~RunetracerWeapon() override = default;

    void Update(float dt, ProjectileManager& projManager, TextureAtlas& atlas, Player& player, EnemyPool& enemyPool) override;

protected:
    void FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex) override;
};
