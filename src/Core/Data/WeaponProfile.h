#pragma once

#include <string>
#include "WeaponLevelDelta.h"

class WeaponProfile
{
private:
    std::string m_id;
    std::string m_name;
    std::string m_description;
    std::string m_frameName;
    std::string m_bulletType;
    std::string m_hitVFX;

    // Gameplay parameters — mutable at runtime via ApplyDelta()
    float m_power;
    float m_area;
    float m_speed;
    float m_duration;
    float m_hitBoxDelay;
    int   m_amount;
    int   m_poolLimit;
    int   m_interval;
    int   m_repeatInterval;
    int   m_penetrating;
    int   m_currentLevel;

public:
    WeaponProfile(const std::string& id, const std::string& name, const std::string& description, const std::string& frameName,
                  const std::string& bulletType, const std::string& hitVFX,
                  float power = 1.0f, float area = 1.0f, float speed = 1.0f, float duration = 2.0f, float hitBoxDelay = 0.5f, int amount = 1, int poolLimit = 0,
                  int interval = 1000, int repeatInterval = 0, int penetrating = 1);

    // Accumulates a level delta into runtime stats. Called by Weapon::LevelUp().
    void ApplyDelta(const WeaponLevelDelta& delta);
    // Reverts the last applied delta. Called by Weapon::LevelDown().
    void RevertDelta(const WeaponLevelDelta& delta);

    const std::string& GetId() const;
    const std::string& GetName() const;
    const std::string& GetDescription() const;
    const std::string& GetFrameName() const;
    const std::string& GetBulletType() const;
    const std::string& GetHitVFX() const;

    float GetPower() const;
    float GetArea() const;
    float GetSpeed() const;
    float GetDuration() const;
    float GetHitBoxDelay() const;
    int   GetAmount() const;
    int   GetPoolLimit() const;
    int   GetInterval() const;
    int   GetRepeatInterval() const;
    int   GetPenetrating() const;
    int   GetCurrentLevel() const;
};
