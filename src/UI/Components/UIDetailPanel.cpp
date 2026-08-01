#include "UIDetailPanel.h"
#include <iostream>
#include "../Core/TextUtility.h"

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
    m_descText.setFillColor(sf::Color::White);
    m_descText.setOutlineColor(sf::Color::White);
    m_descText.setOutlineThickness(0.40f);

    m_priceText.setFont(font);
    m_priceText.setCharacterSize(24);
    m_priceText.setFillColor(sf::Color::White);
    m_priceText.setOutlineColor(sf::Color::White);
    m_priceText.setOutlineThickness(0.40f);

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

    AssetTextureData checkboxData = m_atlas.GetTextureData("menu_checkbox_24_bg");
    if(!checkboxData.texture) {
        checkboxData = m_atlas.GetTextureData("menu_checkbox_24");
    }
    if(checkboxData.texture) {
        m_checkboxBgSprite.setTexture(*checkboxData.texture);
        m_checkboxBgSprite.setTextureRect(checkboxData.rect);
        m_checkboxBgSprite.setOrigin(checkboxData.rect.width / 2.0f, checkboxData.rect.height / 2.0f);
        // Height equals the height of the buy button (77px)
        float targetH = 77.0f;
        float scaleVal = targetH / checkboxData.rect.height;
        m_checkboxBgSprite.setScale(scaleVal, scaleVal);
    }

    AssetTextureData checkmarkData = m_atlas.GetTextureData("yes16");
    if(!checkmarkData.texture) {
        checkmarkData = m_atlas.GetTextureData("menu_checkbox_24_checkmark");
    }
    if(checkmarkData.texture) {
        m_checkmarkIcon.setTexture(*checkmarkData.texture);
        m_checkmarkIcon.setTextureRect(checkmarkData.rect);
        m_checkmarkIcon.setOrigin(checkmarkData.rect.width / 2.0f, checkmarkData.rect.height / 2.0f);
        m_checkmarkIcon.setScale(52.0f / checkmarkData.rect.width, 52.0f / checkmarkData.rect.height);
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

    // Dim detail panel with dark gray when selected card is unactive/disabled
    if (data.isDisabled)
    {
        m_background.SetColor(sf::Color(125, 125, 125, 255));
    }
    else
    {
        m_background.SetColor(sf::Color::White);
    }
    m_background.Update();
    
    m_isMaxLevelMode = (data.currentLevel >= data.maxLevel);
    if (m_isMaxLevelMode)
    {
        m_priceText.setString("Active");
        m_showCoinIcon = false;
        m_showCheckmark = !data.isDisabled;
    }
    else
    {
        m_priceText.setString(std::to_string(data.price));
        m_showCoinIcon = true;
        m_showCheckmark = false;

        m_buyButton->SetTexture(m_atlas, "button_c5_normal");
        m_buyButton->SetText("Buy", m_font, 24);
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

    // Calculate description bounding box and center inside detail panel space
    m_descText.setString(m_currentData.description);
    float leftLimit = iconX + 60.0f;
    float rightLimit = m_position.x + m_size.x - 200.0f;
    float maxDescWidth = rightLimit - leftLimit;
    if (maxDescWidth > 0.0f)
    {
        UI::TextUtility::WrapText(m_descText, maxDescWidth);
    }
    sf::FloatRect descLocal = m_descText.getLocalBounds();
    m_descText.setOrigin(descLocal.left + descLocal.width / 2.0f, descLocal.top + descLocal.height / 2.0f);
    float centerX = (leftLimit + rightLimit) / 2.0f;
    float centerY = m_position.y + m_size.y / 2.0f + 10.0f;
    m_descText.setPosition(centerX, centerY);

    sf::Vector2f buyPos(m_position.x + m_size.x - 170.0f, m_position.y + m_size.y - 90.0f);
    m_buyButton->SetSize(sf::Vector2f(152.0f, 77.0f));
    m_buyButton->SetCornerScale(2.0f);
    m_buyButton->SetPosition(buyPos);

    sf::Vector2f btnCenter(buyPos.x + 76.0f, buyPos.y + 38.5f);
    m_checkboxBgSprite.setPosition(btnCenter);
    m_checkmarkIcon.setPosition(btnCenter);

    sf::FloatRect priceLocal = m_priceText.getLocalBounds();
    float comboY = btnCenter.y - 62.0f; // Higher up so it doesn't touch the button

    if (m_isMaxLevelMode)
    {
        m_priceText.setOrigin(priceLocal.left + priceLocal.width / 2.0f, priceLocal.top + priceLocal.height / 2.0f);
        m_priceText.setPosition(btnCenter.x, comboY);
    }
    else
    {
        m_priceText.setOrigin(priceLocal.left, priceLocal.top + priceLocal.height / 2.0f);
        float comboWidth = 26.0f + 8.0f + priceLocal.width;
        float comboStartX = btnCenter.x - comboWidth / 2.0f;

        m_coinIcon.setPosition(comboStartX + 13.0f, comboY);
        m_priceText.setPosition(comboStartX + 26.0f + 8.0f, comboY);
    }
}

void UIDetailPanel::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    if (m_isMaxLevelMode)
    {
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
        {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
            sf::FloatRect clickArea(m_position.x + m_size.x - 180.0f, m_position.y + m_size.y - 100.0f, 170.0f, 90.0f);
            if (clickArea.contains(mousePos))
            {
                if (m_onBuyClicked && !m_currentData.id.empty())
                {
                    m_onBuyClicked(m_currentData.id);
                }
            }
        }
    }
    else
    {
        m_buyButton->HandleEvent(event, window);
    }
}

void UIDetailPanel::Update(float deltaTime)
{
    if (!m_isMaxLevelMode)
    {
        m_buyButton->Update(deltaTime);
    }
}

void UIDetailPanel::Draw(sf::RenderTarget& target)
{
    target.draw(m_background);
    target.draw(m_titleText);
    target.draw(m_innerFrameSprite);
    target.draw(m_iconSprite);
    target.draw(m_descText);
    target.draw(m_priceText);
    if(m_showCoinIcon) target.draw(m_coinIcon);
    
    if (m_isMaxLevelMode)
    {
        target.draw(m_checkboxBgSprite);
        if(m_showCheckmark) target.draw(m_checkmarkIcon);
    }
    else
    {
        m_buyButton->Draw(target);
    }
}
