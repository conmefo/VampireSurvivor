#pragma once

#include "WeaponProfile.h"
#include <string>
#include <unordered_map>
#include <vector>
#include "WeaponProfile.h"
#include "WeaponLevelDelta.h"

class WeaponDataManager
{
public:
    WeaponDataManager();

    bool LoadData(const std::string& configFilePath);
    const WeaponProfile& GetWeaponById(const std::string& id) const;
    const std::vector<WeaponLevelDelta>& GetLevelDeltas(const std::string& id) const;
    const std::unordered_map<std::string, WeaponProfile>& GetAllWeapons() const;

private:
    std::unordered_map<std::string, WeaponProfile> m_weapons;
    std::unordered_map<std::string, std::vector<WeaponLevelDelta>> m_levelDeltas;
    std::vector<WeaponLevelDelta> m_emptyDeltas;
    WeaponProfile m_fallbackProfile;
};
