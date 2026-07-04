#pragma once

#include "Weapon.h"

class Player;

class KnifeWeapon : public Weapon
{
public:
    explicit KnifeWeapon(const WeaponProfile& profile);
    ~KnifeWeapon() override = default;

protected:
    sf::Vector2f GetTargetPosition(EnemyPool& enemyPool, Player& player) override;
    void FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex) override;

private:
    sf::Vector2f m_lastFacingDir;
};
