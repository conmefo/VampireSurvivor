#pragma once

#include "../Components/NineSliceComponent.h"
#include <SFML/Graphics.hpp>

class Player;
class TextureAtlas;

class ExpBar
{
public:
    ExpBar(const TextureAtlas& atlas, const sf::Font& font);
    ~ExpBar() = default;

    void SetPositionAndSize(const sf::Vector2f& position, const sf::Vector2f& size);
    void Update(float dt, const Player* player);
    void Draw(sf::RenderTarget& target) const;

private:
    NineSliceComponent m_background;
    sf::RectangleShape m_fillRect;
    sf::Text m_levelText;
    int m_cachedLevel = -1;
    float m_progressRatio = 0.0f;
    sf::Vector2f m_position{96.0f, 0.0f};
    sf::Vector2f m_size{1728.0f, 36.0f};
};
