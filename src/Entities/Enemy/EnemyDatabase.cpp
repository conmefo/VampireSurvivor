#include "EnemyDatabase.h"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

namespace
{
    constexpr int MaxAuthenticDeathFrames = 30;
    constexpr float AuthenticColliderRadiusScale = 100.0f;
    constexpr float AuthenticSpeedScale = 0.21f;
    constexpr float AuthenticHealthScale = 10.0f;

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
        if(textureName == "enemies") return "assets/images/enemies/enemies.png";
        if(textureName == "enemies2") return "assets/images/enemies/enemies2.png";
        if(textureName == "enemies3") return "assets/images/enemies/enemies3.png";
        if(textureName == "enemies2023") return "assets/images/enemies/enemies2023.png";
        if(textureName == "enemies2025") return "assets/images/enemies/enemies2025.png";
        if(textureName == "enemiesM") return "assets/images/enemies/enemiesM.png";
        if(textureName == "vfx") return "assets/images/enemies/vs_vfx.png";

        return "assets/images/enemies/enemies.png";
    }

    std::string GetAtlasPathForAuthenticSheet(const std::string& textureName)
    {
        if(textureName == "enemies") return "assets/Data/enemies_atlas.json";
        if(textureName == "enemies2") return "assets/Data/enemies2_atlas.json";
        if(textureName == "enemies3") return "assets/Data/enemies3_atlas.json";
        if(textureName == "enemies2023") return "assets/Data/enemies2023_atlas.json";
        if(textureName == "enemies2025") return "assets/Data/enemies2025_atlas.json";
        if(textureName == "enemiesM") return "assets/Data/enemiesM_atlas.json";
        if(textureName == "vfx") return "assets/Data/vfx_atlas.json";

        return "assets/Data/enemies_atlas.json";
    }

    bool IsPrototypeBoss(const std::string& enemyId, const EnemyStats& stats)
    {
        return enemyId.find("BOSS") != std::string::npos ||
               enemyId.rfind("MASK_", 0) == 0 ||
               stats.maxHealth >= 100.0f ||
               stats.damage >= 30.0f;
    }

    void ApplyAuthenticScaleAndCollider(EnemyDefinition& definition,
                                        const nlohmann::json& authenticStats)
    {
        const float previousScale = std::max(0.001f, definition.spriteScale);
        definition.spriteScale = std::clamp(
            GetFloatOrDefault(authenticStats, "scale", definition.spriteScale),
            0.25f,
            3.0f);

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
            std::clamp(GetFloatOrDefault(collider, "offsetX", 0.0f) * definition.spriteScale, -80.0f, 80.0f),
            std::clamp(GetFloatOrDefault(collider, "offsetY", 0.0f) * definition.spriteScale, -80.0f, 80.0f));
    }

    void ApplyPrototypeBalance(const std::string& enemyId, EnemyStats& stats)
    {
        const bool bossLike = IsPrototypeBoss(enemyId, stats);
        const float maxHealthCap = bossLike ? 1500.0f : 800.0f;
        const float damageCap = bossLike ? 25.0f : 20.0f;
        const float speedCap = bossLike ? 220.0f : 240.0f;
        const float collisionRadiusCap = bossLike ? 48.0f : 34.0f;

        stats.maxHealth = std::clamp(stats.maxHealth, 0.1f, maxHealthCap);
        stats.damage = std::clamp(stats.damage, 0.0f, damageCap);
        stats.speed = std::clamp(stats.speed, 0.0f, speedCap);
        stats.mass = std::max(stats.mass, 0.5f);
        stats.collisionRadius = std::clamp(stats.collisionRadius, 8.0f, collisionRadiusCap);
        stats.deathKnockback = std::clamp(stats.deathKnockback, 0.0f, 8.0f);
        stats.baseAlpha = std::clamp(stats.baseAlpha, 0.0f, 1.0f);
        stats.expYield = std::max(stats.expYield, 0.0f);
    }

    void BuildAuthenticIdleAnimation(EnemyDefinition& definition,
                                     const nlohmann::json& authenticStats,
                                     const nlohmann::json& authenticAtlas,
                                     const sf::Texture* texture)
    {
        if(!texture)
        {
            return;
        }

        const std::string firstFrameName = GetFirstDeathFrameName(authenticStats);
        const std::string framePrefix = GetFramePrefix(firstFrameName);
        const int idleCount = GetIntOrDefault(authenticStats, "idleFrameCount", 1);

        EnemyAnimationDefinition idleAnim;
        idleAnim.frameDuration = 0.16f;
        idleAnim.isLooping = true;

        for(int i = 1; i <= idleCount; ++i)
        {
            char buf[32];
            std::snprintf(buf, sizeof(buf), "_i%02d", i);
            const std::string key = framePrefix + buf;
            auto it = authenticAtlas.find(key);
            if(it != authenticAtlas.end() && it->is_object())
            {
                const auto& fj = *it;
                const int w = fj.value("width", 0);
                const int h = fj.value("height", 0);
                if(w > 0 && h > 0)
                {
                    EnemyAnimationFrame f;
                    f.texture = texture;
                    const int y = fj.value("y", 0);
                    f.rect = sf::IntRect(fj.value("x", 0), static_cast<int>(texture->getSize().y) - y - h, w, h);
                    idleAnim.frames.push_back(f);
                }
            }
            else
            {
                const std::string altKey = framePrefix + "_" + std::to_string(i - 1);
                auto altIt = authenticAtlas.find(altKey);
                if(altIt != authenticAtlas.end() && altIt->is_object())
                {
                    const auto& fj = *altIt;
                    const int w = fj.value("width", 0);
                    const int h = fj.value("height", 0);
                    if(w > 0 && h > 0)
                    {
                        EnemyAnimationFrame f;
                        f.texture = texture;
                        const int y = fj.value("y", 0);
                        f.rect = sf::IntRect(fj.value("x", 0), static_cast<int>(texture->getSize().y) - y - h, w, h);
                        idleAnim.frames.push_back(f);
                    }
                }
            }
        }

        if(idleAnim.frames.empty() && !firstFrameName.empty())
        {
            const std::string rawKey = StripExtension(firstFrameName);
            auto rawIt = authenticAtlas.find(rawKey);
            if(rawIt != authenticAtlas.end() && rawIt->is_object())
            {
                const auto& fj = *rawIt;
                const int w = fj.value("width", 0);
                const int h = fj.value("height", 0);
                if(w > 0 && h > 0)
                {
                    EnemyAnimationFrame f;
                    f.texture = texture;
                    const int y = fj.value("y", 0);
                    f.rect = sf::IntRect(fj.value("x", 0), static_cast<int>(texture->getSize().y) - y - h, w, h);
                    idleAnim.frames.push_back(f);
                }
            }
        }

        if(!idleAnim.frames.empty())
        {
            definition.animations["idle"] = idleAnim;
            definition.animations["WALK"] = idleAnim;
        }
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
        deathAnimation.frameDuration = 0.04f;
        deathAnimation.isLooping = false;

        const int lastFrame = std::min(endFrame, MaxAuthenticDeathFrames - 1);
        for(int frameIndex = 0; frameIndex <= lastFrame; ++frameIndex)
        {
            const std::string frameKey = framePrefix + "_" + std::to_string(frameIndex);
            auto it = authenticAtlas.find(frameKey);
            if(it == authenticAtlas.end() || !it->is_object())
            {
                continue;
            }

            const nlohmann::json& frameJson = *it;
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
}

bool EnemyDatabase::LoadFromFile(const std::string& filepath)
{
    nlohmann::json authenticEnemyData;
    bool loaded = LoadOptionalJsonFile(filepath, authenticEnemyData);
    if(!loaded || !authenticEnemyData.is_object() || authenticEnemyData.empty())
    {
        loaded = LoadAuthenticEnemyData(authenticEnemyData);
    }

    if(!loaded || !authenticEnemyData.is_object() || authenticEnemyData.empty())
    {
        std::cerr << "EnemyDatabase: Failed to load authentic ENEMY_DATA.json from " << filepath << std::endl;
        return false;
    }

    m_definitions.clear();

    std::unordered_map<std::string, nlohmann::json> atlases;

    for(auto it = authenticEnemyData.begin(); it != authenticEnemyData.end(); ++it)
    {
        const std::string enemyId = it.key();
        const nlohmann::json& entries = it.value();
        if(!entries.is_array() || entries.empty() || !entries.front().is_object())
        {
            continue;
        }

        const nlohmann::json& statJson = entries.front();
        EnemyDefinition definition;
        definition.id = enemyId;
        definition.name = GetStringOrDefault(statJson, "bName", enemyId);

        definition.stats.maxHealth = GetFloatOrDefault(statJson, "maxHp", 10.0f) * AuthenticHealthScale;
        definition.stats.speed = GetFloatOrDefault(statJson, "speed", 100.0f) * AuthenticSpeedScale;
        definition.stats.damage = GetFloatOrDefault(statJson, "power", 5.0f);
        definition.stats.mass = GetFloatOrDefault(statJson, "mass", 1.0f);
        definition.stats.collisionRadius = GetFloatOrDefault(statJson, "collisionRadius", 14.0f);
        definition.stats.knockback = GetFloatOrDefault(statJson, "knockback", 1.0f);
        definition.stats.maxKnockback = GetFloatOrDefault(statJson, "maxKnockback", 3.0f);
        definition.stats.deathKnockback = GetFloatOrDefault(statJson, "deathKB", 1.0f);
        definition.stats.baseAlpha = GetFloatOrDefault(statJson, "alpha", 1.0f);
        definition.stats.expYield = GetFloatOrDefault(statJson, "xp", 1.0f);
        definition.stats.baseTint = GetIntOrDefault(statJson, "tint", 0xFFFFFF);
        definition.spriteScale = GetFloatOrDefault(statJson, "scale", 1.0f);

        ApplyAuthenticScaleAndCollider(definition, statJson);

        const std::string bulletType = GetStringOrDefault(statJson, "bulletType", "");
        const float fireDelayMs = GetFloatOrDefault(statJson, "fireDelay", 0.0f);
        definition.stats.isRanged = !bulletType.empty() && fireDelayMs > 0.0f;
        definition.stats.attackRange = definition.stats.isRanged ? 360.0f : 0.0f;
        definition.stats.attackCooldown = definition.stats.isRanged
            ? std::max(0.5f, fireDelayMs / 1000.0f)
            : 0.0f;
        definition.stats.attackTelegraph = definition.stats.isRanged ? 0.45f : 0.65f;

        if(definition.stats.isRanged)
        {
            auto bulletIt = authenticEnemyData.find(bulletType);
            if(bulletIt != authenticEnemyData.end() &&
               bulletIt->is_array() &&
               !bulletIt->empty())
            {
                const auto& projStat = bulletIt->front();
                definition.stats.projectileSpeed = std::max(60.0f, GetFloatOrDefault(projStat, "speed", 120.0f));
                definition.stats.projectileDamage = std::max(1.0f, GetFloatOrDefault(projStat, "power", definition.stats.damage));
            }
            else
            {
                definition.stats.projectileSpeed = 120.0f;
                definition.stats.projectileDamage = definition.stats.damage;
            }

            definition.stats.projectileLifetime =
                std::max(3.0f, definition.stats.attackRange / definition.stats.projectileSpeed + 1.0f);
            definition.stats.projectileRadius = 7.0f;
        }

        ApplyPrototypeBalance(enemyId, definition.stats);

        const std::string textureName = GetStringOrDefault(statJson, "textureName", "enemies");
        const std::string texturePath = GetTexturePathForAuthenticSheet(textureName);
        const sf::Texture* texture = LoadTexture(texturePath);

        if(atlases.find(textureName) == atlases.end())
        {
            nlohmann::json atlasJson;
            LoadOptionalJsonFile(GetAtlasPathForAuthenticSheet(textureName), atlasJson);
            atlases[textureName] = std::move(atlasJson);
        }

        const nlohmann::json& atlasJson = atlases[textureName];

        BuildAuthenticIdleAnimation(definition, statJson, atlasJson, texture);
        BuildAuthenticDeathAnimation(definition, statJson, atlasJson, texture);

        m_definitions[enemyId] = std::move(definition);
    }

    return !m_definitions.empty();
}

const EnemyDefinition* EnemyDatabase::GetDefinition(const std::string& id) const
{
    auto it = m_definitions.find(id);
    if(it != m_definitions.end())
    {
        return &it->second;
    }
    return nullptr;
}

bool EnemyDatabase::HasDefinition(const std::string& id) const
{
    return m_definitions.find(id) != m_definitions.end();
}

const sf::Texture* EnemyDatabase::LoadTexture(const std::string& texturePath)
{
    if(texturePath.empty())
    {
        return nullptr;
    }

    auto it = m_textures.find(texturePath);
    if(it != m_textures.end())
    {
        return it->second.get();
    }

    auto texture = std::make_unique<sf::Texture>();
    if(!texture->loadFromFile(texturePath))
    {
        if(texturePath.find("vs_") == std::string::npos)
        {
            std::string fallback = texturePath;
            auto pos = fallback.find("enemies/");
            if(pos != std::string::npos)
            {
                fallback.insert(pos + 8, "vs_");
                if(texture->loadFromFile(fallback))
                {
                    const sf::Texture* ptr = texture.get();
                    m_textures[texturePath] = std::move(texture);
                    return ptr;
                }
            }
        }

        std::cerr << "EnemyDatabase: Failed to load texture " << texturePath << std::endl;
        return nullptr;
    }

    const sf::Texture* ptr = texture.get();
    m_textures[texturePath] = std::move(texture);
    return ptr;
}
