#include "ParticleDataManager.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

bool ParticleDataManager::LoadFromJson(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        std::cerr << "ParticleDataManager: Failed to open file " << filepath << std::endl;
        return false;
    }

    try
    {
        json j;
        file >> j;

        for (auto& [key, value] : j.items())
        {
            vs::ParticleEmitterConfig config;
            
            config.textureId = value.value("textureId", "");
            config.looping = value.value("looping", true);
            config.duration = value.value("duration", 1.0f);
            config.startSpeed = value.value("startSpeed", 50.0f);
            config.startLifetime = value.value("startLifetime", 1.0f);
            config.emissionRate = value.value("emissionRate", 10.0f);
            config.minSize = value.value("minSize", 0.1f);
            config.maxSize = value.value("maxSize", 1.0f);
            config.endSize = value.value("endSize", 1.0f);
            config.shapeRadius = value.value("shapeRadius", 0.0f);
            config.shapeAngle = value.value("shapeAngle", 360.0f);
            config.gravityModifier = value.value("gravityModifier", 0.0f);
            config.damping = value.value("damping", 0.0f);
            config.colorR = value.value("colorR", 255.0f);
            config.colorG = value.value("colorG", 255.0f);
            config.colorB = value.value("colorB", 255.0f);
            config.weaponScaleX = value.value("weaponScaleX", 1.0f);
            config.weaponScaleY = value.value("weaponScaleY", 1.0f);
            config.emitterOffset = value.value("emitterOffset", 0.0f);
            config.randomRotation = value.value("randomRotation", false);
            
            int blend = value.value("blendMode", 0); // 0=Alpha, 1=Add, 2=Multiply, 3=None
            if (blend == 0) config.blendMode = sf::BlendAlpha;
            else if (blend == 1) config.blendMode = sf::BlendAdd;
            else if (blend == 2) config.blendMode = sf::BlendMultiply;
            else if (blend == 3) config.blendMode = sf::BlendNone;

            config.overrideColor = value.value("overrideColor", false);

            m_configs[key] = config;
        }
        return true;
    }
    catch (const json::parse_error& e)
    {
        std::cerr << "ParticleDataManager: JSON parse error in " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

const vs::ParticleEmitterConfig& ParticleDataManager::GetConfig(const std::string& id) const
{
    auto it = m_configs.find(id);
    if (it != m_configs.end())
    {
        return it->second;
    }
    
    std::cerr << "ParticleDataManager: Config not found for id '" << id << "'" << std::endl;
    return m_defaultConfig;
}

vs::ParticleEmitterConfig& ParticleDataManager::GetMutableConfig(const std::string& id)
{
    auto it = m_configs.find(id);
    if (it != m_configs.end())
    {
        return it->second;
    }
    
    std::cerr << "ParticleDataManager: Mutable config not found for id '" << id << "', creating default." << std::endl;
    m_configs[id] = m_defaultConfig;
    return m_configs[id];
}
