#include "../Player.h"
#include "WeaponInventory.h"
#include "WhipWeapon.h"
#include "MagicMissileWeapon.h"
#include "FireballWeapon.h"
#include "RunetracerWeapon.h"
#include "KnifeWeapon.h"

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

void WeaponInventory::Update(float dt, ProjectileManager& projManager, TextureAtlas& atlas, Player& player, EnemyPool& enemyPool)
{
    for(auto& weapon : m_weapons)
    {
        weapon->Update(dt, projManager, atlas, player, enemyPool);
    }
}

void WeaponInventory::Draw(sf::RenderTarget& target) const
{
    for(const auto& weapon : m_weapons)
    {
        weapon->Draw(target);
    }
}
