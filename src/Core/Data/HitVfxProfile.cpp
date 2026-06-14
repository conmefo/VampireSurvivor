#include "HitVfxProfile.h"

HitVfxProfile::HitVfxProfile(int id, bool isTintFill, uint32_t targetTint, const std::string& hitFrameName, const std::string& impactFrameName, int duration)
    : m_id(id)
    , m_isTintFill(isTintFill)
    , m_targetTint(targetTint)
    , m_hitFrameName(hitFrameName)
    , m_impactFrameName(impactFrameName)
    , m_duration(duration)
{
}

int HitVfxProfile::GetId() const { return m_id; }
bool HitVfxProfile::IsTintFill() const { return m_isTintFill; }
uint32_t HitVfxProfile::GetTargetTint() const { return m_targetTint; }
const std::string& HitVfxProfile::GetHitFrameName() const { return m_hitFrameName; }
const std::string& HitVfxProfile::GetImpactFrameName() const { return m_impactFrameName; }
int HitVfxProfile::GetDuration() const { return m_duration; }
