#include "WeaponProfile.h"

WeaponProfile::WeaponProfile(const std::string& id, const std::string& name, const std::string& description, const std::string& frameName,
                             const std::string& bulletType, const std::string& hitVFX,
                             float power, float area, float speed, float duration, float hitBoxDelay, float magnet, int amount, int poolLimit,
                             int interval, int repeatInterval, int penetrating, int rarity,
                             float knockback, bool isPowerUp, bool isUnlocked)
    : m_id(id)
    , m_name(name)
    , m_description(description)
    , m_frameName(frameName)
    , m_bulletType(bulletType)
    , m_hitVFX(hitVFX)
    , m_power(power)
    , m_area(area)
    , m_speed(speed)
    , m_duration(duration)
    , m_hitBoxDelay(hitBoxDelay)
    , m_magnet(magnet)
    , m_amount(amount)
    , m_poolLimit(poolLimit)
    , m_interval(interval)
    , m_repeatInterval(repeatInterval)
    , m_penetrating(penetrating)
    , m_knockback(knockback)
    , m_currentLevel(1)
    , m_rarity(rarity)
    , m_isPowerUp(isPowerUp)
    , m_isUnlocked(isUnlocked)
{
}

void WeaponProfile::ApplyDelta(const WeaponLevelDelta& delta)
{
    m_power          += delta.power;
    m_area           += delta.area;
    m_speed          += delta.speed;
    m_duration       += delta.duration;
    m_hitBoxDelay    += delta.hitBoxDelay;
    m_magnet         += delta.magnet;
    m_knockback      += delta.knockback;
    m_amount         += delta.amount;
    m_interval       += delta.interval;
    m_repeatInterval += delta.repeatInterval;
    m_penetrating    += delta.penetrating;
    m_poolLimit      += delta.poolLimit;
    m_currentLevel++;
}

void WeaponProfile::RevertDelta(const WeaponLevelDelta& delta)
{
    m_power          -= delta.power;
    m_area           -= delta.area;
    m_speed          -= delta.speed;
    m_duration       -= delta.duration;
    m_hitBoxDelay    -= delta.hitBoxDelay;
    m_magnet         -= delta.magnet;
    m_knockback      -= delta.knockback;
    m_amount         -= delta.amount;
    m_interval       -= delta.interval;
    m_repeatInterval -= delta.repeatInterval;
    m_penetrating    -= delta.penetrating;
    m_poolLimit      -= delta.poolLimit;
    m_currentLevel--;
}

const std::string& WeaponProfile::GetId() const { return m_id; }
const std::string& WeaponProfile::GetName() const { return m_name; }
const std::string& WeaponProfile::GetDescription() const { return m_description; }
const std::string& WeaponProfile::GetFrameName() const { return m_frameName; }
const std::string& WeaponProfile::GetBulletType() const { return m_bulletType; }
const std::string& WeaponProfile::GetHitVFX() const { return m_hitVFX; }

float WeaponProfile::GetPower() const { return m_power; }
float WeaponProfile::GetArea() const { return m_area; }
float WeaponProfile::GetSpeed() const { return m_speed; }
float WeaponProfile::GetDuration() const { return m_duration; }
float WeaponProfile::GetHitBoxDelay() const { return m_hitBoxDelay; }
float WeaponProfile::GetMagnet() const { return m_magnet; }
float WeaponProfile::GetKnockback() const { return m_knockback; }
int   WeaponProfile::GetAmount() const { return m_amount; }
int   WeaponProfile::GetPoolLimit() const { return m_poolLimit; }
int   WeaponProfile::GetInterval() const { return m_interval; }
int   WeaponProfile::GetRepeatInterval() const { return m_repeatInterval; }
int   WeaponProfile::GetPenetrating() const { return m_penetrating; }
int   WeaponProfile::GetCurrentLevel() const { return m_currentLevel; }
int   WeaponProfile::GetRarity() const { return m_rarity; }
bool  WeaponProfile::IsPowerUp() const { return m_isPowerUp; }
bool  WeaponProfile::IsUnlocked() const { return m_isUnlocked; }

const std::unordered_map<std::string, float>& WeaponProfile::GetSpecialStats() const
{
    return m_specialStats;
}

void WeaponProfile::SetSpecialStats(const std::unordered_map<std::string, float>& stats)
{
    m_specialStats = stats;
}
