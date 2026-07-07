#pragma once

#include <SFML/Graphics.hpp>
#include "../Entities/Player.h"

class PlayerHUD
{
public:
    PlayerHUD(const Player& player);
    ~PlayerHUD() = default;

    void Update(float dt);
    void Draw(sf::RenderTarget& target) const;

private:
    const Player& m_player;

    sf::RectangleShape m_bgRect;
    sf::RectangleShape m_fillRect;

    static constexpr float BAR_WIDTH = 40.0f;
    static constexpr float BAR_HEIGHT = 6.0f;
    static constexpr float Y_OFFSET = 30.0f; // Distance below the player
};
