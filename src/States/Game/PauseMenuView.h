#pragma once

#include "../../Core/Data/CharacterProfile.h"
#include "../../UI/UIManager.h"
#include <SFML/Graphics.hpp>
#include <functional>
#include <memory>
#include <vector>

class PowerUpDataManager;
class PlayerProgressionManager;
class StatsPanel;
class TextureAtlas;
class UIButton;
class UIPanel;
class WeaponDataManager;

class PauseMenuView
{
public:
    PauseMenuView(
        TextureAtlas& atlas,
        const sf::Font& font,
        const sf::Font& boldFont,
        const CharacterProfile& character,
        const WeaponDataManager& weapons,
        const PlayerProgressionManager& progression,
        const PowerUpDataManager& powerUps);
    ~PauseMenuView();

    void SetOnResume(std::function<void()> callback);
    void SetOnQuit(std::function<void()> callback);
    void SetOnToggleHitboxes(std::function<void()> callback);

    void SetStageName(const std::string& stageName);
    void SetPlayerPosition(const sf::Vector2f& position, const sf::Vector2f& worldSize);
    void SetHitboxesVisible(bool visible);
    bool IsOptionsOpen() const;

    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void Update(float dt);
    void Draw(sf::RenderTarget& target);

private:
    UIButton* CreateButton(const std::string& label, const sf::Vector2f& position, const sf::Vector2f& size);
    void SetOptionsOpen(bool open);
    void SetFocusedButton(int index);
    void ActivateFocusedButton();
    void UpdateHitboxButtonLabel();

    TextureAtlas& m_atlas;
    const sf::Font& m_font;
    const sf::Font& m_boldFont;

    UIManager m_uiManager;
    std::unique_ptr<StatsPanel> m_statsPanel;
    std::unique_ptr<UIPanel> m_equipmentPanel;

    sf::RectangleShape m_dimmer;
    sf::RectangleShape m_mapBackground;
    sf::RectangleShape m_mapGrid;
    sf::CircleShape m_playerMarker;
    sf::Sprite m_pauseIcon;
    sf::Sprite m_characterSprite;
    sf::Sprite m_weaponSprite;
    std::vector<sf::Sprite> m_emptySlots;

    sf::Text m_title;
    sf::Text m_characterName;
    sf::Text m_equipmentTitle;
    sf::Text m_weaponName;
    sf::Text m_mapTitle;
    sf::Text m_stageName;
    sf::Text m_mapHint;
    sf::Text m_optionsTitle;
    sf::Text m_optionsDescription;

    UIButton* m_resumeButton = nullptr;
    UIButton* m_optionsButton = nullptr;
    UIButton* m_quitButton = nullptr;
    UIButton* m_hitboxButton = nullptr;
    UIButton* m_optionsBackButton = nullptr;
    std::vector<UIButton*> m_mainButtons;
    std::vector<UIButton*> m_optionsButtons;

    std::function<void()> m_onResume;
    std::function<void()> m_onQuit;
    std::function<void()> m_onToggleHitboxes;

    int m_focusedIndex = 0;
    bool m_optionsOpen = false;
    bool m_hitboxesVisible = false;
};
