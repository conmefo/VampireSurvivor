#pragma once

#include "Weapon.h"

class SongOfManaWeapon : public Weapon
{
public:
    explicit SongOfManaWeapon(const WeaponProfile& profile);
    ~SongOfManaWeapon() override = default;

    void Update(float dt, ProjectileManager& projManager, TextureAtlas& atlas, Player& player, EnemyPool& enemyPool) override;

protected:
    void FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex) override;
};
