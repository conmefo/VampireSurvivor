#pragma once

#include "../Panels/UIPanel.h"

#include <SFML/Graphics/Sprite.hpp>

class TextureAtlas;

class RunGoldDisplay : public UIPanel
{
public:
    RunGoldDisplay(TextureAtlas& atlas, const sf::Font& font);

    void SetGold(int gold);
    void SetPosition(const sf::Vector2f& position) override;
    void Draw(sf::RenderTarget& target) override;

private:
    sf::Sprite m_coinIcon;
    const sf::Font& m_font;
    int m_gold = -1;
};
