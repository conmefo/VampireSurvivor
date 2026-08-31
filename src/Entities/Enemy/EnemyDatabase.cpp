#include "EnemyDatabase.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

namespace
{
    constexpr float AuthenticColliderRadiusScale = 100.0f;

    float GetFloatOrDefault(const nlohmann::json& json, const char* key, float fallback)
    {
        return json.contains(key) ? json[key].get<float>() : fallback;
    }

    int GetIntOrDefault(const nlohmann::json& json, const char* key, int fallback)
    {
        return json.contains(key) ? static_cast<int>(json[key].get<float>()) : fallback;
    }

    std::string GetStringOrDefault(const nlohmann::json& json, const char* key, const std::string& fallback)
    {
        if(!json.contains(key) || !json[key].is_string())
        {
            return fallback;
        }

        return json[key].get<std::string>();
    }

    bool LoadOptionalJsonFile(const std::string& filepath, nlohmann::json& json)
    {
        std::ifstream file(filepath);
        if(!file.is_open())
        {
            return false;
        }

        try
        {
            file >> json;
        }
        catch(const nlohmann::json::parse_error&)
        {
            return false;
        }

        return true;
    }

    bool LoadAuthenticEnemyData(nlohmann::json& json)
    {
        return LoadOptionalJsonFile("assets/Data/ENEMY_DATA.json", json) ||
               LoadOptionalJsonFile("Assets/Data/ENEMY_DATA.json", json) ||
               LoadOptionalJsonFile("../assets/Data/ENEMY_DATA.json", json) ||
               LoadOptionalJsonFile("../Assets/Data/ENEMY_DATA.json", json) ||
               LoadOptionalJsonFile("../../assets/Data/ENEMY_DATA.json", json) ||
               LoadOptionalJsonFile("../../Assets/Data/ENEMY_DATA.json", json);
    }

    bool LoadAuthenticEnemyAtlas(const std::string& textureName, nlohmann::json& json)
    {
        if(textureName == "enemies")
        {
            return LoadOptionalJsonFile("assets/Data/enemies_atlas.json", json) ||
                   LoadOptionalJsonFile("Assets/Data/enemies_atlas.json", json);
        }
        if(textureName == "enemies3")
        {
            return LoadOptionalJsonFile("assets/Data/enemies3_atlas.json", json) ||
                   LoadOptionalJsonFile("Assets/Data/enemies3_atlas.json", json);
        }
        return false;
    }

    const nlohmann::json* FindAuthenticEnemyStats(const nlohmann::json& enemyData, const std::string& enemyId)
    {
        if(!enemyData.contains(enemyId) || !enemyData[enemyId].is_array() || enemyData[enemyId].empty())
        {
            return nullptr;
        }

        const nlohmann::json& firstEntry = enemyData[enemyId].front();
        return firstEntry.is_object() ? &firstEntry : nullptr;
    }

    std::string StripExtension(const std::string& filename)
    {
        const std::size_t dot = filename.find_last_of('.');
        if(dot == std::string::npos)
        {
            return filename;
        }

        return filename.substr(0, dot);
    }

    std::string GetFramePrefix(const std::string& firstFrameName)
    {
        const std::string frameKey = StripExtension(firstFrameName);
        const std::size_t underscore = frameKey.find_last_of('_');
        if(underscore == std::string::npos)
        {
            return frameKey;
        }

        return frameKey.substr(0, underscore);
    }

    std::string GetFirstDeathFrameName(const nlohmann::json& authenticStats)
    {
        if(!authenticStats.contains("frameNames") ||
           !authenticStats["frameNames"].is_array() ||
           authenticStats["frameNames"].empty() ||
           !authenticStats["frameNames"].front().is_string())
        {
            return "";
        }

        return authenticStats["frameNames"].front().get<std::string>();
    }

    std::string GetTexturePathForAuthenticSheet(const std::string& textureName)
    {
        if(textureName == "enemies")
        {
            return "assets/images/enemies/vs_enemies.png";
        }
        if(textureName == "enemies2")
        {
            return "assets/images/enemies/vs_enemies2.png";
        }
        if(textureName == "enemies3")
        {
            return "assets/images/enemies/vs_enemies3.png";
        }
        if(textureName == "vfx")
        {
            return "assets/images/enemies/vs_vfx.png";
        }

        return "";
    }

