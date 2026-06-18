#pragma once

#include "Weapon.h"

class WhipWeapon : public Weapon
{
public:
    explicit WhipWeapon(const WeaponProfile& profile);
    ~WhipWeapon() override = default;

protected:
    void FireOne(ProjectileManager& projManager, TextureAtlas& atlas, sf::Vector2f playerPosition, sf::Vector2f playerDirection, sf::Vector2f targetPosition, int projectileIndex) override;

private:
    bool m_facingLeft;
};
