#pragma once

#include "../Core/UIElement.h"
#include "../../Core/Resources/TextureAtlas.h"
#include "../../Core/Data/PowerUpData.h"
#include "NineSliceComponent.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class PowerUpCard : public UIElement
{
public:
    PowerUpCard(TextureAtlas& atlas, const sf::Font& font);
    
    void InitDummyData();
    void InjectData(const PowerUpData& data);

    void Update(float deltaTime) override;
    void Draw(sf::RenderTarget& target) override;

    void SetPosition(const sf::Vector2f& pos) override;
    void SetScale(float scale);
    void SetBgColor(const sf::Color& color);

private:
    void UpdateLayout();

    TextureAtlas& m_atlas;
    const sf::Font& m_font;

    NineSliceComponent m_bgNineSlice;
    sf::Sprite m_innerFrameSprite;
    sf::Sprite m_iconSprite;
    sf::Text m_titleText;

    // Flyweight checklist
    sf::Sprite m_checkboxSprite;
    sf::IntRect m_emptyBoxRect;
    sf::IntRect m_filledBoxRect;

    int m_maxLevel = 0;
    int m_currentLevel = 0;

    float m_scale = 1.0f;

    sf::Vector2f m_innerFrameBaseScale{1.f, 1.f};
    sf::Vector2f m_iconBaseScale{1.f, 1.f};
    sf::Vector2f m_checkboxBaseScale{1.f, 1.f};
};
