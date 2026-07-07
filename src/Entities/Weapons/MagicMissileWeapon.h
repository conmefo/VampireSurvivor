#pragma once

class Player;

#include "Weapon.h"

class MagicMissileWeapon : public Weapon
{
public:
    explicit MagicMissileWeapon(const WeaponProfile& profile);
    ~MagicMissileWeapon() override = default;

protected:
    void FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex) override;

private:
    sf::Vector2f m_lastFacingDir;
};
