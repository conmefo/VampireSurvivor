#pragma once

#include "../Core/UIElement.h"
#include "../Components/NineSliceComponent.h"
#include "../Components/FaderComponent.h"
#include <SFML/Graphics/Text.hpp>
#include <string>

class UIPanel : public UIElement
{
public:
    UIPanel(class TextureAtlas& atlas, const std::string& assetId, float marginLeft = 0.0f, float marginTop = 0.0f, float marginRight = 0.0f, float marginBottom = 0.0f);
    ~UIPanel() override = default;

    void Update(float deltaTime) override;
    void Draw(sf::RenderTarget& target) override;

    void SetPosition(const sf::Vector2f& pos) override;
    void SetSize(const sf::Vector2f& size) override;
    virtual void SetColor(const sf::Color& color);
    void SetTexture(class TextureAtlas& atlas, const std::string& assetId);
    void SetMargins(float left, float top, float right, float bottom);
    void SetText(const std::string& text, const sf::Font& font, unsigned int charSize = 20);

    FaderComponent& GetFader();

private:
    void CenterText();

    NineSliceComponent m_backgroundRenderer;
    FaderComponent m_fader;
    sf::Text m_text;
    bool m_hasText;
};
