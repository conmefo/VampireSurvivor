#pragma once

#include <vector>
#include <memory>
#include <string>
#include "Weapon.h"
#include "WeaponFactory.h"
#include "IWeaponLevelObserver.h"
#include "../Projectiles/ProjectileManager.h"
#include "../../Core/Resources/TextureAtlas.h"
#include <SFML/System/Vector2.hpp>

class EnemyPool;

class WeaponInventory : public IWeaponLevelObserver
{
public:
    WeaponInventory() = default;
    ~WeaponInventory() = default;

    // Called by GameState after creating the WeaponFactory, before adding weapons.
    void SetFactory(WeaponFactory* factory);

    void AddWeapon(std::unique_ptr<Weapon> weapon);
    void Update(float dt, ProjectileManager& projManager, TextureAtlas& atlas, Player& player, EnemyPool& enemyPool);
    void Draw(sf::RenderTarget& target) const;

    // Finds weapon by id and calls LevelUp() on it. Used by debug key and future UI.
    void LevelUpWeapon(const std::string& weaponId);
    void LevelDownWeapon(const std::string& weaponId);

    // IWeaponLevelObserver — called when a weapon's level-up triggers evolution
    void OnWeaponEvolution(const std::string& evolvedWeaponId) override;

private:
    Weapon* FindWeaponById(const std::string& weaponId);

    std::vector<std::unique_ptr<Weapon>> m_weapons;
    WeaponFactory* m_factory = nullptr;
};
