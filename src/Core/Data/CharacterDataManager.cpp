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

    nlohmann::ordered_json jsonData;
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
    m_characterOrder.clear();

    for(auto& el : jsonData.items())
    {
        std::string id = el.key();
        
        if(!el.value().is_array() || el.value().empty())
        {
            continue;
        }

        m_characterOrder.push_back(id);

        const auto& charJson = el.value()[0];

        std::string charName = charJson.value("charName", "Unknown");
        std::string surname = charJson.value("surname", "");
        std::string name = charName;
        if(!surname.empty())
        {
            name += " " + surname;
        }

        std::string description = charJson.value("description", "");
        
        std::string spriteName = charJson.value("spriteName", "");
        size_t dotPos = spriteName.find_last_of('.');
        if(dotPos != std::string::npos)
        {
            spriteName = spriteName.substr(0, dotPos);
        }
        std::string portraitTextureId = spriteName;

        std::string startingWeaponId = charJson.value("startingWeapon", "");
        int basePrice = charJson.value("price", 0);

        std::unordered_map<std::string, float> stats;
        std::vector<std::string> statKeys = {
            "maxHp", "armor", "regen", "moveSpeed", "power", "cooldown",
            "area", "speed", "duration", "amount", "luck", "growth",
            "greed", "curse", "magnet", "revivals", "rerolls", "skips", "banish"
        };

        for(const auto& key : statKeys)
        {
            if(charJson.contains(key) && charJson[key].is_number())
            {
                stats[key] = charJson[key].get<float>();
            }
        }

        bool isAlwaysHidden = charJson.value("alwaysHidden", false);
        bool isBought = charJson.value("isBought", false);

        CharacterProfile profile(id, name, description, portraitTextureId, startingWeaponId, basePrice, stats, isAlwaysHidden, isBought);
        m_characters.insert({id, profile});
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

const std::vector<std::string>& CharacterDataManager::GetCharacterOrder() const
{
    return m_characterOrder;
}
