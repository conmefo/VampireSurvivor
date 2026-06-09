#include "PowerUpCard.h"

PowerUpCard::PowerUpCard(TextureAtlas& atlas, const sf::Font& font)
    : m_atlas(atlas), m_font(font)
{
}

void PowerUpCard::InitDummyData()
{
    // Outer Background
    m_bgNineSlice.SetTexture(m_atlas, "frame1_c2");
    m_bgNineSlice.SetMargins(12, 12, 12, 12);
    m_bgNineSlice.SetSize(sf::Vector2f(174.0f, 174.0f));
    m_size = sf::Vector2f(174.0f, 174.0f);

    // Inner frame
    AssetTextureData frameData = m_atlas.GetTextureData("frameD");
    if(frameData.texture) {
        m_innerFrameSprite.setTexture(*frameData.texture);
        m_innerFrameSprite.setTextureRect(frameData.rect);
        m_innerFrameSprite.setOrigin(frameData.rect.width / 2.0f, frameData.rect.height / 2.0f);
        m_innerFrameBaseScale = sf::Vector2f(69.0f / frameData.rect.width, 69.0f / frameData.rect.height);
    }

    // Icon
    AssetTextureData iconData = m_atlas.GetTextureData("MoneyPile");
    if(iconData.texture) {
        m_iconSprite.setTexture(*iconData.texture);
        m_iconSprite.setTextureRect(iconData.rect);
        m_iconSprite.setOrigin(iconData.rect.width / 2.0f, iconData.rect.height / 2.0f);
        m_iconBaseScale = sf::Vector2f(35.0f / iconData.rect.width, 35.0f / iconData.rect.height);
    }

    // Title
    m_titleText.setFont(m_font);
    m_titleText.setString("Might");
    m_titleText.setCharacterSize(24);
    sf::FloatRect textRect = m_titleText.getLocalBounds();
    m_titleText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);

    // Flyweight checkboxes
    AssetTextureData emptyBoxData = m_atlas.GetTextureData("menu_checkbox_24_bg");
    if(emptyBoxData.texture) {
        m_checkboxSprite.setTexture(*emptyBoxData.texture);
        m_emptyBoxRect = emptyBoxData.rect;
        m_checkboxBaseScale = sf::Vector2f(1.0f, 1.0f); // keep original size
    }

    AssetTextureData filledBoxData = m_atlas.GetTextureData("menu_checkbox_24_checkmark");
    if(filledBoxData.texture) {
        m_filledBoxRect = filledBoxData.rect;
    }

    m_maxLevel = 5;
    m_currentLevel = 0;
    SetScale(1.0f); // Applies the base scales!
}

void PowerUpCard::InjectData(const PowerUpData& data)
{
    m_titleText.setString(data.title);
    sf::FloatRect textRect = m_titleText.getLocalBounds();
    m_titleText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    
    m_currentLevel = data.currentLevel;
    m_maxLevel = data.maxLevel;
    
    if(data.iconRect.width > 0 && data.iconRect.height > 0)
    {
        m_iconSprite.setTextureRect(data.iconRect);
        m_iconSprite.setOrigin(data.iconRect.width / 2.0f, data.iconRect.height / 2.0f);
        m_iconBaseScale = sf::Vector2f(35.0f / data.iconRect.width, 35.0f / data.iconRect.height);
        m_iconSprite.setScale(m_iconBaseScale.x * m_scale, m_iconBaseScale.y * m_scale);
    }
    
    if(data.emptyBoxRect.width > 0) m_emptyBoxRect = data.emptyBoxRect;
    if(data.filledBoxRect.width > 0) m_filledBoxRect = data.filledBoxRect;
}

void PowerUpCard::SetPosition(const sf::Vector2f& pos)
{
    UIElement::SetPosition(pos);
    UpdateLayout();
}

void PowerUpCard::SetScale(float scale)
{
    m_scale = scale;
    UpdateLayout();
}

void PowerUpCard::SetBgColor(const sf::Color& color)
{
    m_bgNineSlice.SetColor(color);
    m_bgNineSlice.Update();
}

void PowerUpCard::UpdateLayout()
{
    m_bgNineSlice.setPosition(m_position.x - m_size.x * m_scale / 2.0f, m_position.y - m_size.y * m_scale / 2.0f);
    m_bgNineSlice.setScale(m_scale, m_scale);
    
    m_innerFrameSprite.setScale(m_innerFrameBaseScale.x * m_scale, m_innerFrameBaseScale.y * m_scale);
    m_iconSprite.setScale(m_iconBaseScale.x * m_scale, m_iconBaseScale.y * m_scale);
    m_titleText.setScale(m_scale, m_scale);
    m_checkboxSprite.setScale(m_checkboxBaseScale.x * m_scale, m_checkboxBaseScale.y * m_scale);
    
    m_bgNineSlice.Update();
    
    m_titleText.setPosition(m_position.x, m_position.y - m_size.y * m_scale / 2.0f + 30.0f * m_scale);
    
    // Center the inner frame exactly on the outer background's center
    m_innerFrameSprite.setPosition(m_position);
    m_iconSprite.setPosition(m_position);
}

void PowerUpCard::Update(float deltaTime)
{
}

void PowerUpCard::Draw(sf::RenderTarget& target)
{
    target.draw(m_bgNineSlice);
    target.draw(m_innerFrameSprite);
    target.draw(m_iconSprite);
    target.draw(m_titleText);

    // Draw checkboxes using Flyweight pattern
    float spacing = 25.0f * m_scale;
    float startX = m_position.x - ((m_maxLevel - 1) * spacing) / 2.0f;
    float startY = m_position.y + m_size.y * m_scale / 2.0f - 20.0f * m_scale - 7.0f;

    for(int i = 0; i < m_maxLevel; ++i)
    {
        m_checkboxSprite.setPosition(startX + i * spacing, startY);
        
        // Draw the background box
        m_checkboxSprite.setTextureRect(m_emptyBoxRect);
        m_checkboxSprite.setOrigin(m_emptyBoxRect.width / 2.0f, m_emptyBoxRect.height / 2.0f);
        target.draw(m_checkboxSprite);

        // Draw the checkmark layered on top if achieved
        if(i < m_currentLevel)
        {
            m_checkboxSprite.setTextureRect(m_filledBoxRect);
            m_checkboxSprite.setOrigin(m_filledBoxRect.width / 2.0f, m_filledBoxRect.height / 2.0f);
            target.draw(m_checkboxSprite);
        }
    }
}
