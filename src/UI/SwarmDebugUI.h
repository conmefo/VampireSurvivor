#pragma once

#include "Panels/UIPanel.h"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <SFML/Graphics.hpp>

class GameState;

class SwarmDebugUI : public UIElement
{
public:
    SwarmDebugUI(TextureAtlas& atlas, const sf::Font& font, GameState& gameState);
    ~SwarmDebugUI() override = default;

    void Update(float deltaTime) override;
    void Draw(sf::RenderTarget& target) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void SetPosition(const sf::Vector2f& pos) override;

    void ToggleVisible() { m_visible = !m_visible; }
    bool IsVisible() const { return m_visible; }
    void SetVisible(bool visible) { m_visible = visible; }

private:
    struct ButtonEntry
    {
        sf::RectangleShape box;
        sf::Text label;
        std::function<void()> onClick;
        bool isHovered = false;
        bool isPressed = false;
        sf::Color baseColor;
        sf::Color hoverColor;
    };

    void AddButton(const std::string& label, const sf::Color& color, std::function<void()> onClick);
    void LayoutButtons();

    TextureAtlas& m_atlas;
    const sf::Font& m_font;
    GameState& m_gameState;
    bool m_visible = true;

    sf::RectangleShape m_background;
    sf::Text m_titleText;
    sf::Text m_shortcutHint;
    std::vector<ButtonEntry> m_buttons;
    sf::Vector2f m_position{20.0f, 170.0f};
};
