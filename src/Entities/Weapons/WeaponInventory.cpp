#include "WeaponInventory.h"
#include "WhipWeapon.h"
#include "MagicMissileWeapon.h"
#include "FireballWeapon.h"
#include "RunetracerWeapon.h"

WeaponInventory::WeaponInventory()
{
}

void WeaponInventory::AddWeapon(std::unique_ptr<Weapon> weapon)
{
    if(weapon)
    {
        m_weapons.push_back(std::move(weapon));
    }
}

void WeaponInventory::Update(float dt, ProjectileManager& projManager, TextureAtlas& atlas, sf::Vector2f playerPosition, sf::Vector2f playerDirection, EnemyPool& enemyPool)
{
    for(auto& weapon : m_weapons)
    {
        weapon->Update(dt, projManager, atlas, playerPosition, playerDirection, enemyPool);
    }
}
