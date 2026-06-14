#include "HitVfxDataManager.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

HitVfxDataManager::HitVfxDataManager()
    : m_fallbackProfile(-1, false, 0, "", "", 0)
{
}

bool HitVfxDataManager::LoadData(const std::string& configFilePath)
{
    std::ifstream file(configFilePath);
    if(!file.is_open())
    {
        std::cerr << "Failed to open HitVFX configuration file: " << configFilePath << "\n";
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

    m_profiles.clear();

    for(auto& el : jsonData.items())
    {
        int id = std::stoi(el.key());
        const auto& vfxJson = el.value();

        bool isTintFill = vfxJson.value("isTintFill", false);
        uint32_t targetTint = vfxJson.value("targetTint", 0);
        
        std::string hitFrameName = vfxJson.value("hitFrameName", "");
        std::string impactFrameName = vfxJson.value("impactFrameName", "");
        
        // Strip .png extension
        size_t dotPos = hitFrameName.find_last_of('.');
        if(dotPos != std::string::npos)
        {
            hitFrameName = hitFrameName.substr(0, dotPos);
        }
        
        dotPos = impactFrameName.find_last_of('.');
        if(dotPos != std::string::npos)
        {
            impactFrameName = impactFrameName.substr(0, dotPos);
        }

        int duration = vfxJson.value("duration", 0);

        HitVfxProfile profile(id, isTintFill, targetTint, hitFrameName, impactFrameName, duration);
        m_profiles.insert({id, profile});
    }

    return true;
}

const HitVfxProfile& HitVfxDataManager::GetVfxById(int id) const
{
    auto it = m_profiles.find(id);
    if(it != m_profiles.end())
    {
        return it->second;
    }
    return m_fallbackProfile;
}

const HitVfxProfile& HitVfxDataManager::GetVfxByName(const std::string& hitVfxName) const
{
    // The authentic Rosetta Stone mapping from IL2CPP HitVfxType enum
    int id = 0; // Default to None

    if(hitVfxName == "Default") id = 1;
    else if(hitVfxName == "TimeFreeze") id = 2;
    else if(hitVfxName == "Fire") id = 3;
    else if(hitVfxName == "Dark") id = 4;
    else if(hitVfxName == "Light") id = 5;
    else if(hitVfxName == "Push") id = 6;
    else if(hitVfxName == "Prism") id = 7;
    else if(hitVfxName == "Red") id = 8;
    else if(hitVfxName == "Blue") id = 9;
    else if(hitVfxName == "Beam") id = 10;
    else if(hitVfxName == "Evil") id = 11;
    else if(hitVfxName == "Wind") id = 12;
    else if(hitVfxName == "Explo") id = 13;
    else if(hitVfxName == "Yellow") id = 14;
    else if(hitVfxName == "Water") id = 15;
    
    return GetVfxById(id);
}

const std::unordered_map<int, HitVfxProfile>& HitVfxDataManager::GetAllProfiles() const
{
    return m_profiles;
}
