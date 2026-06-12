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
    // Need to refund exact amount paid for all upgrades
    // Reconstruct the cost sequence
    int totalRefund = 0;
    
    // We have to simulate the purchasing process to know how much was spent, 
    // because price scales with TotalPurchasedPowerUps.
    // However, the inflation rule in standard game is per global power-up level.
    // It's a sum of (BasePrice * (1 + 0.1 * i)) where i is the order they were bought.
    // Wait, the order of buying matters for refund? Yes.
    // Since we didn't store the exact order, it might be slightly off.
    // Let's just do a naive total refund by clearing powerups and adding back the sum
    // or just say we don't refund the inflation part perfectly without history.
    // Actually, in original game, order doesn't matter for total cost because 
    // the total inflation is the sum of (base prices * their respective global indices).
    // Let's just iterate every purchased power up and sum its base price. Then add the total inflation.
    
    int globalIndex = 0;
    // We don't have order, so just refund an approximate or exact amount if we sort it.
    for (const auto& pair : m_purchasedPowerUps)
    {
        const PowerUpProfile& profile = dataManager.GetPowerUpById(pair.first);
        for(int i = 0; i < pair.second; ++i)
        {
            totalRefund += static_cast<int>(profile.GetBasePrice() * (1.0f + 0.1f * globalIndex));
            globalIndex++;
        }
    }
    
    AddGold(totalRefund);
    m_purchasedPowerUps.clear();
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

int PlayerProgressionManager::GetNextPowerUpPrice(const std::string& powerUpId, const PowerUpDataManager& dataManager) const
{
    const PowerUpProfile& profile = dataManager.GetPowerUpById(powerUpId);
    int basePrice = profile.GetBasePrice();
    int totalPurchased = GetTotalPurchasedPowerUps();
    
    // Calculate inflated price
    return static_cast<int>(basePrice * (1.0f + 0.1f * totalPurchased));
}

float PlayerProgressionManager::GetGlobalStatBuff(const std::string& statKey, const PowerUpDataManager& powerUpData) const
{
    float totalBuff = 0.0f;
    for (const auto& pair : m_purchasedPowerUps)
    {
        const PowerUpProfile& profile = powerUpData.GetPowerUpById(pair.first);
        int level = pair.second;
        
        float buffPerLevel = profile.GetStatBuff(statKey);
        totalBuff += (buffPerLevel * level);
    }
    return totalBuff;
}
