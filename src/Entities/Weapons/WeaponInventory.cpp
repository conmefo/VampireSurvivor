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
        Weapon* existing = FindWeaponById(weapon->GetProfile().GetId());
        if(existing)
        {
            existing->LevelUp();
            return;
        }

        if(IsFull())
        {
            std::cerr << "WeaponInventory::AddWeapon: inventory full (" << m_maxSlots << " slots), cannot add '" << weapon->GetProfile().GetId() << "'\n";
            return;
        }

        weapon->SetObserver(this);
        weapon->SetAudioService(m_audioService);
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

const Weapon* WeaponInventory::GetWeapon(const std::string& weaponId) const
{
    for(const auto& weapon : m_weapons)
    {
        if(weapon->GetProfile().GetId() == weaponId)
            return weapon.get();
    }
    return nullptr;
}

bool WeaponInventory::HasWeapon(const std::string& weaponId) const
{
    return GetWeapon(weaponId) != nullptr;
}

void WeaponInventory::LevelUpWeapon(const std::string& weaponId)
{
    bool found = false;
    for(auto& weapon : m_weapons)
    {
        if(weapon->GetProfile().GetId() == weaponId)
        {
            weapon->LevelUp();
            found = true;
        }
    }
    if(!found)
    {
        std::cerr << "WeaponInventory::LevelUpWeapon: weapon '" << weaponId << "' not found\n";
    }
}

void WeaponInventory::LevelDownWeapon(const std::string& weaponId)
{
    bool found = false;
    for(auto& weapon : m_weapons)
    {
        if(weapon->GetProfile().GetId() == weaponId)
        {
            weapon->LevelDown();
            found = true;
        }
    }
    if(!found)
    {
        std::cerr << "WeaponInventory::LevelDownWeapon: weapon '" << weaponId << "' not found\n";
    }
}

void WeaponInventory::OnWeaponEvolution(const std::string& evolvedWeaponId)
{
    // Evolved weapon logic detached for now until evolution mechanics are fully implemented
    (void)evolvedWeaponId;
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
