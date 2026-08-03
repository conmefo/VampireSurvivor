#include "WeaponDataManager.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

WeaponDataManager::WeaponDataManager()
    : m_fallbackProfile("UNKNOWN", "Unknown Weapon", "Fallback Data", "default_weapon", "NONE", "", 1.0f, 1.0f, 1.0f, 1, 0)
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
    m_levelDeltas.clear();

    for(auto& el : jsonData.items())
    {
        std::string id = el.key();

        if(!el.value().is_array() || el.value().empty())
        {
            continue;
        }

        // --- Base stats from index [0] ---
        const auto& weaponJson = el.value()[0];

        std::string name = weaponJson.value("name", "Unknown");
        std::string description = weaponJson.value("description", "");
        std::string frameName = weaponJson.value("frameName", "");
        std::string bulletType = weaponJson.value("bulletType", "NONE");
        std::string hitVFX = weaponJson.value("hitVFX", "");

        // Strip .png extension if it exists to safely match items_atlas string keys
        size_t dotPos = frameName.find_last_of('.');
        if(dotPos != std::string::npos)
        {
            frameName = frameName.substr(0, dotPos);
        }

        float power = weaponJson.value("power", 1.0f);
        float area = weaponJson.value("area", 1.0f);
        float speed = weaponJson.value("speed", 1.0f);
        float duration = weaponJson.value("duration", 2000) / 1000.0f; // ms to seconds
        float hitBoxDelay = weaponJson.value("hitBoxDelay", 500) / 1000.0f; // ms to seconds
        int amount = weaponJson.value("amount", 1);
        int poolLimit = weaponJson.value("poolLimit", 0);
        int interval = weaponJson.value("interval", 1000);
        int repeatInterval = weaponJson.value("repeatInterval", 0);
        int penetrating = weaponJson.value("penetrating", 1);

        WeaponProfile profile(id, name, description, frameName, bulletType, hitVFX, power, area, speed, duration, hitBoxDelay, amount, poolLimit, interval, repeatInterval, penetrating);
        m_weapons.insert({id, profile});

        // --- Level deltas from indices [1..N] ---
        std::vector<WeaponLevelDelta> deltas;
        const auto& levelsArray = el.value();
        for(size_t i = 1; i < levelsArray.size(); ++i)
        {
            const auto& levelJson = levelsArray[i];
            WeaponLevelDelta delta;
            delta.power          = levelJson.value("power", 0.0f);
            delta.area           = levelJson.value("area", 0.0f);
            delta.speed          = levelJson.value("speed", 0.0f);
            delta.duration       = levelJson.value("duration", 0) / 1000.0f; // ms to seconds
            delta.hitBoxDelay    = levelJson.value("hitBoxDelay", 0) / 1000.0f; // ms to seconds
            delta.amount         = levelJson.value("amount", 0);
            delta.interval       = levelJson.value("interval", 0);
            delta.repeatInterval = levelJson.value("repeatInterval", 0);
            delta.penetrating    = levelJson.value("penetrating", 0);
            delta.poolLimit      = levelJson.value("poolLimit", 0);
            delta.addEvolvedWeapon = levelJson.value("addEvolvedWeapon", "");
            deltas.push_back(delta);
        }
        m_levelDeltas.insert({id, std::move(deltas)});
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

const std::vector<WeaponLevelDelta>& WeaponDataManager::GetLevelDeltas(const std::string& id) const
{
    auto it = m_levelDeltas.find(id);
    if(it != m_levelDeltas.end())
    {
        return it->second;
    }
    return m_emptyDeltas;
}

const std::unordered_map<std::string, WeaponProfile>& WeaponDataManager::GetAllWeapons() const
{
    return m_weapons;
}
