#pragma once

#include "Weapon.h"

class FireballWeapon : public Weapon
{
public:
    explicit FireballWeapon(const WeaponProfile& profile);
    ~FireballWeapon() override = default;

protected:
    sf::Vector2f GetTargetPosition(EnemyPool& enemyPool, sf::Vector2f playerPosition, sf::Vector2f playerDirection) override;
    void FireOne(ProjectileManager& projManager, TextureAtlas& atlas, sf::Vector2f playerPosition, sf::Vector2f playerDirection, sf::Vector2f targetPosition, int projectileIndex) override;
};