    void BuildAuthenticDeathAnimation(EnemyDefinition& definition,
                                      const nlohmann::json& authenticStats,
                                      const nlohmann::json& authenticAtlas,
                                      const sf::Texture* texture)
    {
        if(!texture)
        {
            return;
        }

        const int endFrame = GetIntOrDefault(authenticStats, "end", -1);
        if(endFrame < 0)
        {
            return;
        }

        const std::string framePrefix = GetFramePrefix(GetFirstDeathFrameName(authenticStats));
        if(framePrefix.empty())
        {
            return;
        }

        EnemyAnimationDefinition deathAnimation;
        deathAnimation.frameDuration = 1.0f / 60.0f;
        deathAnimation.isLooping = false;

        for(int frameIndex = 0; frameIndex <= endFrame; ++frameIndex)
        {
            const std::string frameKey = framePrefix + "_" + std::to_string(frameIndex);
            if(!authenticAtlas.contains(frameKey) || !authenticAtlas[frameKey].is_object())
            {
                continue;
            }

            const nlohmann::json& frameJson = authenticAtlas[frameKey];
            const int width = frameJson.value("width", 0);
            const int height = frameJson.value("height", 0);
            if(width <= 0 || height <= 0)
            {
                continue;
            }

            EnemyAnimationFrame frame;
            frame.texture = texture;
            const int y = frameJson.value("y", 0);
            frame.rect = sf::IntRect(
                frameJson.value("x", 0),
                static_cast<int>(texture->getSize().y) - y - height,
                width,
                height);
            deathAnimation.frames.push_back(frame);
        }

        if(!deathAnimation.frames.empty())
        {
            definition.deathAnimation = deathAnimation;
        }
    }

    void ApplyAuthenticScaleAndCollider(EnemyDefinition& definition,
                                        const nlohmann::json& authenticStats)
    {
        const float previousScale = std::max(0.001f, definition.spriteScale);
        const float authenticScale = GetFloatOrDefault(authenticStats, "scale", definition.spriteScale);
        if(authenticScale > 0.0f)
        {
            definition.spriteScale = authenticScale;
        }

        if(!authenticStats.contains("colliderOverride") ||
           !authenticStats["colliderOverride"].is_object())
        {
            if(authenticStats.contains("scale"))
            {
                definition.stats.collisionRadius *= definition.spriteScale / previousScale;
            }
            return;
        }

        const nlohmann::json& collider = authenticStats["colliderOverride"];
        definition.stats.collisionRadius =
            GetFloatOrDefault(collider, "radius", definition.stats.collisionRadius / AuthenticColliderRadiusScale) *
            AuthenticColliderRadiusScale *
            definition.spriteScale;

        definition.stats.collisionOffset = sf::Vector2f(
            GetFloatOrDefault(collider, "offsetX", 0.0f) * definition.spriteScale,
            GetFloatOrDefault(collider, "offsetY", 0.0f) * definition.spriteScale);
    }

