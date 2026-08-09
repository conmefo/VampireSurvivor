#include "StageItemWidget.h"
#include "../Core/TextUtility.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>

StageItemWidget::StageItemWidget(TextureAtlas& atlas, const sf::Font& font, const sf::Font* boldFont, const StageInfo& stageInfo)
    : m_stageInfo(stageInfo)
{
    m_bgPanel.SetTexture(atlas, "frame1_c2");
    m_bgPanel.SetMargins(12, 12, 12, 12);

    m_selectionFrame.SetTexture(atlas, "selectionSquare_03");
    m_selectionFrame.SetMargins(12, 12, 12, 12);
    m_selectionFrame.SetCornerScale(2.0f);

    m_dimOverlay.setFillColor(sf::Color(0, 0, 0, 35)); // Dim just a bit

    // Title Text (Gold color, normal style, inside stage icon at middle top)
    m_titleText.setFont(font);
    m_titleText.setString(sf::String::fromUtf8(stageInfo.stageName.begin(), stageInfo.stageName.end()));
    m_titleText.setCharacterSize(22);
    m_titleText.setFillColor(sf::Color(255, 215, 0)); // Gold color
    m_titleText.setStyle(sf::Text::Regular);          // Normal, not bold
    m_titleText.setOutlineThickness(0.5f);
    m_titleText.setOutlineColor(sf::Color(255, 215, 0)); // Same color as text outline

    // Description Text (White color, larger font size 21)
    m_descText.setFont(font);
    m_descText.setString(sf::String::fromUtf8(stageInfo.description.begin(), stageInfo.description.end()));
    m_descText.setCharacterSize(19); // Increased size
    m_descText.setFillColor(sf::Color::White);
    m_descText.setOutlineThickness(0.3f);
    m_descText.setOutlineColor(sf::Color::White); // Same color as text outline

    // Icon Sprite lookup
    std::string key = stageInfo.uiFrame;
    if (key.size() > 4 && key.substr(key.size() - 4) == ".png")
    {
        key = key.substr(0, key.size() - 4);
    }

    AssetTextureData iconData = atlas.GetTextureData(key);
    if (!iconData.texture)
    {
        iconData = atlas.GetTextureData(stageInfo.uiFrame);
    }

    if (iconData.texture)
    {
        m_iconSprite.setTexture(*iconData.texture);
        m_iconSprite.setTextureRect(iconData.rect);
    }
}

void StageItemWidget::InitializeLayout(const sf::Vector2f& size)
{
    SetSize(size);

    // Height matches exactly with stage icon height (151px)
    float itemHeight = ICON_HEIGHT;
    sf::Vector2f exactSize(size.x, itemHeight);

    // Left edge of background matches left edge of stage icon exactly (m_position.x)
    m_bgPanel.setPosition(m_position);
    m_bgPanel.SetSize(exactSize);
    m_dimOverlay.setPosition(m_position);
    m_dimOverlay.setSize(exactSize);

    // Stage Icon (245x151 px) at exact left edge
    float iconX = m_position.x;
    float iconY = m_position.y;
    m_iconSprite.setPosition(iconX, iconY);

    sf::FloatRect iconLocal = m_iconSprite.getLocalBounds();
    if (iconLocal.width > 0 && iconLocal.height > 0)
    {
        m_iconSprite.setScale(ICON_WIDTH / iconLocal.width, ICON_HEIGHT / iconLocal.height);
    }

    // Title Text: drawn inside stage icon at middle top
    sf::FloatRect titleBounds = m_titleText.getLocalBounds();
    m_titleText.setOrigin(titleBounds.left + titleBounds.width / 2.0f, titleBounds.top);
    float iconCenterX = iconX + ICON_WIDTH / 2.0f;
    m_titleText.setPosition(iconCenterX, iconY + 18.0f);

    // Selection frame around stage icon (center-aligned, padded larger so selectionSquare_03 is clearly visible)
    static constexpr float FRAME_PADDING = 7.0f;
    float frameX = iconX - FRAME_PADDING;
    float frameY = iconY - FRAME_PADDING;
    float frameW = ICON_WIDTH + (FRAME_PADDING * 2.0f);
    float frameH = ICON_HEIGHT + (FRAME_PADDING * 2.0f);
    m_selectionFrame.setPosition(sf::Vector2f(frameX, frameY));
    m_selectionFrame.SetSize(sf::Vector2f(frameW, frameH));

    // Description text: right of stage icon to the end of item
    float descX = iconX + ICON_WIDTH + 15.0f;
    float descY = m_position.y + 12.0f;
    float maxDescWidth = size.x - (descX - m_position.x) - 15.0f;

    m_descText.setPosition(descX, descY);
    m_descText.setString(sf::String::fromUtf8(m_stageInfo.description.begin(), m_stageInfo.description.end()));
    if (maxDescWidth > 50.0f)
    {
        UI::TextUtility::WrapText(m_descText, maxDescWidth);
    }
}

void StageItemWidget::SetPosition(const sf::Vector2f& pos)
{
    UIElement::SetPosition(pos);
    InitializeLayout(m_size);
}

void StageItemWidget::SetSelected(bool selected)
{
    m_isSelected = selected;
}

void StageItemWidget::SetOnClick(std::function<void(const std::string&)> callback)
{
    m_onClick = std::move(callback);
}

void StageItemWidget::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    if (!m_isActive) return;

    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        sf::FloatRect bounds(m_position, sf::Vector2f(m_size.x, ICON_HEIGHT));
        if (bounds.contains(mousePos))
        {
            if (m_onClick)
            {
                m_onClick(m_stageInfo.stageKey);
            }
        }
    }
}

void StageItemWidget::Update(float deltaTime)
{
    (void)deltaTime;
    m_selectionFrame.Update();
}

void StageItemWidget::Draw(sf::RenderTarget& target)
{
    if (!m_isActive) return;

    if (m_isSelected)
    {
        target.draw(m_dimOverlay);
    }

    target.draw(m_iconSprite);

    if (m_isSelected)
    {
        target.draw(m_selectionFrame);
    }

    target.draw(m_titleText);
    target.draw(m_descText);
}
