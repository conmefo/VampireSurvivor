#pragma once

#include "WeaponProfile.h"
#include <string>
#include <unordered_map>

class WeaponDataManager
{
private:
    std::unordered_map<std::string, WeaponProfile> m_weapons;
    WeaponProfile m_fallbackProfile;

public:
    WeaponDataManager();

    bool LoadData(const std::string& configFilePath);

    const WeaponProfile& GetWeaponById(const std::string& id) const;
    const std::unordered_map<std::string, WeaponProfile>& GetAllWeapons() const;
};
