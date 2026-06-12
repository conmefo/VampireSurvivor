#include "PowerUpDataManager.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <unordered_set>

using json = nlohmann::json;

PowerUpDataManager::PowerUpDataManager()
{
    std::unordered_map<std::string, float> emptyStats;
    m_fallbackProfile = PowerUpProfile("UNKNOWN", "Unknown PowerUp", "Data missing", "Unknown", 0, 0, emptyStats);
}

bool PowerUpDataManager::LoadFromJson(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        std::cerr << "PowerUpDataManager: Failed to open file: " << filepath << std::endl;
        return false;
    }

    json root;
    try
    {
        file >> root;
    }
    catch (const json::parse_error& e)
    {
        std::cerr << "PowerUpDataManager: Parse error in file " << filepath << ": " << e.what() << std::endl;
        return false;
    }

    std::unordered_set<std::string> standardKeys = {
        "level", "bulletType", "name", "description", "texture", 
        "frameName", "isPowerUp", "price", "unlockedRank", "active", 
        "hidden", "isAnUnlockable", "specialBG"
    };

    for (auto it = root.begin(); it != root.end(); ++it)
    {
        std::string powerUpId = it.key();
        const json& levelsArray = it.value();

        if (!levelsArray.is_array() || levelsArray.empty())
        {
            continue;
        }

        const json& baseData = levelsArray[0];

        // Ensure this is an active powerup and not hidden
        bool isActive = baseData.value("active", false);
        if (!isActive)
        {
            continue;
        }

        std::string name = baseData.value("name", "Unknown");
        std::string description = baseData.value("description", "");
        int price = baseData.value("price", 0);
        int maxLevel = static_cast<int>(levelsArray.size());
        
        std::string frameName = baseData.value("frameName", "");
        // Remove .png from frameName for TextureAtlas compatibility
        size_t pngPos = frameName.find(".png");
        if (pngPos != std::string::npos)
        {
            frameName = frameName.substr(0, pngPos);
        }

        std::unordered_map<std::string, float> statBuffs;
        
        // Find stat keys
        for (auto itemIt = baseData.begin(); itemIt != baseData.end(); ++itemIt)
        {
            if (standardKeys.find(itemIt.key()) == standardKeys.end())
            {
                if (itemIt.value().is_number())
                {
                    statBuffs[itemIt.key()] = itemIt.value().get<float>();
                }
            }
        }

        PowerUpProfile profile(powerUpId, name, description, frameName, price, maxLevel, statBuffs);
        m_powerUps[powerUpId] = profile;
        m_powerUpOrder.push_back(powerUpId);
    }

    return true;
}

const PowerUpProfile& PowerUpDataManager::GetPowerUpById(const std::string& id) const
{
    auto it = m_powerUps.find(id);
    if (it != m_powerUps.end())
    {
        return it->second;
    }
    return m_fallbackProfile;
}

const std::unordered_map<std::string, PowerUpProfile>& PowerUpDataManager::GetAllPowerUps() const
{
    return m_powerUps;
}

const std::vector<std::string>& PowerUpDataManager::GetPowerUpOrder() const
{
    return m_powerUpOrder;
}
