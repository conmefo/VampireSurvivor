#include "UIPanel.h"



UIPanel::UIPanel(TextureAtlas& atlas, const std::string& assetId, float marginLeft, float marginTop, float marginRight, float marginBottom)
    : m_hasText(false)
{
    m_backgroundRenderer.SetTexture(atlas, assetId);
    m_backgroundRenderer.SetMargins(marginLeft, marginTop, marginRight, marginBottom);
    m_fader.SetDuration(0.5f); // Default

    // Just set an initial size, it'll usually be overridden by SetSize
    SetSize(sf::Vector2f(100.f, 100.f)); 
}

void UIPanel::Update(float deltaTime)
{
    if(!m_isActive)
    {
        return;
    }
    
    m_fader.Update(deltaTime);
    m_backgroundRenderer.Update();

    if(m_hasText)
    {
        sf::Color textColor = m_text.getFillColor();
        textColor.a = static_cast<sf::Uint8>(m_fader.GetFadeRatio() * 255.0f);
        m_text.setFillColor(textColor);
    }
}

void UIPanel::Draw(sf::RenderTarget& target)
{
    if(!m_isActive)
    {
        return;
    }
    
    target.draw(m_backgroundRenderer);
    if(m_hasText)
    {
        target.draw(m_text);
    }
}

void UIPanel::SetPosition(const sf::Vector2f& pos)
{
    UIElement::SetPosition(pos);
    m_backgroundRenderer.setPosition(pos);
    AlignText();
}

void UIPanel::SetSize(const sf::Vector2f& size)
{
    UIElement::SetSize(size);
    m_backgroundRenderer.SetSize(size);
    AlignText();
}

void UIPanel::SetColor(const sf::Color& color)
{
    m_backgroundRenderer.SetColor(color);
}

void UIPanel::SetTexture(TextureAtlas& atlas, const std::string& assetId)
{
    m_backgroundRenderer.SetTexture(atlas, assetId);
}

void UIPanel::SetMargins(float left, float top, float right, float bottom)
{
    m_backgroundRenderer.SetMargins(left, top, right, bottom);
}

void UIPanel::SetCornerScale(float scale)
{
    m_backgroundRenderer.SetCornerScale(scale);
}

FaderComponent& UIPanel::GetFader()
{
    return m_fader;
}

void UIPanel::SetText(const std::string& text, const sf::Font& font, unsigned int charSize)
{
    m_text.setFont(font);
    m_text.setString(text);
    m_text.setCharacterSize(charSize);
    m_hasText = true;
    AlignText();
}

void UIPanel::SetTextAlignment(TextAlignment alignment)
{
    m_textAlignment = alignment;
    AlignText();
}

void UIPanel::SetTextSize(unsigned int size)
{
    m_text.setCharacterSize(size);
    AlignText();
}

void UIPanel::SetTextStyle(sf::Uint32 style)
{
    m_text.setStyle(style);
    AlignText();
}

void UIPanel::AlignText()
{
    if(!m_hasText)
    {
        return;
    }

    sf::FloatRect textRect = m_text.getLocalBounds();
    float padding = 15.0f; // Padding from the edge for left/right alignment

    if (m_textAlignment == TextAlignment::Center) {
        m_text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        m_text.setPosition(m_position.x + m_size.x / 2.0f, m_position.y + m_size.y / 2.0f);
    }
    else if (m_textAlignment == TextAlignment::Left) {
        m_text.setOrigin(textRect.left, textRect.top + textRect.height / 2.0f);
        m_text.setPosition(m_position.x + padding, m_position.y + m_size.y / 2.0f);
    }
    else if (m_textAlignment == TextAlignment::Right) {
        m_text.setOrigin(textRect.left + textRect.width, textRect.top + textRect.height / 2.0f);
        m_text.setPosition(m_position.x + m_size.x - padding, m_position.y + m_size.y / 2.0f);
    }
}