    void BuildAuthenticIdleAnimation(EnemyDefinition& definition,
                                     const nlohmann::json& authenticStats,
                                     const nlohmann::json& authenticAtlas,
                                     const sf::Texture* texture)
    {
        if(!texture || !authenticStats.contains("frameNames") || !authenticStats["frameNames"].is_array())
        {
            return;
        }

        EnemyAnimationDefinition walkAnimation;
        walkAnimation.frameDuration = 0.1f;
        walkAnimation.isLooping = true;
        const int idleFrameCount = std::max(1, GetIntOrDefault(authenticStats, "idleFrameCount", 1));
        for(const nlohmann::json& frameNameJson : authenticStats["frameNames"])
        {
            if(!frameNameJson.is_string())
            {
                continue;
            }

            const std::string prefix = GetFramePrefix(frameNameJson.get<std::string>());
            for(int frameIndex = 0; frameIndex < idleFrameCount; ++frameIndex)
            {
                const std::string frameKey = prefix + "_" + std::to_string(frameIndex);
                if(!authenticAtlas.contains(frameKey) || !authenticAtlas[frameKey].is_object())
                {
                    continue;
                }

                const nlohmann::json& frameJson = authenticAtlas[frameKey];
                const int width = frameJson.value("width", 0);
                const int height = frameJson.value("height", 0);
                if(width <= 0 || height <= 0)
                {
                    continue;
                }

                walkAnimation.frames.push_back({
                    texture,
                    sf::IntRect(
                        frameJson.value("x", 0),
                        static_cast<int>(texture->getSize().y) - frameJson.value("y", 0) - height,
                        width,
                        height)});
            }
        }

        if(!walkAnimation.frames.empty())
        {
            definition.animations["WALK"] = std::move(walkAnimation);
        }
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
    nlohmann::json authenticEnemyData;
    const bool hasAuthenticEnemyData = LoadAuthenticEnemyData(authenticEnemyData);
    nlohmann::json authenticEnemyAtlas;
    const bool hasAuthenticEnemyAtlas = LoadAuthenticEnemyAtlas("enemies", authenticEnemyAtlas);
    nlohmann::json authenticEnemy3Atlas;
    const bool hasAuthenticEnemy3Atlas = LoadAuthenticEnemyAtlas("enemies3", authenticEnemy3Atlas);

    for(auto animationIt = animationsJson.begin(); animationIt != animationsJson.end(); ++animationIt)
    {
        const std::string enemyId = animationIt.key();
        const nlohmann::json& enemyAnimationJson = animationIt.value();

        EnemyDefinition definition;
        definition.id = enemyId;
        definition.name = enemyAnimationJson.value("name", enemyId);
        const nlohmann::json* authenticStats =
            hasAuthenticEnemyData ? FindAuthenticEnemyStats(authenticEnemyData, enemyId) : nullptr;

        if(statsJson.contains(enemyId))
        {
            const nlohmann::json& statJson = statsJson[enemyId];
            definition.stats.maxHealth = GetFloatOrDefault(statJson, "maxHp", definition.stats.maxHealth);
            definition.stats.speed = GetFloatOrDefault(statJson, "speed", definition.stats.speed);
            definition.stats.damage = GetFloatOrDefault(statJson, "power", definition.stats.damage);
            definition.stats.mass = GetFloatOrDefault(statJson, "mass", definition.stats.mass);
            definition.stats.collisionRadius = GetFloatOrDefault(statJson, "collisionRadius", definition.stats.collisionRadius);
            definition.stats.deathKnockback = GetFloatOrDefault(statJson, "deathKB", definition.stats.deathKnockback);
            definition.stats.baseAlpha = GetFloatOrDefault(statJson, "alpha", definition.stats.baseAlpha);
            definition.stats.expYield = GetFloatOrDefault(statJson, "xp", definition.stats.expYield);
            definition.stats.baseTint = GetIntOrDefault(statJson, "tint", definition.stats.baseTint);
            definition.spriteScale = GetFloatOrDefault(statJson, "spriteScale", definition.spriteScale);

            definition.stats.isRanged = statJson.value("isRanged", false);
            definition.stats.attackRange = GetFloatOrDefault(statJson, "attackRange", definition.stats.attackRange);
            definition.stats.attackCooldown = GetFloatOrDefault(statJson, "attackCooldown", definition.stats.attackCooldown);
            definition.stats.attackTelegraph = GetFloatOrDefault(statJson, "attackTelegraph", definition.stats.attackTelegraph);
            definition.stats.projectileSpeed = GetFloatOrDefault(statJson, "projectileSpeed", definition.stats.projectileSpeed);
            definition.stats.projectileDamage = GetFloatOrDefault(statJson, "projectileDamage", definition.stats.projectileDamage);
            definition.stats.projectileLifetime = GetFloatOrDefault(statJson, "projectileLifetime", definition.stats.projectileLifetime);
            definition.stats.projectileRadius = GetFloatOrDefault(statJson, "projectileRadius", definition.stats.projectileRadius);

            if(authenticStats)
            {
                definition.stats.maxHealth =
                    GetFloatOrDefault(*authenticStats, "maxHp", definition.stats.maxHealth);
                definition.stats.speed =
                    GetFloatOrDefault(*authenticStats, "speed", definition.stats.speed);
                definition.stats.damage =
                    GetFloatOrDefault(*authenticStats, "power", definition.stats.damage);
                definition.stats.mass =
                    GetFloatOrDefault(*authenticStats, "mass", definition.stats.mass);
                definition.stats.expYield =
                    GetFloatOrDefault(*authenticStats, "xp", definition.stats.expYield);
                ApplyAuthenticScaleAndCollider(definition, *authenticStats);

                definition.stats.deathKnockback =
                    GetFloatOrDefault(*authenticStats, "deathKB", definition.stats.deathKnockback);
                definition.stats.baseAlpha =
                    GetFloatOrDefault(*authenticStats, "alpha", definition.stats.baseAlpha);
                definition.stats.baseTint =
                    GetIntOrDefault(*authenticStats, "tint", definition.stats.baseTint);

                // Only enemies that have an authentic projectile type and fire delay
                // get ranged attacks. Ordinary enemies, including MUDMAN1 and MUMMY,
                // keep the game's default contact-damage behavior.
                const std::string bulletType =
                    GetStringOrDefault(*authenticStats, "bulletType", "");
                const float fireDelayMs =
                    GetFloatOrDefault(*authenticStats, "fireDelay", 0.0f);
                definition.stats.isRanged = !bulletType.empty() && fireDelayMs > 0.0f;
                definition.stats.attackRange = definition.stats.isRanged ? 360.0f : 0.0f;
                definition.stats.attackCooldown = definition.stats.isRanged
                    ? std::max(0.5f, fireDelayMs / 1000.0f)
                    : 0.0f;
                definition.stats.attackTelegraph = definition.stats.isRanged ? 0.45f : 0.65f;

                if(definition.stats.isRanged)
                {
                    const nlohmann::json* projectileStats =
                        FindAuthenticEnemyStats(authenticEnemyData, bulletType);
                    if(projectileStats)
                    {
                        definition.stats.projectileSpeed =
                            GetFloatOrDefault(*projectileStats, "speed", definition.stats.projectileSpeed);
                        definition.stats.projectileDamage =
                            GetFloatOrDefault(*projectileStats, "power", definition.stats.damage);
                    }
                    else
                    {
                        definition.stats.projectileDamage = definition.stats.damage;
                    }

                    definition.stats.projectileLifetime =
                        std::max(3.0f, definition.stats.attackRange / definition.stats.projectileSpeed + 1.0f);
                    definition.stats.projectileRadius = 7.0f;
                }
            }

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

        if(authenticStats && hasAuthenticEnemyAtlas)
        {
            const std::string textureName = GetStringOrDefault(*authenticStats, "textureName", "");
            const sf::Texture* texture = LoadTexture(GetTexturePathForAuthenticSheet(textureName));
            BuildAuthenticDeathAnimation(definition, *authenticStats, authenticEnemyAtlas, texture);
        }

        m_definitions[enemyId] = definition;
    }

    if(hasAuthenticEnemyData)
    {
        for(const auto& enemyEntry : authenticEnemyData.items())
        {
            if(m_definitions.find(enemyEntry.key()) != m_definitions.end())
            {
                continue;
            }

            const nlohmann::json* authenticStats = FindAuthenticEnemyStats(authenticEnemyData, enemyEntry.key());
            if(!authenticStats)
            {
                continue;
            }

            const std::string textureName = GetStringOrDefault(*authenticStats, "textureName", "");
            const nlohmann::json* authenticAtlas = nullptr;
            if(textureName == "enemies" && hasAuthenticEnemyAtlas)
            {
                authenticAtlas = &authenticEnemyAtlas;
            }
            else if(textureName == "enemies3" && hasAuthenticEnemy3Atlas)
            {
                authenticAtlas = &authenticEnemy3Atlas;
            }

            const sf::Texture* texture = LoadTexture(GetTexturePathForAuthenticSheet(textureName));
            if(!authenticAtlas || !texture)
            {
                continue;
            }

            EnemyDefinition definition;
            definition.id = enemyEntry.key();
            definition.name = GetStringOrDefault(*authenticStats, "name", definition.id);
            definition.stats.maxHealth = GetFloatOrDefault(*authenticStats, "maxHp", definition.stats.maxHealth);
            definition.stats.speed = GetFloatOrDefault(*authenticStats, "speed", definition.stats.speed);
            definition.stats.damage = GetFloatOrDefault(*authenticStats, "power", definition.stats.damage);
            definition.stats.mass = GetFloatOrDefault(*authenticStats, "mass", definition.stats.mass);
            definition.stats.expYield = GetFloatOrDefault(*authenticStats, "xp", definition.stats.expYield);
            definition.stats.deathKnockback = GetFloatOrDefault(*authenticStats, "deathKB", definition.stats.deathKnockback);
            definition.stats.baseAlpha = GetFloatOrDefault(*authenticStats, "alpha", definition.stats.baseAlpha);
            definition.stats.baseTint = GetIntOrDefault(*authenticStats, "tint", definition.stats.baseTint);
            ApplyAuthenticScaleAndCollider(definition, *authenticStats);
            BuildAuthenticIdleAnimation(definition, *authenticStats, *authenticAtlas, texture);
            BuildAuthenticDeathAnimation(definition, *authenticStats, *authenticAtlas, texture);

            if(!definition.animations.empty())
            {
                m_definitions[definition.id] = std::move(definition);
            }
        }
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
