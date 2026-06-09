#pragma once
#include "../Core/UIElement.h"
#include "../../Core/Resources/TextureAtlas.h"
#include <SFML/Graphics.hpp>

class UIScrollbar : public UIElement
{
public:
    UIScrollbar(TextureAtlas& atlas);
    ~UIScrollbar() override = default;

    void UpdateLayout(float viewportHeight, float contentHeight);
    
    void SetScrollRatio(float ratio);
    float GetScrollRatio() const { return m_scrollRatio; }
    bool IsDragging() const { return m_isDragging; }

    void Update(float deltaTime) override;
    void Draw(sf::RenderTarget& target) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;

private:
    void Draw3Slice(sf::RenderTarget& target, sf::Sprite& sprite, const sf::IntRect& top, const sf::IntRect& mid, const sf::IntRect& bot, float x, float y, float targetHeight);

    TextureAtlas& m_atlas;
    
    sf::Sprite m_trackSprite;
    sf::Sprite m_thumbSprite;

    sf::IntRect m_trackTop;
    sf::IntRect m_trackMid;
    sf::IntRect m_trackBot;

    sf::IntRect m_thumbTop;
    sf::IntRect m_thumbMid;
    sf::IntRect m_thumbBot;

    bool m_isDragging = false;
    float m_dragOffsetY = 0.0f;
    float m_scrollRatio = 0.0f;

    float m_trackHeight = 0.0f;
    float m_thumbHeight = 0.0f;
    float m_viewportHeight = 0.0f;
    float m_contentHeight = 0.0f;
};
