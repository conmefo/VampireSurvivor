#pragma once

#include <vector>
#include <memory>
#include "Weapon.h"
#include "../Projectiles/ProjectileManager.h"
#include "../../Core/Resources/TextureAtlas.h"
#include <SFML/System/Vector2.hpp>

class EnemyPool;

class WeaponInventory
{
public:
    WeaponInventory();
    ~WeaponInventory() = default;

    void AddWeapon(std::unique_ptr<Weapon> weapon);
    void Update(float dt, ProjectileManager& projManager, TextureAtlas& atlas, Player& player, EnemyPool& enemyPool);
    void Draw(sf::RenderTarget& target) const;

private:
    std::vector<std::unique_ptr<Weapon>> m_weapons;
};
