#pragma once

#include "../Core/Resources/TextureAtlas.h"
#include "../Core/Data/WeaponDataManager.h"
#include "../Entities/Player.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>

// Easy-to-modify layout configuration parameters
struct WeaponSlotsHUDConfig
{
    sf::Vector2f position{110.0f, 42.0f}; // Top-left position on screen (UI coordinates)
    float scale = 2.4f;                   // Overall HUD scale factor
    float cellWidth = 16.0f;              // Width of each cell in frame local space (96 / 6 = 16)
    float cellHeight = 16.0f;             // Height of each cell in frame local space (32 / 2 = 16)
    float iconPadding = 1.0f;             // Inset padding for item icons inside cells
    float lockedIconScale = 0.95f;        // Scale multiplier for no16 locked slot icon
};

class WeaponSlotsHUD
{
public:
    WeaponSlotsHUD() = default;
    ~WeaponSlotsHUD() = default;

    void Initialize(const TextureAtlas& atlas);
    
    void SetConfig(const WeaponSlotsHUDConfig& config) { m_config = config; }
    const WeaponSlotsHUDConfig& GetConfig() const { return m_config; }
    WeaponSlotsHUDConfig& GetConfig() { return m_config; }

    void SetPosition(const sf::Vector2f& pos) { m_config.position = pos; }
    void SetScale(float scale) { m_config.scale = scale; }

    void Draw(
        sf::RenderTarget& target,
        const std::vector<std::unique_ptr<Player>>& players,
        const WeaponDataManager& weaponData,
        int maxSlots = 6) const;

private:
    void DrawSlotRow(
        sf::RenderTarget& target,
        const std::vector<std::string>& itemIconKeys,
        int maxSlots,
        int rowIndex,
        const sf::Vector2f& basePos,
        float scale) const;

    AssetTextureData GetIconData(const std::string& key) const;

    const TextureAtlas* m_atlas = nullptr;
    AssetTextureData m_frameData;
    AssetTextureData m_lockedData;
    WeaponSlotsHUDConfig m_config;
};
