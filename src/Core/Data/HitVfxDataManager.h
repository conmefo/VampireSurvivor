#pragma once

#include "HitVfxProfile.h"
#include <unordered_map>
#include <string>

class HitVfxDataManager
{
private:
    std::unordered_map<int, HitVfxProfile> m_profiles;
    HitVfxProfile m_fallbackProfile;

public:
    HitVfxDataManager();

    bool LoadData(const std::string& configFilePath);

    const HitVfxProfile& GetVfxById(int id) const;
    const HitVfxProfile& GetVfxByName(const std::string& hitVfxName) const;
    
    const std::unordered_map<int, HitVfxProfile>& GetAllProfiles() const;
};
