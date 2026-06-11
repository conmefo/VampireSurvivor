#pragma once
#include <unordered_set>
#include <string>
#include "CharacterProfile.h"

class PlayerProgressionManager
{
private:
    int m_gold = 0;
    std::unordered_set<std::string> m_unlockedCharacters;

public:
    PlayerProgressionManager() = default;
    
    int GetGold() const
    {
        return m_gold;
    }

    void AddGold(int amount)
    {
        m_gold += amount;
    }

    void InitializeUnlockedCharacters(const std::unordered_map<std::string, CharacterProfile>& allCharacters)
    {
        for (const auto& pair : allCharacters)
        {
            if (pair.second.IsBought() || pair.second.GetBasePrice() == 0)
            {
                m_unlockedCharacters.insert(pair.first);
            }
        }
    }

    void UnlockCharacter(const std::string& characterId)
    {
        m_unlockedCharacters.insert(characterId);
    }

    bool IsCharacterUnlocked(const std::string& characterId) const
    {
        return m_unlockedCharacters.find(characterId) != m_unlockedCharacters.end();
    }
};
