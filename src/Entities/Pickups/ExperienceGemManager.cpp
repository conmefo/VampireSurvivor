#include "ExperienceGemManager.h"
#include "../Player.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <random>

namespace
{
    float Length(const sf::Vector2f& value)
    {
        return std::sqrt(value.x * value.x + value.y * value.y);
    }

    sf::Vector2f Normalize(const sf::Vector2f& value)
    {
        const float length = Length(value);
        if(length <= 0.0001f)
        {
            return sf::Vector2f(0.0f, 0.0f);
        }

        return sf::Vector2f(value.x / length, value.y / length);
    }
}

void ExperienceGemManager::Initialize(TextureAtlas& atlas)
{
    m_blueTextureData = atlas.GetTextureData("GemBlue");
    m_greenTextureData = atlas.GetTextureData("GemGreen");
    m_redTextureData = atlas.GetTextureData("GemRed");
}

void ExperienceGemManager::Clear()
{
    m_gems.clear();
}

AssetTextureData ExperienceGemManager::GetTextureDataForValue(float value, GemType& outType) const
{
    if (value >= 20.0f)
    {
        outType = GemType::Red;
        return m_redTextureData.texture ? m_redTextureData : m_blueTextureData;
    }
    else if (value >= 5.0f)
    {
        outType = GemType::Green;
        return m_greenTextureData.texture ? m_greenTextureData : m_blueTextureData;
    }
    else
    {
        outType = GemType::Blue;
        return m_blueTextureData;
    }
}

void ExperienceGemManager::ApplyTextureDataToSprite(sf::Sprite& sprite, const AssetTextureData& data) const
{
    if (!data.texture) return;
    sprite.setTexture(*data.texture);
    sprite.setTextureRect(data.rect);
    sprite.setOrigin(
        static_cast<float>(data.rect.width) / 2.0f,
        static_cast<float>(data.rect.height) / 2.0f);
}

void ExperienceGemManager::Spawn(const sf::Vector2f& position, float value)
{
    if (value <= 0.0f)
    {
        return;
    }

    // 400-Gem Compression Cap Rule:
    // If on-screen gems reach the 400 limit, aggregate incoming XP into an existing Red Gem instead of spawning a new entity.
    if (m_gems.size() >= MaxOnScreenGems)
    {
        // Search for an existing Red Gem on the ground to accumulate XP into
        ExperienceGem* targetGem = nullptr;
        for (ExperienceGem& gem : m_gems)
        {
            if (gem.type == GemType::Red)
            {
                targetGem = &gem;
                break;
            }
        }

        // If no Red Gem exists yet, convert the first available gem to a Red Gem
        if (!targetGem && !m_gems.empty())
        {
            targetGem = &m_gems.front();
            targetGem->type = GemType::Red;
            ApplyTextureDataToSprite(targetGem->sprite, m_redTextureData.texture ? m_redTextureData : m_blueTextureData);
        }

        if (targetGem)
        {
            targetGem->value += value;
            return;
        }
    }

    // Normal spawn when below the 400-gem limit
    GemType gemType = GemType::Blue;
    AssetTextureData texData = GetTextureDataForValue(value, gemType);
    if (!texData.texture)
    {
        return;
    }

    ExperienceGem gem;
    gem.position = position;
    gem.value = value;
    gem.type = gemType;
    ApplyTextureDataToSprite(gem.sprite, texData);
    gem.sprite.setScale(GemScale, GemScale);
    gem.sprite.setPosition(position);

    m_gems.push_back(gem);
}

void ExperienceGemManager::SpawnMultiple(const sf::Vector2f& centerPosition, int count, float valuePerGem)
{
    float radius = 50.0f;
    for (int i = 0; i < count; ++i)
    {
        float angle = static_cast<float>(i) * (2.0f * 3.14159265f / static_cast<float>(count));
        sf::Vector2f offset(std::cos(angle) * radius, std::sin(angle) * radius);
        Spawn(centerPosition + offset, valuePerGem);
    }
}

