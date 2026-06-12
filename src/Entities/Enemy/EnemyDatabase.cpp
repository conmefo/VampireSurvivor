#include "EnemyDatabase.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

namespace
{
    float GetFloatOrDefault(const nlohmann::json& json, const char* key, float fallback)
    {
        return json.contains(key) ? json[key].get<float>() : fallback;
    }

    int GetIntOrDefault(const nlohmann::json& json, const char* key, int fallback)
    {
        return json.contains(key) ? static_cast<int>(json[key].get<float>()) : fallback;
    }
}

bool EnemyDatabase::LoadFromFile(const std::string& filepath)
{
    std::ifstream file(filepath);
    if(!file.is_open())
    {
        std::cerr << "EnemyDatabase: Failed to open " << filepath << std::endl;
        return false;
    }

    nlohmann::json json;
    try
    {
        file >> json;
    }
    catch(const nlohmann::json::parse_error& e)
    {
        std::cerr << "EnemyDatabase: Parse error in " << filepath << ": " << e.what() << std::endl;
        return false;
    }

    if(!json.contains("atlas") || !json.contains("animations") || !json.contains("stats"))
    {
        std::cerr << "EnemyDatabase: Missing atlas, animations, or stats in " << filepath << std::endl;
        return false;
    }

    m_definitions.clear();

    const nlohmann::json& atlasJson = json["atlas"];
    const nlohmann::json& animationsJson = json["animations"];
    const nlohmann::json& statsJson = json["stats"];

    for(auto animationIt = animationsJson.begin(); animationIt != animationsJson.end(); ++animationIt)
    {
        const std::string enemyId = animationIt.key();
        const nlohmann::json& enemyAnimationJson = animationIt.value();

        EnemyDefinition definition;
        definition.id = enemyId;
        definition.name = enemyAnimationJson.value("name", enemyId);

        if(statsJson.contains(enemyId))
        {
            const nlohmann::json& statJson = statsJson[enemyId];
            definition.stats.maxHealth = GetFloatOrDefault(statJson, "maxHp", definition.stats.maxHealth);
            definition.stats.speed = GetFloatOrDefault(statJson, "speed", definition.stats.speed);
            definition.stats.damage = GetFloatOrDefault(statJson, "power", definition.stats.damage);
            definition.stats.mass = GetFloatOrDefault(statJson, "mass", definition.stats.mass);
            definition.stats.collisionRadius = GetFloatOrDefault(statJson, "collisionRadius", definition.stats.collisionRadius);
            definition.stats.expYield = GetIntOrDefault(statJson, "xp", definition.stats.expYield);
            definition.spriteScale = GetFloatOrDefault(statJson, "spriteScale", definition.spriteScale);
        }

        if(enemyAnimationJson.contains("states"))
        {
            const nlohmann::json& statesJson = enemyAnimationJson["states"];
            for(auto stateIt = statesJson.begin(); stateIt != statesJson.end(); ++stateIt)
            {
                const std::string stateName = stateIt.key();
                const nlohmann::json& stateJson = stateIt.value();

                EnemyAnimationDefinition animation;
                animation.frameDuration = GetFloatOrDefault(stateJson, "frameDuration", animation.frameDuration);
                animation.isLooping = stateJson.value("isLooping", animation.isLooping);

                if(stateJson.contains("frames"))
                {
                    for(const nlohmann::json& frameNameJson : stateJson["frames"])
                    {
                        const std::string frameName = frameNameJson.get<std::string>();
                        if(!atlasJson.contains(frameName))
                        {
                            std::cerr << "EnemyDatabase: Missing atlas frame " << frameName << std::endl;
                            continue;
                        }

                        const nlohmann::json& frameJson = atlasJson[frameName];
                        const std::string texturePath = frameJson.value("texture", "");
                        const sf::Texture* texture = LoadTexture(texturePath);
                        if(!texture)
                        {
                            continue;
                        }

                        EnemyAnimationFrame frame;
                        frame.texture = texture;
                        frame.rect = sf::IntRect(
                            frameJson.value("x", 0),
                            frameJson.value("y", 0),
                            frameJson.value("width", 0),
                            frameJson.value("height", 0));
                        animation.frames.push_back(frame);
                    }
                }

                definition.animations[stateName] = animation;
            }
        }

        m_definitions[enemyId] = definition;
    }

    return !m_definitions.empty();
}

const EnemyDefinition* EnemyDatabase::GetDefinition(const std::string& id) const
{
    auto found = m_definitions.find(id);
    if(found == m_definitions.end())
    {
        return nullptr;
    }

    return &found->second;
}

bool EnemyDatabase::HasDefinition(const std::string& id) const
{
    return GetDefinition(id) != nullptr;
}

const sf::Texture* EnemyDatabase::LoadTexture(const std::string& texturePath)
{
    if(texturePath.empty())
    {
        return nullptr;
    }

    auto found = m_textures.find(texturePath);
    if(found != m_textures.end())
    {
        return found->second.get();
    }

    auto texture = std::make_unique<sf::Texture>();
    if(!texture->loadFromFile(texturePath))
    {
        std::cerr << "EnemyDatabase: Failed to load texture " << texturePath << std::endl;
        return nullptr;
    }

    texture->setSmooth(false);
    const sf::Texture* result = texture.get();
    m_textures[texturePath] = std::move(texture);
    return result;
}
