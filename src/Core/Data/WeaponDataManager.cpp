#include "WeaponDataManager.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

WeaponDataManager::WeaponDataManager()
    : m_fallbackProfile("UNKNOWN", "Unknown Weapon", "Fallback Data", "default_weapon", 1.0f, 1.0f, 1.0f, 1, 0)
{
}

bool WeaponDataManager::LoadData(const std::string& configFilePath)
{
    std::ifstream file(configFilePath);
    if(!file.is_open())
    {
        std::cerr << "Failed to open Weapon configuration file: " << configFilePath << "\n";
        return false;
    }

    nlohmann::json jsonData;
    try
    {
        file >> jsonData;
    }
    catch(const nlohmann::json::parse_error& e)
    {
        std::cerr << "JSON parse error in file " << configFilePath << ": " << e.what() << "\n";
        return false;
    }

    m_weapons.clear();

    for(auto& el : jsonData.items())
    {
        std::string id = el.key();
        
        if(!el.value().is_array() || el.value().empty())
        {
            continue;
        }

        const auto& weaponJson = el.value()[0];

        std::string name = weaponJson.value("name", "Unknown");
        std::string description = weaponJson.value("description", "");
        std::string frameName = weaponJson.value("frameName", "");
        
        // Strip .png extension if it exists to safely match items_atlas string keys
        size_t dotPos = frameName.find_last_of('.');
        if(dotPos != std::string::npos)
        {
            frameName = frameName.substr(0, dotPos);
        }

        float power = weaponJson.value("power", 1.0f);
        float area = weaponJson.value("area", 1.0f);
        float speed = weaponJson.value("speed", 1.0f);
        int amount = weaponJson.value("amount", 1);
        int poolLimit = weaponJson.value("poolLimit", 0);

        WeaponProfile profile(id, name, description, frameName, power, area, speed, amount, poolLimit);
        m_weapons.insert({id, profile});
    }

    return true;
}

const WeaponProfile& WeaponDataManager::GetWeaponById(const std::string& id) const
{
    auto it = m_weapons.find(id);
    if(it != m_weapons.end())
    {
        return it->second;
    }
    return m_fallbackProfile;
}

const std::unordered_map<std::string, WeaponProfile>& WeaponDataManager::GetAllWeapons() const
{
    return m_weapons;
}
