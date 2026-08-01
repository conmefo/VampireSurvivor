#include "PlayerProgressionManager.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

bool PlayerProgressionManager::Save(const std::string& filepath) const
{
    json j;
    j["gold"] = m_gold;
    
    json charsArray = json::array();
    for (const std::string& charId : m_unlockedCharacters)
    {
        charsArray.push_back(charId);
    }
    j["unlockedCharacters"] = charsArray;
    
    json powerupsObj = json::object();
    for (const auto& pair : m_purchasedPowerUps)
    {
        powerupsObj[pair.first] = pair.second;
    }
    j["purchasedPowerUps"] = powerupsObj;

    json disabledArray = json::array();
    for (const std::string& disId : m_disabledPowerUps)
    {
        disabledArray.push_back(disId);
    }
    j["disabledPowerUps"] = disabledArray;
    
    std::ofstream file(filepath);
    if (!file.is_open())
    {
        std::cerr << "PlayerProgressionManager: Failed to open save file for writing: " << filepath << std::endl;
        return false;
    }
    
    file << j.dump(4);
    return true;
}

bool PlayerProgressionManager::Load(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        // File doesn't exist, which is fine for first time players
        return false;
    }
    
    json j;
    try
    {
        file >> j;
    }
    catch (const json::parse_error& e)
    {
        std::cerr << "PlayerProgressionManager: Parse error in save file: " << e.what() << std::endl;
        return false;
    }
    
    m_gold = j.value("gold", 0);
    
    m_unlockedCharacters.clear();
    if (j.contains("unlockedCharacters") && j["unlockedCharacters"].is_array())
    {
        for (const auto& item : j["unlockedCharacters"])
        {
            if (item.is_string())
            {
                m_unlockedCharacters.insert(item.get<std::string>());
            }
        }
    }
    
    m_purchasedPowerUps.clear();
    if (j.contains("purchasedPowerUps") && j["purchasedPowerUps"].is_object())
    {
        for (auto it = j["purchasedPowerUps"].begin(); it != j["purchasedPowerUps"].end(); ++it)
        {
            if (it.value().is_number_integer())
            {
                m_purchasedPowerUps[it.key()] = it.value().get<int>();
            }
        }
    }

    m_disabledPowerUps.clear();
    if (j.contains("disabledPowerUps") && j["disabledPowerUps"].is_array())
    {
        for (const auto& item : j["disabledPowerUps"])
        {
            if (item.is_string())
            {
                m_disabledPowerUps.insert(item.get<std::string>());
            }
        }
    }
    
    return true;
}

int PlayerProgressionManager::GetGold() const
{
    return m_gold;
}

void PlayerProgressionManager::AddGold(int amount)
{
    m_gold += amount;
}

bool PlayerProgressionManager::SpendGold(int amount)
{
    if (m_gold >= amount)
    {
        m_gold -= amount;
        return true;
    }
    return false;
}

void PlayerProgressionManager::InitializeUnlockedCharacters(const std::unordered_map<std::string, CharacterProfile>& allCharacters)
{
    for (const auto& pair : allCharacters)
    {
        if (pair.second.IsBought() || pair.second.GetBasePrice() == 0)
        {
            m_unlockedCharacters.insert(pair.first);
        }
    }
}

void PlayerProgressionManager::UnlockCharacter(const std::string& characterId)
{
    m_unlockedCharacters.insert(characterId);
}

bool PlayerProgressionManager::IsCharacterUnlocked(const std::string& characterId) const
{
    return m_unlockedCharacters.find(characterId) != m_unlockedCharacters.end();
}

int PlayerProgressionManager::GetPowerUpLevel(const std::string& powerUpId) const
{
    auto it = m_purchasedPowerUps.find(powerUpId);
    if (it != m_purchasedPowerUps.end())
    {
        return it->second;
    }
    return 0;
}

void PlayerProgressionManager::BuyPowerUp(const std::string& powerUpId, const PowerUpDataManager& dataManager)
{
    const PowerUpProfile& profile = dataManager.GetPowerUpById(powerUpId);
    int currentLvl = GetPowerUpLevel(powerUpId);
    
    if (currentLvl >= profile.GetMaxLevel()) return;
    
    int price = GetNextPowerUpPrice(powerUpId, dataManager);
    if (SpendGold(price))
    {
        m_purchasedPowerUps[powerUpId] = currentLvl + 1;
        Save();
    }
}

void PlayerProgressionManager::RefundAllPowerUps(const PowerUpDataManager& dataManager)
{
    int totalRefund = 0;
    int globalIndex = 0;
    
    for (const auto& pair : m_purchasedPowerUps)
    {
        const PowerUpProfile& profile = dataManager.GetPowerUpById(pair.first);
        int basePrice = profile.GetBasePrice();
        for(int lvl = 0; lvl < pair.second; ++lvl)
        {
            // Exact Ghidra cost formula: (tierIndex + 1) * basePrice + markup(globalIndex)
            int markup = globalIndex * 20;
            int cost = (lvl + 1) * basePrice + markup;
            totalRefund += cost;
            globalIndex++;
        }
    }
    
    AddGold(totalRefund);
    m_purchasedPowerUps.clear();
    m_disabledPowerUps.clear();
    Save();
}

bool PlayerProgressionManager::IsPowerUpDisabled(const std::string& powerUpId) const
{
    return m_disabledPowerUps.find(powerUpId) != m_disabledPowerUps.end();
}

void PlayerProgressionManager::ToggleDisablePowerUp(const std::string& powerUpId)
{
    if(m_disabledPowerUps.find(powerUpId) != m_disabledPowerUps.end())
    {
        m_disabledPowerUps.erase(powerUpId);
    }
    else
    {
        m_disabledPowerUps.insert(powerUpId);
    }
    Save();
}

int PlayerProgressionManager::GetTotalPurchasedPowerUps() const
{
    int total = 0;
    for (const auto& pair : m_purchasedPowerUps)
    {
        total += pair.second;
    }
    return total;
}

int PlayerProgressionManager::GetPowerUpMarkup() const
{
    // Ghidra: get_PowerUpMarkUp = totalBoughtLevels * 20
    return GetTotalPurchasedPowerUps() * 20;
}

int PlayerProgressionManager::GetNextPowerUpPrice(const std::string& powerUpId, const PowerUpDataManager& dataManager) const
{
    const PowerUpProfile& profile = dataManager.GetPowerUpById(powerUpId);
    int basePrice = profile.GetBasePrice();
    int currentBoughtLevel = GetPowerUpLevel(powerUpId);
    
    // Exact Ghidra formula from PlayerStats$$GetPrice:
    // (currentBoughtLevel + 1) * basePrice + PowerUpMarkUp
    int tierBaseCost = (currentBoughtLevel + 1) * basePrice;
    int markup = GetPowerUpMarkup();
    
    return tierBaseCost + markup;
}

float PlayerProgressionManager::GetGlobalStatBuff(const std::string& statKey, const PowerUpDataManager& powerUpData) const
{
    float totalBuff = 0.0f;
    for (const auto& pair : m_purchasedPowerUps)
    {
        // Skip sealed / disabled powerups (matches GameManager$$ApplyPurchasedPowerUpData)
        if (IsPowerUpDisabled(pair.first))
        {
            continue;
        }

        const PowerUpProfile& profile = powerUpData.GetPowerUpById(pair.first);
        int level = pair.second;
        
        float buffPerLevel = profile.GetStatBuff(statKey);
        totalBuff += (buffPerLevel * level);
    }
    return totalBuff;
}
