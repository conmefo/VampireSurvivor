#pragma once

#include "UIPanel.h"
#include <vector>
#include <string>
#include <SFML/Graphics/Text.hpp>

// Forward declarations
class TextureAtlas;
class CharacterProfile;
namespace sf { class Font; class RenderTarget; class Event; class RenderWindow; }

class StatsPanel : public UIPanel
{
private:
    struct StatRow
    {
        sf::Text labelText;
        sf::Text valueText;
        sf::Text buffText;
        std::string key;
    };

    std::vector<StatRow> m_rows;
    std::vector<std::string> m_statKeys;
    const sf::Font& m_font;

    static constexpr float ROW_PADDING = 27.0f;
    static constexpr unsigned int TEXT_SIZE = 24;
    static constexpr float START_Y_OFFSET = 20.0f;
    static constexpr float PADDING_X = 20.0f;

    void FormatStatText(StatRow& row, float value, float buff) const;
    std::string FormatLabelName(const std::string& key) const;

public:
    StatsPanel(TextureAtlas& atlas, const sf::Font& font);
    ~StatsPanel() override = default;

    void SetCharacterProfile(const CharacterProfile& profile, const class PlayerProgressionManager* progressionManager = nullptr, const class PowerUpDataManager* powerUpManager = nullptr);

    void Update(float deltaTime) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void Draw(sf::RenderTarget& target) override;
    void SetPosition(const sf::Vector2f& pos) override;
    void SetSize(const sf::Vector2f& size) override;
};
