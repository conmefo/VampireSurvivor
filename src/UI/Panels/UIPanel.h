#pragma once

#include "../Core/UIElement.h"
#include "../Components/NineSliceComponent.h"
#include "../Components/FaderComponent.h"

class UIPanel : public UIElement
{
public:
    UIPanel(const sf::Texture& texture, float left, float top, float right, float bottom);
    ~UIPanel() override = default;

    void Update(float deltaTime) override;
    void Draw(sf::RenderTarget& target) override;

    void SetPosition(const sf::Vector2f& pos) override;
    void SetSize(const sf::Vector2f& size) override;
    virtual void SetColor(const sf::Color& color);

    FaderComponent& GetFader();

private:
    NineSliceComponent m_backgroundRenderer;
    FaderComponent m_fader;
};
