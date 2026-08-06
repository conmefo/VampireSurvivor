#pragma once

#include "../../Core/Resources/TextureAtlas.h"
#include "../Player.h"

#include <SFML/Graphics.hpp>
#include <vector>

class ExperienceGemManager
{
public:
    void Initialize(TextureAtlas& atlas);
    void Clear();
    void Spawn(const sf::Vector2f& position, float value);
    void SpawnMultiple(const sf::Vector2f& centerPosition, int count, float valuePerGem);
    void SpawnRandomInRadius(const sf::Vector2f& centerPosition, int count, float minRadius, float maxRadius);
    std::size_t GetActiveGemCount() const { return m_gems.size(); }
    void Update(float dt, Player& player);
    void Draw(sf::RenderTarget& target) const;

    // Tuning getters & setters for rubber band magnet physics
    float GetBaseOutwardSpeed() const { return m_baseOutwardSpeed; }
    void SetBaseOutwardSpeed(float val) { m_baseOutwardSpeed = val; }

    float GetBaseInwardAccel() const { return m_baseInwardAccel; }
    void SetBaseInwardAccel(float val) { m_baseInwardAccel = val; }

    float GetMaxMagnetSpeed() const { return m_maxMagnetSpeed; }
    void SetMaxMagnetSpeed(float val) { m_maxMagnetSpeed = val; }

    float GetMagnetRadiusMultiplier() const { return m_magnetRadiusMultiplier; }
    void SetMagnetRadiusMultiplier(float val) { m_magnetRadiusMultiplier = val; }

private:
    enum class GemType
    {
        Blue,
        Green,
        Red
    };

    struct ExperienceGem
    {
        sf::Sprite sprite;
        sf::Vector2f position;
        sf::Vector2f velocity{0.0f, 0.0f};
        float value = 0.0f;
        GemType type = GemType::Blue;
        bool isBeingAttracted = false;
    };

    AssetTextureData GetTextureDataForValue(float value, GemType& outType) const;
    void ApplyTextureDataToSprite(sf::Sprite& sprite, const AssetTextureData& data) const;

    static constexpr float GemScale = 1.0f;
    static constexpr float CollectRadius = 24.0f;
    static constexpr float BaseMagnetRadius = 96.0f;
    static constexpr std::size_t MaxOnScreenGems = 400;

    float m_baseOutwardSpeed = 255.0f;
    float m_baseInwardAccel = 1120.0f;
    float m_maxMagnetSpeed = 900.0f;
    float m_magnetRadiusMultiplier = 0.15f;

    AssetTextureData m_blueTextureData{nullptr, sf::IntRect()};
    AssetTextureData m_greenTextureData{nullptr, sf::IntRect()};
    AssetTextureData m_redTextureData{nullptr, sf::IntRect()};
    std::vector<ExperienceGem> m_gems;
};
