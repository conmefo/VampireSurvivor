#pragma once

#include "UIPanel.h"
#include <vector>
#include <memory>
#include <string>
#include <functional>

#include "../Elements/CharacterCardWidget.h"

// Forward declarations
class TextureAtlas;
class CharacterDataManager;
class PlayerProgressionManager;
namespace sf { class Font; class RenderTarget; class Event; class RenderWindow; }

class RosterGridPanel : public UIPanel
{
private:
    std::vector<std::unique_ptr<CharacterCardWidget>> m_cards;
    std::string m_selectedCharacterId;
    std::function<void(const std::string&)> m_onSelectionChanged;
    
    TextureAtlas& m_atlas;
    const sf::Font& m_font;

    static constexpr int MAX_COLUMNS = 4;
    static constexpr float CARD_SPACING_X = 15.0f;
    static constexpr float CARD_SPACING_Y = 15.0f;
    static constexpr float GRID_START_OFFSET_X = 20.0f;
    static constexpr float GRID_START_OFFSET_Y = 20.0f;

    void OnCardClicked(const std::string& characterId);

public:
    RosterGridPanel(TextureAtlas& atlas, const sf::Font& font);
    ~RosterGridPanel() override = default;

    void InitializeRoster(const CharacterDataManager& dataManager, const PlayerProgressionManager& progManager);
    void SetOnSelectionChanged(std::function<void(const std::string&)> callback);

    void Update(float deltaTime) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void Draw(sf::RenderTarget& target) override;

    void SetPosition(const sf::Vector2f& pos) override;
    void SetSize(const sf::Vector2f& size) override;
};
