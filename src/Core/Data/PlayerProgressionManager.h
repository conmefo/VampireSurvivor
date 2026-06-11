#pragma once

class PlayerProgressionManager
{
private:
    int m_gold = 0;

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

    bool IsCharacterUnlocked(const std::string& characterId) const
    {
        if (characterId.find("gennaro") != std::string::npos || characterId.find("Gennaro") != std::string::npos)
        {
            return false;
        }
        return true;
    }
};
