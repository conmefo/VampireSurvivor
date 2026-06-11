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
};
