#pragma once

#include "../Core/UIElement.h"
#include "../../Core/Resources/TextureAtlas.h"
#include "../../Core/Data/PowerUpData.h"
#include "NineSliceComponent.h"
#include "../Elements/UIButton.h"
#include <SFML/Graphics.hpp>
#include <memory>

class UIDetailPanel : public UIElement
{
public:
    UIDetailPanel(TextureAtlas& atlas, const sf::Font& font);
    void UpdateContent(const PowerUpData& data);

    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void Update(float deltaTime) override;
    void Draw(sf::RenderTarget& target) override;

    void SetPosition(const sf::Vector2f& pos) override;
    void SetSize(const sf::Vector2f& size) override;
    void SetCornerScale(float scale);

private:
    TextureAtlas& m_atlas;
    const sf::Font& m_font;

    NineSliceComponent m_background;
    sf::Text m_titleText;
    sf::Text m_descText;
    sf::Text m_priceText;
    std::unique_ptr<UIButton> m_buyButton;
    sf::Sprite m_coinIcon;

    sf::Sprite m_innerFrameSprite;
    sf::Sprite m_iconSprite;
    sf::Vector2f m_innerFrameBaseScale;
    sf::Vector2f m_iconBaseScale;

    void UpdateLayout();
};
