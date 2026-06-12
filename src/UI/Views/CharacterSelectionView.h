#pragma once

#include "../Core/UIElement.h"
#include <memory>
#include <functional>
#include <vector>
#include <SFML/Graphics/RenderTarget.hpp>
#include "../../Core/Resources/TextureAtlas.h"

// Forward declarations
class MainBoardPanel;
class StatsPanel;
class GoldDisplayWidget;
class UIButton;
class PlayerProgressionManager;
class CharacterDataManager;
class WeaponDataManager;
class PowerUpDataManager;
class UIPlayerCoopCard;
namespace sf { class Font; class Event; class RenderWindow; }

enum class SelectionState
{
    SelectingP1,
    SelectingP2,
    ReadyToStart
};

class CharacterSelectionView : public UIElement
{
private:
    std::unique_ptr<MainBoardPanel> m_mainBoard;
    std::unique_ptr<StatsPanel> m_statsPanel;
    std::unique_ptr<GoldDisplayWidget> m_goldDisplay;
    std::unique_ptr<UIButton> m_confirmButton;
    std::unique_ptr<UIButton> m_backButton;
    std::unique_ptr<UIButton> m_enterCoopButton;
    
    std::unique_ptr<UIPlayerCoopCard> m_p1Card;
    std::unique_ptr<UIPlayerCoopCard> m_p2Card;

    std::function<void()> m_onBackClicked;
    std::function<void(const std::vector<std::string>&)> m_onConfirmClicked;

    const sf::Font& m_font;

    std::string m_hoveredCharacterId;
    std::string m_p1CharacterId;
    std::string m_p2CharacterId;
    
    SelectionState m_selectionState = SelectionState::SelectingP1;
    bool m_isCoopMode = false;
    
    float m_shrinkLerpTime = 0.0f;
    bool m_isShrinking = false;

    // Cursors for the START button
    std::vector<AssetTextureData> m_cursorFrames;
    sf::Sprite m_leftCursor;
    sf::Sprite m_rightCursor;
    float m_cursorAnimTimer = 0.0f;
    int m_currentCursorFrame = 0;
    static constexpr float CURSOR_FRAME_DURATION = 0.1f;

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
    CharacterSelectionView(TextureAtlas& atlas, const sf::Font& font, const sf::Font* boldFont, const CharacterDataManager& characterData, const PlayerProgressionManager* progressionManager = nullptr, const WeaponDataManager* weaponManager = nullptr, const PowerUpDataManager* powerUpManager = nullptr);
    ~CharacterSelectionView() override;

    void InitializeLayout(const sf::Vector2f& viewportSize);

    void SetOnBackClicked(std::function<void()> callback);
    void SetOnConfirmClicked(std::function<void(const std::vector<std::string>&)> callback);

    void SetState(SelectionState state);
    void UpdateCursors(float deltaTime);

    void Update(float deltaTime) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void Draw(sf::RenderTarget& target) override;
};
