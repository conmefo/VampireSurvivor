#pragma once

#include "PowerUpProfile.h"
#include <unordered_map>
#include <string>
#include <vector>

class PowerUpDataManager
{
private:
    std::unordered_map<std::string, PowerUpProfile> m_powerUps;
    std::vector<std::string> m_powerUpOrder; // To keep the UI display order consistent with JSON
    PowerUpProfile m_fallbackProfile;

public:
    PowerUpDataManager();
    
    bool LoadFromJson(const std::string& filepath);
    
    const PowerUpProfile& GetPowerUpById(const std::string& id) const;
    const std::unordered_map<std::string, PowerUpProfile>& GetAllPowerUps() const;
    const std::vector<std::string>& GetPowerUpOrder() const;
};
