#include "UIElement.h"

void UIElement::SetPosition(const sf::Vector2f& pos)
{
    m_position = pos;
}

void UIElement::SetSize(const sf::Vector2f& size)
{
    m_size = size;
}

sf::Vector2f UIElement::GetPosition() const
{
    return m_position;
}

sf::Vector2f UIElement::GetSize() const
{
    return m_size;
}

void UIElement::SetActive(bool active)
{
    m_isActive = active;
}

bool UIElement::IsActive() const
{
    return m_isActive;
}
