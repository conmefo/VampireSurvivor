#pragma once

#include "../Panels/UIPanel.h"
#include "../../Core/WindowSettings.h"
#include <SFML/Graphics/Sprite.hpp>

// Forward declarations
class TextureAtlas;
class PlayerProgressionManager;
namespace sf { class Font; class RenderTarget; }

class GoldDisplayWidget : public UIPanel
{
private:
    sf::Sprite m_coinIcon;
    const PlayerProgressionManager* m_progressionManager;
    const sf::Font& m_font;
    int m_lastKnownGold;

public:
    // Pixel-perfect alignment coordinates matching MainMenuState
    static constexpr float DEFAULT_X = Core::VIRTUAL_WIDTH / 2.0f - 129.5f; // 670.5f
    static constexpr float DEFAULT_Y = 22.0f;
    static constexpr float DEFAULT_WIDTH = 259.0f;
    static constexpr float DEFAULT_HEIGHT = 70.0f;

    GoldDisplayWidget(TextureAtlas& atlas, const PlayerProgressionManager* progressionManager, const sf::Font& font);
    ~GoldDisplayWidget() override = default;

    void SetPosition(const sf::Vector2f& pos) override;
    void Update(float deltaTime) override;
    void Draw(sf::RenderTarget& target) override;
};
