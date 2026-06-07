#include "UIPanel.h"

UIPanel::UIPanel(const sf::Texture& texture, float left, float top, float right, float bottom)
{
    m_backgroundRenderer.SetTexture(texture);
    m_backgroundRenderer.SetMargins(left, top, right, bottom);
    m_fader.SetDuration(0.5f); // Default
}

void UIPanel::Update(float deltaTime)
{
    if(!m_isActive)
    {
        return;
    }
    
    m_fader.Update(deltaTime);
    m_backgroundRenderer.Update();
}

void UIPanel::Draw(sf::RenderTarget& target)
{
    if(!m_isActive)
    {
        return;
    }
    
    target.draw(m_backgroundRenderer);
}

void UIPanel::SetPosition(const sf::Vector2f& pos)
{
    UIElement::SetPosition(pos);
    m_backgroundRenderer.setPosition(pos);
}

void UIPanel::SetSize(const sf::Vector2f& size)
{
    UIElement::SetSize(size);
    m_backgroundRenderer.SetSize(size);
}

void UIPanel::SetColor(const sf::Color& color)
{
    m_backgroundRenderer.SetColor(color);
}

FaderComponent& UIPanel::GetFader()
{
    return m_fader;
}
