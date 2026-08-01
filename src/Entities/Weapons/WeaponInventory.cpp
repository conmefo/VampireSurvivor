#include "../Player.h"
#include "WeaponInventory.h"
#include <iostream>

void WeaponInventory::SetFactory(WeaponFactory* factory)
{
    m_factory = factory;
}

void WeaponInventory::AddWeapon(std::unique_ptr<Weapon> weapon)
{
    if(weapon)
    {
        weapon->SetObserver(this);
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

void WeaponInventory::LevelUpWeapon(const std::string& weaponId)
{
    Weapon* weapon = FindWeaponById(weaponId);
    if(weapon)
    {
        weapon->LevelUp();
    }
    else
    {
        std::cerr << "WeaponInventory::LevelUpWeapon: weapon '" << weaponId << "' not found\n";
    }
}

void WeaponInventory::LevelDownWeapon(const std::string& weaponId)
{
    Weapon* weapon = FindWeaponById(weaponId);
    if(weapon)
    {
        weapon->LevelDown();
    }
    else
    {
        std::cerr << "WeaponInventory::LevelDownWeapon: weapon '" << weaponId << "' not found\n";
    }
}

void WeaponInventory::OnWeaponEvolution(const std::string& evolvedWeaponId)
{
    if(!m_factory)
    {
        std::cerr << "WeaponInventory::OnWeaponEvolution: no factory set\n";
        return;
    }
    // Create and add the evolved weapon using the factory
    auto evolved = m_factory->Create(evolvedWeaponId);
    if(evolved)
    {
        AddWeapon(std::move(evolved));
    }
    else
    {
        std::cerr << "WeaponInventory::OnWeaponEvolution: failed to create '" << evolvedWeaponId << "'\n";
    }
}

Weapon* WeaponInventory::FindWeaponById(const std::string& weaponId)
{
    for(auto& weapon : m_weapons)
    {
        if(weapon->GetProfile().GetId() == weaponId)
        {
            return weapon.get();
        }
    }
    return nullptr;
}
