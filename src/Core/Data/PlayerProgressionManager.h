#pragma once
#include <unordered_set>
#include <unordered_map>
#include <string>
#include "CharacterProfile.h"
#include "PowerUpDataManager.h"

class PlayerProgressionManager
{
private:
    int m_gold = 0;
    std::unordered_set<std::string> m_unlockedCharacters;
    std::unordered_map<std::string, int> m_purchasedPowerUps;

public:
    PlayerProgressionManager() = default;
    
    bool Save(const std::string& filepath = "save_data.json") const;
    bool Load(const std::string& filepath = "save_data.json");

    int GetGold() const;
    void AddGold(int amount);
    bool SpendGold(int amount);

    void InitializeUnlockedCharacters(const std::unordered_map<std::string, CharacterProfile>& allCharacters);
    void UnlockCharacter(const std::string& characterId);
    bool IsCharacterUnlocked(const std::string& characterId) const;

    int GetPowerUpLevel(const std::string& powerUpId) const;
    void BuyPowerUp(const std::string& powerUpId, const PowerUpDataManager& dataManager);
    void RefundAllPowerUps(const PowerUpDataManager& dataManager);
    
    int GetTotalPurchasedPowerUps() const;
    int GetNextPowerUpPrice(const std::string& powerUpId, const PowerUpDataManager& dataManager) const;

    float GetGlobalStatBuff(const std::string& statKey, const PowerUpDataManager& powerUpData) const;
};
