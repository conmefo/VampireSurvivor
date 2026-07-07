#pragma once

#include "Weapon.h"

class AxeWeapon : public Weapon
{
public:
    explicit AxeWeapon(const WeaponProfile& profile);

protected:
    void FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex) override;
};
