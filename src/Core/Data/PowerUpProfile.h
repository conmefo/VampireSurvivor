#pragma once

#include <string>
#include <unordered_map>

class PowerUpProfile
{
private:
    std::string m_id;
    std::string m_name;
    std::string m_description;
    std::string m_frameName;
    int m_basePrice;
    int m_maxLevel;
    std::unordered_map<std::string, float> m_statBuffsPerLevel;

public:
    PowerUpProfile() : m_basePrice(0), m_maxLevel(0) {}

    PowerUpProfile(
        const std::string& id,
        const std::string& name,
        const std::string& description,
        const std::string& frameName,
        int basePrice,
        int maxLevel,
        const std::unordered_map<std::string, float>& statBuffsPerLevel)
        : m_id(id)
        , m_name(name)
        , m_description(description)
        , m_frameName(frameName)
        , m_basePrice(basePrice)
        , m_maxLevel(maxLevel)
        , m_statBuffsPerLevel(statBuffsPerLevel)
    {
    }

    const std::string& GetId() const { return m_id; }
    const std::string& GetName() const { return m_name; }
    const std::string& GetDescription() const { return m_description; }
    const std::string& GetFrameName() const { return m_frameName; }
    int GetBasePrice() const { return m_basePrice; }
    int GetMaxLevel() const { return m_maxLevel; }
    
    const std::unordered_map<std::string, float>& GetStatBuffsPerLevel() const { return m_statBuffsPerLevel; }

    float GetStatBuff(const std::string& statKey) const
    {
        auto it = m_statBuffsPerLevel.find(statKey);
        if (it != m_statBuffsPerLevel.end())
        {
            return it->second;
        }
        return 0.0f;
    }
};
