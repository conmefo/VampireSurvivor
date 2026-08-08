#include "TreasureItemDetailCardWidget.h"
#include "UI/Core/TextUtility.h"
#include <SFML/OpenGL.hpp>
#include <algorithm>
#include <cmath>

void TreasureItemDetailCardWidget::Init(TextureAtlas& atlas)
{
    m_cardBg.SetTexture(atlas, m_config.frameTextureKey);
    m_cardBg.SetMargins(16.0f, 16.0f, 16.0f, 16.0f);
    m_cardBg.SetCornerScale(1.4f);

    m_iconBgFrame.SetTexture(atlas, m_config.iconFrameKey);
    m_iconBgFrame.SetMargins(10.0f, 10.0f, 10.0f, 10.0f);
    m_iconBgFrame.SetCornerScale(2.0f);
}

void TreasureItemDetailCardWidget::SetCardInfo(const ChestRewardItem& item, int cardIndex, int totalCards, TextureAtlas& atlas)
{
    m_currentItem = item;
    m_cardIndex = cardIndex;
    m_totalCards = std::max(1, totalCards);

    // Strip ".png" extension if present, as items_atlas.json uses keys like "Whip", "WandFire", "Garlic"
    std::string keyNoExt = item.iconKey;
    if(keyNoExt.size() > 4 && keyNoExt.substr(keyNoExt.size() - 4) == ".png")
    {
        keyNoExt = keyNoExt.substr(0, keyNoExt.size() - 4);
    }
    if(keyNoExt == "LightningRing")
    {
        keyNoExt = "LighningRing"; // Original asset typo handling
    }

    m_iconData = atlas.GetTextureData(keyNoExt);
    if(!m_iconData.texture)
    {
        m_iconData = atlas.GetTextureData(item.iconKey);
    }
    if(!m_iconData.texture && keyNoExt != "LighningRing")
    {
        m_iconData = atlas.GetTextureData("LighningRing");
    }
    if(!m_iconData.texture && !item.itemName.empty())
    {
        m_iconData = atlas.GetTextureData(item.itemName);
    }
    if(!m_iconData.texture)
    {
        m_iconData = atlas.GetTextureData("Whip");
    }
}

void TreasureItemDetailCardWidget::StartGrow()
{
    m_animState = DetailCardAnimState::Growing;
    m_animTimer = 0.0f;
    m_scaleYProgress = 0.0f;
}

void TreasureItemDetailCardWidget::StartShrink()
{
    m_animState = DetailCardAnimState::Shrinking;
    m_animTimer = 0.0f;
    m_scaleYProgress = 1.0f;
}

void TreasureItemDetailCardWidget::Update(float dt)
{
    if(m_animState == DetailCardAnimState::Growing)
    {
        m_animTimer += dt;
        float duration = std::max(0.01f, m_config.growDuration);
        float t = std::clamp(m_animTimer / duration, 0.0f, 1.0f);
        m_scaleYProgress = t * t * (3.0f - 2.0f * t); // Smoothstep curve

        if(t >= 1.0f)
        {
            m_animState = DetailCardAnimState::Idle;
            m_scaleYProgress = 1.0f;
        }
    }
    else if(m_animState == DetailCardAnimState::Shrinking)
    {
        m_animTimer += dt;
        float duration = std::max(0.01f, m_config.shrinkDuration);
        float t = std::clamp(m_animTimer / duration, 0.0f, 1.0f);
        float smoothT = t * t * (3.0f - 2.0f * t);
        m_scaleYProgress = 1.0f - smoothT;

        if(t >= 1.0f)
        {
            m_animState = DetailCardAnimState::Hidden;
            m_scaleYProgress = 0.0f;
        }
    }
}

void TreasureItemDetailCardWidget::UpdateLayout(const sf::Vector2f& viewSize, const sf::Vector2f& viewCenter, const sf::Vector2f& panelPos, const sf::Vector2f& panelSize)
{
    m_scaleX = viewSize.x / 1920.0f;
    m_scaleY = viewSize.y / 1080.0f;

    m_cardSize = sf::Vector2f(m_config.baseSize.x * m_scaleX, m_config.baseSize.y * m_scaleY);

    // Center-aligned horizontally with view
    float cardX = viewCenter.x - m_cardSize.x * 0.5f;
    // Lower edge is m_config.bottomMargin px above lower edge of View (screen bottom)
    float viewBottomY = viewCenter.y + viewSize.y * 0.5f;
    float cardY = viewBottomY - m_cardSize.y - m_config.bottomMargin * m_scaleY;

    m_cardPos = sf::Vector2f(cardX, cardY);

    m_cardBg.SetSize(m_cardSize);
    m_cardBg.setPosition(m_cardPos);
    m_cardBg.Update();
}

