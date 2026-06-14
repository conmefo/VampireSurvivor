#pragma once

#include <string>
#include <cstdint>

class HitVfxProfile
{
private:
    int m_id;
    bool m_isTintFill;
    uint32_t m_targetTint;
    std::string m_hitFrameName;
    std::string m_impactFrameName;
    int m_duration;

public:
    HitVfxProfile(int id, bool isTintFill, uint32_t targetTint, const std::string& hitFrameName, const std::string& impactFrameName, int duration);

    int GetId() const;
    bool IsTintFill() const;
    uint32_t GetTargetTint() const;
    const std::string& GetHitFrameName() const;
    const std::string& GetImpactFrameName() const;
    int GetDuration() const;
};