void ExperienceGemManager::SpawnRandomInRadius(const sf::Vector2f& centerPosition, int count, float minRadius, float maxRadius)
{
    static std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159265f);
    std::uniform_real_distribution<float> radiusDist(minRadius, maxRadius);
    std::uniform_real_distribution<float> valDist(1.0f, 25.0f);

    for (int i = 0; i < count; ++i)
    {
        float angle = angleDist(rng);
        float radius = radiusDist(rng);
        float val = valDist(rng);
        sf::Vector2f offset(std::cos(angle) * radius, std::sin(angle) * radius);
        Spawn(centerPosition + offset, val);
    }
}

void ExperienceGemManager::Update(float dt, const std::vector<std::unique_ptr<Player>>& players)
{
    for(ExperienceGem& gem : m_gems)
    {
        // Find nearest alive player
        Player* nearestPlayer = nullptr;
        float nearestDistSq = std::numeric_limits<float>::max();
        for(const auto& p : players)
        {
            if(!p || p->IsDead()) continue;
            sf::Vector2f diff = p->GetCenterPosition() - gem.position;
            float distSq = diff.x * diff.x + diff.y * diff.y;
            if(distSq < nearestDistSq)
            {
                nearestDistSq = distSq;
                nearestPlayer = p.get();
            }
        }

        if(!nearestPlayer) continue;

        const sf::Vector2f playerCenter = nearestPlayer->GetCenterPosition();
        const float collectDistance = nearestPlayer->GetCollisionRadius() + CollectRadius;

        const float effectiveMagnetRadius = nearestPlayer->GetMagnetRadius() * m_magnetRadiusMultiplier;
        const float radiusRatio = std::max(1.0f, effectiveMagnetRadius / BaseMagnetRadius);
        const float outwardSpeed = m_baseOutwardSpeed * std::sqrt(radiusRatio);
        const float inwardAccel = m_baseInwardAccel * radiusRatio;

        const sf::Vector2f toPlayer = playerCenter - gem.position;
        const float distance = Length(toPlayer);

        if(distance <= collectDistance)
        {
            if(m_onGemCollected)
            {
                m_onGemCollected(gem.value);
            }
            gem.value = 0.0f;
            continue;
        }

        // Trigger attraction when gem enters magnet radius OR is already attracted
        if(distance <= effectiveMagnetRadius || gem.isBeingAttracted)
        {
            if (!gem.isBeingAttracted)
            {
                gem.isBeingAttracted = true;
                // Initial outward pop impulse away from player
                sf::Vector2f awayDir = Normalize(-toPlayer);
                // If awayDir is zero, default to up vector
                if (awayDir.x == 0.0f && awayDir.y == 0.0f)
                {
                    awayDir = sf::Vector2f(0.0f, -1.0f);
                }
                gem.velocity = awayDir * outwardSpeed;
            }

            // Continuous gravitational pull acceleration toward player
            sf::Vector2f pullDir = Normalize(toPlayer);
            gem.velocity += pullDir * inwardAccel * dt;

            // Cap max velocity so gems don't overshoot infinitely
            float currentSpeed = Length(gem.velocity);
            if (currentSpeed > m_maxMagnetSpeed)
            {
                gem.velocity = Normalize(gem.velocity) * m_maxMagnetSpeed;
            }

            gem.position += gem.velocity * dt;
            gem.sprite.setPosition(gem.position);
        }
    }

    m_gems.erase(
        std::remove_if(m_gems.begin(), m_gems.end(), [](const ExperienceGem& gem) {
            return gem.value <= 0.0f;
        }),
        m_gems.end());
}

void ExperienceGemManager::Draw(sf::RenderTarget& target) const
{
    for(const ExperienceGem& gem : m_gems)
    {
        target.draw(gem.sprite);
    }
}
