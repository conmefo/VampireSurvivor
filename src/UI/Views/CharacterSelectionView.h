#pragma once

#include "../Core/UIElement.h"
#include <memory>
#include <functional>
#include <SFML/Graphics/RenderTarget.hpp>

// Forward declarations
class TextureAtlas;
class MainBoardPanel;
class StatsPanel;
class GoldDisplayWidget;
class UIButton;
class PlayerProgressionManager;
class CharacterDataManager;
namespace sf { class Font; class Event; class RenderWindow; }

class CharacterSelectionView : public UIElement
{
private:
    std::unique_ptr<MainBoardPanel> m_mainBoard;
    std::unique_ptr<StatsPanel> m_statsPanel;
    std::unique_ptr<GoldDisplayWidget> m_goldDisplay;
    std::unique_ptr<UIButton> m_confirmButton;
    std::unique_ptr<UIButton> m_backButton;

    std::function<void()> m_onBackClicked;
    std::function<void(const std::string&)> m_onConfirmClicked;

    std::string m_selectedCharacterId;

    static constexpr float BOARD_WIDTH = 805.0f;
    static constexpr float BOARD_HEIGHT = 980.0f;
    static constexpr float STATS_WIDTH = 320.0f;
    static constexpr float STATS_HEIGHT = 640.0f;
    static constexpr float STATS_MARGIN_RIGHT = 80.0f;
    static constexpr float BUTTON_WIDTH = 200.0f;
    static constexpr float BUTTON_HEIGHT = 60.0f;
    static constexpr float BUTTON_MARGIN_BOTTOM = 20.0f;
    static constexpr float BUTTON_MARGIN_RIGHT = 20.0f;

public:
    CharacterSelectionView(TextureAtlas& atlas, const sf::Font& font, const CharacterDataManager& characterData, const PlayerProgressionManager* progressionManager);
    ~CharacterSelectionView() override;

    void InitializeLayout(const sf::Vector2f& viewportSize);

    void SetOnBackClicked(std::function<void()> callback);
    void SetOnConfirmClicked(std::function<void(const std::string&)> callback);

    void Update(float deltaTime) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void Draw(sf::RenderTarget& target) override;
};
