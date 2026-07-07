#pragma once

#include <string>
#include <unordered_map>
#include "../../Entities/Particles/ParticleEmitter.h"

class ParticleDataManager
{
public:
    ParticleDataManager() = default;
    ~ParticleDataManager() = default;

    bool LoadFromJson(const std::string& filepath);
    const vs::ParticleEmitterConfig& GetConfig(const std::string& id) const;
    vs::ParticleEmitterConfig& GetMutableConfig(const std::string& id);

private:
    std::unordered_map<std::string, vs::ParticleEmitterConfig> m_configs;
    vs::ParticleEmitterConfig m_defaultConfig;
};
