#include "UIButton.h"



UIButton::UIButton(TextureAtlas& atlas, const std::string& assetId, float marginLeft, float marginTop, float marginRight, float marginBottom)
    : UIPanel(atlas, assetId, marginLeft, marginTop, marginRight, marginBottom)
    , m_state(ButtonState::Normal)
    , m_normalColor(sf::Color::White)
    , m_hoverColor(sf::Color(220, 220, 220))
    , m_pressedColor(sf::Color(180, 180, 180))
    , m_disabledColor(sf::Color(100, 100, 100, 150))
    , m_onClickAction(nullptr)
    , m_isFocused(false)
{
    UpdateVisuals();
}

void UIButton::SetOnClickCallback(std::function<void()> callback)
{
    m_onClickAction = std::move(callback);
}

void UIButton::SetFocus(bool focused)
{
    if(m_state == ButtonState::Disabled)
    {
        return;
    }

    m_isFocused = focused;
    
    if(m_isFocused && m_state != ButtonState::Pressed)
    {
        SetState(ButtonState::Hovered);
    }
    else if(!m_isFocused && m_state != ButtonState::Pressed)
    {
        SetState(ButtonState::Normal);
    }
}

bool UIButton::IsFocused() const
{
    return m_isFocused;
}

void UIButton::SetStateColors(const sf::Color& normal, const sf::Color& hover, const sf::Color& pressed, const sf::Color& disabled)
{
    m_normalColor = normal;
    m_hoverColor = hover;
    m_pressedColor = pressed;
    m_disabledColor = disabled;
    UpdateVisuals();
}

void UIButton::SetState(ButtonState state)
{
    m_state = state;
    UpdateVisuals();
}

bool UIButton::Contains(const sf::Vector2f& point) const
{
    sf::FloatRect bounds(m_position.x, m_position.y, m_size.x, m_size.y);
    return bounds.contains(point);
}

void UIButton::UpdateVisuals()
{
    switch(m_state)
    {
        case ButtonState::Normal:
            SetColor(m_normalColor);
            break;
        case ButtonState::Hovered:
            SetColor(m_hoverColor);
            break;
        case ButtonState::Pressed:
            SetColor(m_pressedColor);
            break;
        case ButtonState::Disabled:
            SetColor(m_disabledColor);
            break;
    }
}

void UIButton::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    if(!m_isActive || m_state == ButtonState::Disabled)
    {
        return;
    }

    if(event.type == sf::Event::MouseMoved)
    {
        sf::Vector2i pixelPos(event.mouseMove.x, event.mouseMove.y);
        sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
        
        bool isHovering = Contains(worldPos);
        
        if(isHovering && !m_isFocused)
        {
            SetFocus(true);
        }
        else if(!isHovering && m_isFocused)
        {
            SetFocus(false);
        }
    }
    else if(event.type == sf::Event::MouseButtonPressed)
    {
        if(event.mouseButton.button == sf::Mouse::Left && m_isFocused)
        {
            SetState(ButtonState::Pressed);
        }
    }
    else if(event.type == sf::Event::MouseButtonReleased)
    {
        if(event.mouseButton.button == sf::Mouse::Left && m_state == ButtonState::Pressed)
        {
            sf::Vector2i pixelPos(event.mouseButton.x, event.mouseButton.y);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            
            if(Contains(worldPos))
            {
                SetState(ButtonState::Hovered);
                if(m_onClickAction)
                {
                    m_onClickAction();
                }
            }
            else
            {
                SetState(ButtonState::Normal);
                m_isFocused = false;
            }
        }
    }
}
