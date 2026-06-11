#pragma once
#include "../Core/UIElement.h"
#include "../../Core/WindowSettings.h"
#include "../../Core/Resources/TextureAtlas.h"
#include "UIScrollbar.h"
#include <memory>

class UIScrollView : public UIElement
{
public:
    UIScrollView(float viewWidth, float viewHeight, TextureAtlas& atlas);
    ~UIScrollView() override = default;

    void SetContent(std::unique_ptr<UIElement> content);
    void SetPadding(float left, float top);
    
    void Update(float deltaTime) override;
    void Draw(sf::RenderTarget& target) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void SetPosition(const sf::Vector2f& pos) override;
    void SetSize(const sf::Vector2f& size) override;

private:
    std::unique_ptr<UIElement> m_content;
    std::unique_ptr<UIScrollbar> m_scrollbar;
    float m_viewWidth;
    float m_viewHeight;
    float m_scrollY = 0.0f;
    float m_scrollSpeed = 30.0f;
    float m_paddingLeft = 0.0f;
    float m_paddingTop = 0.0f;
};