void TreasureItemDetailCardWidget::Draw(sf::RenderTarget& target, const sf::Font& font) const
{
    if(m_animState == DetailCardAnimState::Hidden || m_scaleYProgress <= 0.001f)
    {
        return;
    }

    float currentHeight = m_cardSize.y * m_scaleYProgress;
    sf::Vector2f clipPos = m_cardPos;
    sf::Vector2f clipSize(m_cardSize.x, currentHeight);

    sf::Vector2i topLeftPixels = target.mapCoordsToPixel(clipPos);
    sf::Vector2i bottomRightPixels = target.mapCoordsToPixel(clipPos + clipSize);

    int scissorX = std::min(topLeftPixels.x, bottomRightPixels.x);
    int scissorWidth = std::abs(bottomRightPixels.x - topLeftPixels.x);
    int scissorY = static_cast<int>(target.getSize().y) - std::max(topLeftPixels.y, bottomRightPixels.y);
    int scissorHeight = std::abs(bottomRightPixels.y - topLeftPixels.y);

    // ---- SCISSOR BLOCK: sprites & shapes only (no sf::Text!) ----
    // pushGLStates corrupts SFML's internal font-texture shader,
    // so sf::Text must be drawn AFTER popGLStates/resetGLStates.
    target.pushGLStates();
    glEnable(GL_SCISSOR_TEST);
    glScissor(scissorX, scissorY, scissorWidth, scissorHeight);

    // 1. Draw frame1_c2 9-slice card background
    target.draw(m_cardBg);

    // 2. Under Level Text: Icon & Frame (sprites)
    sf::Vector2f iconCenter = m_cardPos + sf::Vector2f(m_config.iconPos.x * m_scaleX, m_config.iconPos.y * m_scaleY);
    float frameSize = m_config.iconFrameSize * m_scaleY;
    sf::Vector2f framePos = iconCenter - sf::Vector2f(frameSize * 0.5f, frameSize * 0.5f);

    const_cast<NineSliceComponent&>(m_iconBgFrame).SetSize(sf::Vector2f(frameSize, frameSize));
    const_cast<NineSliceComponent&>(m_iconBgFrame).setPosition(framePos);
    const_cast<NineSliceComponent&>(m_iconBgFrame).Update();
    target.draw(m_iconBgFrame);

    if(m_iconData.texture && m_iconData.rect.width > 0 && m_iconData.rect.height > 0)
    {
        sf::Sprite iconSprite;
        iconSprite.setTexture(*m_iconData.texture);
        iconSprite.setTextureRect(m_iconData.rect);
        iconSprite.setOrigin(m_iconData.rect.width * 0.5f, m_iconData.rect.height * 0.5f);
        iconSprite.setPosition(iconCenter);
        float targetIconSize = m_config.iconSize * m_scaleY;
        float iconScale = targetIconSize / static_cast<float>(m_iconData.rect.height);
        iconSprite.setScale(iconScale, iconScale);
        target.draw(iconSprite);
    }

    glDisable(GL_SCISSOR_TEST);
    target.popGLStates();
    target.resetGLStates();

    // ---- TEXT BLOCK: drawn OUTSIDE pushGLStates so font shader works ----
    // Helper for formatting text with 0.5px matching outline and alignment modes (0=Left, 1=Center, 2=Right)
    auto configureText = [&](sf::Text& txt, const std::string& str, unsigned int fontSize, const sf::Vector2f& relPos, const sf::Color& col, int alignMode = 0) {
        txt.setFont(font);
        txt.setString(str);
        txt.setCharacterSize(static_cast<unsigned int>(std::round(fontSize * m_scaleY)));
        txt.setFillColor(col);
        txt.setOutlineColor(col);
        txt.setOutlineThickness(m_config.textOutlineThickness * m_scaleY);

        sf::FloatRect bounds = txt.getLocalBounds();
        float posX = m_cardPos.x + relPos.x * m_scaleX;
        float posY = m_cardPos.y + relPos.y * m_scaleY;

        if(alignMode == 1) // Center
        {
            posX -= bounds.width * 0.5f;
        }
        else if(alignMode == 2) // Right
        {
            posX -= bounds.width;
        }

        txt.setPosition(posX - bounds.left, posY - bounds.top);
        target.draw(txt);
    };

    // 3. Top-Left: Level Text (White)
    std::string levelStr = (m_currentItem.newLevel > 0) ? ("Level: " + std::to_string(m_currentItem.newLevel)) : "LV UP!";
    configureText(m_levelText, levelStr, m_config.levelFontSize, m_config.levelTextPos, m_config.levelTextColor, 0);

    // 4. Top-Right: Card Order Text (Dark Gray e.g. "1/3") — Right-aligned to never overflow!
    std::string orderStr = std::to_string(m_cardIndex + 1) + "/" + std::to_string(m_totalCards);
    configureText(m_orderText, orderStr, m_config.orderFontSize, m_config.orderTextPos, m_config.orderTextColor, 2);

    // 5. Middle-Top: Item Name (Gold) — Center-aligned
    std::string nameStr = !m_currentItem.itemName.empty() ? m_currentItem.itemName : "Reward Item";
    configureText(m_nameText, nameStr, m_config.nameFontSize, m_config.nameTextPos, m_config.nameTextColor, 1);

    // 6. Right of Icon: Item Description (White) — Auto-wrapped using UI::TextUtility::WrapText
    std::string descStr = m_currentItem.description;
    if(descStr.empty())
    {
        if(!m_currentItem.itemName.empty())
        {
            descStr = m_currentItem.itemName + " upgraded to level " + std::to_string(m_currentItem.newLevel) + ".";
        }
        else
        {
            descStr = "Fires projectiles at enemies.";
        }
    }

    m_descText.setFont(font);
    m_descText.setString(descStr);
    m_descText.setCharacterSize(static_cast<unsigned int>(std::round(m_config.descFontSize * m_scaleY)));
    m_descText.setFillColor(m_config.descTextColor);
    m_descText.setOutlineColor(m_config.descTextColor);
    m_descText.setOutlineThickness(m_config.textOutlineThickness * m_scaleY);

    float maxDescWidth = (m_config.baseSize.x - m_config.descTextPos.x - 25.0f) * m_scaleX;
    UI::TextUtility::WrapText(m_descText, maxDescWidth);

    sf::FloatRect descBounds = m_descText.getLocalBounds();
    float descX = m_cardPos.x + m_config.descTextPos.x * m_scaleX;
    float descY = m_cardPos.y + m_config.descTextPos.y * m_scaleY;
    m_descText.setPosition(descX - descBounds.left, descY - descBounds.top);
    target.draw(m_descText);
}
