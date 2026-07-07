#pragma once

#include "ParticleEmitter.h"
#include "../../Core/Resources/TextureAtlas.h"
#include "../../Core/Data/ParticleDataManager.h"
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>

namespace vs
{

class ParticleManager
{
public:
    ParticleManager();
    ~ParticleManager() = default;

    void Initialize(const TextureAtlas* atlas, const ParticleDataManager* dataManager);

    const ParticleEmitterConfig& GetConfig(const std::string& id) const;

    // Spawns a new emitter and returns a pointer to it so its config can be tweaked dynamically
    ParticleEmitter* SpawnEmitter(const ParticleEmitterConfig& config, const sf::Vector2f& position);

    void Update(float dt);
    void Draw(sf::RenderTarget& target) const;

    void Clear();

private:
    const TextureAtlas* m_atlas = nullptr;
    const ParticleDataManager* m_dataManager = nullptr;
    std::vector<std::unique_ptr<ParticleEmitter>> m_emitters;
    
    // Pre-allocated vertex array to avoid reallocations
    mutable sf::VertexArray m_vertices;
};

} // namespace vs
