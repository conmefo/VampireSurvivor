#pragma once

#include <string>

class WeaponProfile
{
private:
    std::string m_id;
    std::string m_name;
    std::string m_description;
    std::string m_frameName;

    // Gameplay parameters
    float m_power;
    float m_area;
    float m_speed;
    int m_amount;
    int m_poolLimit;

public:
    WeaponProfile(const std::string& id, const std::string& name, const std::string& description, const std::string& frameName,
                  float power = 1.0f, float area = 1.0f, float speed = 1.0f, int amount = 1, int poolLimit = 0);
    
    const std::string& GetId() const;
    const std::string& GetName() const;
    const std::string& GetDescription() const;
    const std::string& GetFrameName() const;

    float GetPower() const;
    float GetArea() const;
    float GetSpeed() const;
    int GetAmount() const;
    int GetPoolLimit() const;
};
