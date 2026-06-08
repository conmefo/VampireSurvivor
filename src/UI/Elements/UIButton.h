#pragma once

#include "../Panels/UIPanel.h"
#include <functional>

enum class ButtonState
{
    Normal,
    Hovered,
    Pressed,
    Disabled
};

class UIButton : public UIPanel
{
public:
    UIButton(class TextureAtlas& atlas, const std::string& assetId, float marginLeft = 0.0f, float marginTop = 0.0f, float marginRight = 0.0f, float marginBottom = 0.0f);
    ~UIButton() override = default;

    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window);
    
    void SetOnClickCallback(std::function<void()> callback);
    void SetFocus(bool focused);
    bool IsFocused() const;

    void SetStateColors(const sf::Color& normal, const sf::Color& hover, const sf::Color& pressed, const sf::Color& disabled);
    void SetState(ButtonState state);

    bool Contains(const sf::Vector2f& point) const;

private:
    void UpdateVisuals();

    ButtonState m_state;
    sf::Color m_normalColor;
    sf::Color m_hoverColor;
    sf::Color m_pressedColor;
    sf::Color m_disabledColor;
    
    std::function<void()> m_onClickAction;
    bool m_isFocused;
};
