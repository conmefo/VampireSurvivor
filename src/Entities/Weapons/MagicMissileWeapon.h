#pragma once

#include "Weapon.h"

class MagicMissileWeapon : public Weapon
{
public:
    explicit MagicMissileWeapon(const WeaponProfile& profile);
    ~MagicMissileWeapon() override = default;

protected:
    void FireOne(ProjectileManager& projManager, TextureAtlas& atlas, sf::Vector2f playerPosition, sf::Vector2f playerDirection, sf::Vector2f targetPosition, int projectileIndex) override;

private:
    sf::Vector2f m_lastFacingDir;
};
