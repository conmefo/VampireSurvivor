#include "WeaponProfile.h"

WeaponProfile::WeaponProfile(const std::string& id, const std::string& name, const std::string& description, const std::string& frameName,
                             float power, float area, float speed, int amount, int poolLimit)
    : m_id(id)
    , m_name(name)
    , m_description(description)
    , m_frameName(frameName)
    , m_power(power)
    , m_area(area)
    , m_speed(speed)
    , m_amount(amount)
    , m_poolLimit(poolLimit)
{
}

const std::string& WeaponProfile::GetId() const { return m_id; }
const std::string& WeaponProfile::GetName() const { return m_name; }
const std::string& WeaponProfile::GetDescription() const { return m_description; }
const std::string& WeaponProfile::GetFrameName() const { return m_frameName; }

float WeaponProfile::GetPower() const { return m_power; }
float WeaponProfile::GetArea() const { return m_area; }
float WeaponProfile::GetSpeed() const { return m_speed; }
int WeaponProfile::GetAmount() const { return m_amount; }
int WeaponProfile::GetPoolLimit() const { return m_poolLimit; }
