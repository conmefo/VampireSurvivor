#pragma once

#include "Weapon.h"

class LightningRingWeapon : public Weapon
{
public:
    explicit LightningRingWeapon(const WeaponProfile& profile);
    ~LightningRingWeapon() override = default;

    void Update(float dt, ProjectileManager& projManager, TextureAtlas& atlas, Player& player, EnemyPool& enemyPool) override;

protected:
    sf::Vector2f GetTargetPosition(EnemyPool& enemyPool, Player& player) override;
    void FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex) override;

private:
    EnemyPool* m_enemyPool = nullptr;
};
