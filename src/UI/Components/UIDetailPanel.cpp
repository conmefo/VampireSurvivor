#include "UIDetailPanel.h"
#include <iostream>

UIDetailPanel::UIDetailPanel(TextureAtlas& atlas, const sf::Font& font)
    : m_atlas(atlas), m_font(font)
{
    m_background.SetTexture(atlas, "frame1_c2");
    m_background.SetMargins(12, 12, 12, 12);

    m_titleText.setFont(font);
    m_titleText.setCharacterSize(24);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setStyle(sf::Text::Bold);

    m_descText.setFont(font);
    m_descText.setCharacterSize(20);
    m_descText.setStyle(sf::Text::Bold);
    m_descText.setFillColor(sf::Color(200, 200, 200));

    m_priceText.setFont(font);
    m_priceText.setCharacterSize(24);
    m_priceText.setFillColor(sf::Color::Yellow);
    m_priceText.setStyle(sf::Text::Bold);

    m_buyButton = std::make_unique<UIButton>(atlas, "button_c5_normal", 10.0f, 10.0f, 10.0f, 10.0f);
    m_buyButton->SetText("Buy", font, 24);
    m_buyButton->SetStateColors(sf::Color::White, sf::Color(255, 255, 255, 200), sf::Color(255, 255, 255, 150), sf::Color(100, 100, 100, 150));
    m_buyButton->SetOnClickCallback([this]() {
        if(m_onBuyClicked && !m_currentData.id.empty())
        {
            m_onBuyClicked(m_currentData.id);
        }
    });
    
    AssetTextureData moneyData = m_atlas.GetTextureData("CoinGold");
    if(moneyData.texture)
    {
        m_coinIcon.setTexture(*moneyData.texture);
        m_coinIcon.setTextureRect(moneyData.rect);
        m_coinIcon.setOrigin(moneyData.rect.width / 2.0f, moneyData.rect.height / 2.0f);
        m_coinIcon.setScale(26.0f / moneyData.rect.width, 26.0f / moneyData.rect.height);
    }

    AssetTextureData frameData = m_atlas.GetTextureData("frameD");
    if(frameData.texture) {
        m_innerFrameSprite.setTexture(*frameData.texture);
        m_innerFrameSprite.setTextureRect(frameData.rect);
        m_innerFrameSprite.setOrigin(frameData.rect.width / 2.0f, frameData.rect.height / 2.0f);
        m_innerFrameBaseScale = sf::Vector2f(69.0f / frameData.rect.width, 69.0f / frameData.rect.height);
        m_innerFrameSprite.setScale(m_innerFrameBaseScale);
    }
}

void UIDetailPanel::SetOnBuyClicked(std::function<void(const std::string&)> callback)
{
    m_onBuyClicked = callback;
}

void UIDetailPanel::UpdateContent(const PowerUpData& data)
{
    m_currentData = data;
    m_titleText.setString(data.title);
    m_descText.setString(data.description);
    
    if (data.currentLevel >= data.maxLevel)
    {
        m_priceText.setString("MAX");
        m_buyButton->SetState(ButtonState::Disabled);
    }
    else
    {
        m_priceText.setString(std::to_string(data.price));
        m_buyButton->SetState(ButtonState::Normal);
    }

    if(data.iconRect.width > 0 && data.iconRect.height > 0)
    {
        AssetTextureData iconTexData = m_atlas.GetTextureData(data.textureId);
        if(iconTexData.texture)
        {
            m_iconSprite.setTexture(*iconTexData.texture);
            m_iconSprite.setTextureRect(data.iconRect);
            m_iconSprite.setOrigin(data.iconRect.width / 2.0f, data.iconRect.height / 2.0f);
            m_iconBaseScale = sf::Vector2f(35.0f / data.iconRect.width, 35.0f / data.iconRect.height);
            m_iconSprite.setScale(m_iconBaseScale);
        }
    }

    UpdateLayout();
}

void UIDetailPanel::SetPosition(const sf::Vector2f& pos)
{
    UIElement::SetPosition(pos);
    UpdateLayout();
}

void UIDetailPanel::SetSize(const sf::Vector2f& size)
{
    m_size = size;
    UpdateLayout();
}

void UIDetailPanel::SetCornerScale(float scale)
{
    m_background.SetCornerScale(scale);
}

void UIDetailPanel::UpdateLayout()
{
    m_background.SetSize(m_size);
    m_background.setPosition(m_position.x, m_position.y);
    m_background.Update();

    m_titleText.setPosition(m_position.x + 50.0f, m_position.y + 15.0f);
    
    float iconX = m_position.x + 90.0f;
    float iconY = m_position.y + 95.0f;
    m_innerFrameSprite.setPosition(iconX, iconY);
    m_iconSprite.setPosition(iconX, iconY);
    
    m_descText.setPosition(iconX + 80.0f, iconY - 30.0f);

    m_coinIcon.setPosition(m_position.x + m_size.x - 120.0f, m_position.y + 35.0f);
    m_priceText.setPosition(m_position.x + m_size.x - 90.0f, m_position.y + 20.0f);

    m_buyButton->SetSize(sf::Vector2f(152.0f, 77.0f));
    m_buyButton->SetCornerScale(2.0f);
    m_buyButton->SetPosition(sf::Vector2f(m_position.x + m_size.x - 170.0f, m_position.y + m_size.y - 90.0f));
}

void UIDetailPanel::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    m_buyButton->HandleEvent(event, window);
}

void UIDetailPanel::Update(float deltaTime)
{
    m_buyButton->Update(deltaTime);
}

void UIDetailPanel::Draw(sf::RenderTarget& target)
{
    target.draw(m_background);
    target.draw(m_titleText);
    target.draw(m_innerFrameSprite);
    target.draw(m_iconSprite);
    target.draw(m_descText);
    target.draw(m_priceText);
    target.draw(m_coinIcon);
    m_buyButton->Draw(target);
}
