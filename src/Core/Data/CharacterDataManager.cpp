#include "CharacterDataManager.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

CharacterDataManager::CharacterDataManager()
    : m_fallbackProfile("UNKNOWN", "Unknown", "Fallback Data", "default_portrait", "default_weapon", 0, std::unordered_map<std::string, float>())
{
}

bool CharacterDataManager::LoadData(const std::string& configFilePath)
{
    std::ifstream file(configFilePath);
    if(!file.is_open())
    {
        std::cerr << "Failed to open Character configuration file: " << configFilePath << "\n";
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

    m_characters.clear();

    if(jsonData.contains("characters") && jsonData["characters"].is_array())
    {
        for(const auto& charJson : jsonData["characters"])
        {
            std::string id = charJson.value("id", "UNKNOWN");
            std::string name = charJson.value("name", "Unknown");
            std::string description = charJson.value("description", "");
            std::string portraitTextureId = charJson.value("portraitTextureId", "");
            std::string startingWeaponId = charJson.value("startingWeaponId", "");
            int basePrice = charJson.value("basePrice", 0);

            std::unordered_map<std::string, float> stats;
            if(charJson.contains("stats") && charJson["stats"].is_object())
            {
                for(auto& el : charJson["stats"].items())
                {
                    stats[el.key()] = el.value().get<float>();
                }
            }

            CharacterProfile profile(id, name, description, portraitTextureId, startingWeaponId, basePrice, stats);
            m_characters.insert({id, profile});
        }
    }

    return true;
}

const CharacterProfile& CharacterDataManager::GetCharacterById(const std::string& id) const
{
    auto it = m_characters.find(id);
    if(it != m_characters.end())
    {
        return it->second;
    }

    std::cerr << "Character ID not found: " << id << ". Returning fallback profile.\n";
    return m_fallbackProfile;
}

const std::unordered_map<std::string, CharacterProfile>& CharacterDataManager::GetAllCharacters() const
{
    return m_characters;
}
