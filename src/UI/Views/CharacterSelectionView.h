#pragma once

#include "../Core/UIElement.h"
#include <memory>
#include <SFML/Graphics/RenderTarget.hpp>

// Forward declarations
class TextureAtlas;
class MainBoardPanel;
class StatsPanel;
class GoldDisplayWidget;
class UIButton;
class PlayerProgressionManager;
namespace sf { class Font; class Event; class RenderWindow; }

class CharacterSelectionView : public UIElement
{
private:
    std::unique_ptr<MainBoardPanel> m_mainBoard;
    std::unique_ptr<StatsPanel> m_statsPanel;
    std::unique_ptr<GoldDisplayWidget> m_goldDisplay;
    std::unique_ptr<UIButton> m_confirmButton;

    static constexpr float BOARD_WIDTH = 700.0f;
    static constexpr float BOARD_HEIGHT = 800.0f;
    static constexpr float STATS_WIDTH = 300.0f;
    static constexpr float STATS_MARGIN_RIGHT = 30.0f;
    static constexpr float BUTTON_WIDTH = 200.0f;
    static constexpr float BUTTON_HEIGHT = 60.0f;
    static constexpr float BUTTON_MARGIN_BOTTOM = 20.0f;
    static constexpr float BUTTON_MARGIN_RIGHT = 20.0f;

public:
    CharacterSelectionView(TextureAtlas& atlas, const sf::Font& font, const PlayerProgressionManager* progressionManager);
    ~CharacterSelectionView() override;

    void InitializeLayout(const sf::Vector2f& viewportSize);

    void Update(float deltaTime) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void Draw(sf::RenderTarget& target) override;
};
