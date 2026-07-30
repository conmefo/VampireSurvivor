#pragma once

class Player;

#include "Weapon.h"

class FireballWeapon : public Weapon
{
public:
    explicit FireballWeapon(const WeaponProfile& profile);
    ~FireballWeapon() override = default;

protected:
    sf::Vector2f GetTargetPosition(EnemyPool& enemyPool, Player& player) override;
    void FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex) override;

private:
    sf::Vector2f m_burstTargetPosition;
};
