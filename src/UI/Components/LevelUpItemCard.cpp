#include "LevelUpItemCard.h"
#include "../../Core/Resources/TextureAtlas.h"
#include "../Core/TextUtility.h"
#include <cmath>
#include <algorithm>

LevelUpItemCard::LevelUpItemCard(const TextureAtlas& atlas, const sf::Font& font)
    : m_atlas(atlas)
    , m_font(font)
{
    // frame1_c2 sprite for card background
    m_bgNineSlice.SetTexture(m_atlas, "frame1_c2");
    m_bgNineSlice.SetMargins(8.0f, 8.0f, 8.0f, 8.0f);
    m_bgNineSlice.SetCornerScale(1.5f);

    // frameB sprite for item icon container
    AssetTextureData frameBData = m_atlas.GetTextureData("frameB");
    if (frameBData.texture)
    {
        m_frameBSprite.setTexture(*frameBData.texture);
        m_frameBSprite.setTextureRect(frameBData.rect);
    }

    // Load spinning arrow cursor frames (arrow_01 .. arrow_08) into AnimationData
    m_arrowAnimData.name = "arrow_cursor_spin";
    m_arrowAnimData.frameDuration = 0.08f;
    m_arrowAnimData.isLooping = true;
    for (int i = 1; i <= 8; ++i)
    {
        AssetTextureData data = m_atlas.GetTextureData("arrow_0" + std::to_string(i));
        if (data.texture)
        {
            m_arrowAnimData.frames.push_back(data);
        }
    }

    if (!m_arrowAnimData.IsEmpty())
    {
        m_arrowAnimator.Play(&m_arrowAnimData);
        m_arrowAnimator.SetOriginMode(AnimationOriginMode::CenterEachFrame);
    }

    m_titleText.setFont(m_font);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setOutlineColor(sf::Color::White);
    m_titleText.setOutlineThickness(0.5f);

    m_levelTagText.setFont(m_font);
    m_levelTagText.setOutlineColor(sf::Color(225, 239, 68));
    m_levelTagText.setOutlineThickness(0.5f);

    m_descriptionText.setFont(m_font);
    m_descriptionText.setFillColor(sf::Color::White);
    m_descriptionText.setOutlineColor(sf::Color::White);
    m_descriptionText.setOutlineThickness(0.5f);

    m_banishBtnText.setFont(m_font);
    m_banishBtnText.setFillColor(sf::Color::White);
    m_banishBtnText.setOutlineColor(sf::Color::White);
    m_banishBtnText.setOutlineThickness(0.5f);
}

LevelUpItemCard::LevelUpItemCard(LevelUpItemCard&& other) noexcept
    : m_atlas(other.m_atlas)
    , m_font(other.m_font)
    , m_bgNineSlice(std::move(other.m_bgNineSlice))
    , m_frameBSprite(std::move(other.m_frameBSprite))
    , m_itemSprite(std::move(other.m_itemSprite))
    , m_hasItemTexture(other.m_hasItemTexture)
    , m_titleText(std::move(other.m_titleText))
    , m_levelTagText(std::move(other.m_levelTagText))
    , m_descriptionText(std::move(other.m_descriptionText))
    , m_banishBtnText(std::move(other.m_banishBtnText))
    , m_arrowAnimData(std::move(other.m_arrowAnimData))
    , m_arrowAnimator(std::move(other.m_arrowAnimator))
    , m_leftArrow(std::move(other.m_leftArrow))
    , m_rightArrow(std::move(other.m_rightArrow))
    , m_option(std::move(other.m_option))
    , m_optionIndex(other.m_optionIndex)
    , m_showBanish(other.m_showBanish)
    , m_isHovered(other.m_isHovered)
    , m_isPressed(other.m_isPressed)
    , m_pressedInside(other.m_pressedInside)
    , m_cardBounds(other.m_cardBounds)
    , m_banishBounds(other.m_banishBounds)
{
    // Re-bind animator pointer to the new memory address of m_arrowAnimData
    if (!m_arrowAnimData.IsEmpty())
    {
        m_arrowAnimator.Play(&m_arrowAnimData, true);
        m_arrowAnimator.SetOriginMode(AnimationOriginMode::CenterEachFrame);
    }
}

LevelUpItemCard& LevelUpItemCard::operator=(LevelUpItemCard&& other) noexcept
{
    if (this != &other)
    {
        m_bgNineSlice = std::move(other.m_bgNineSlice);
        m_frameBSprite = std::move(other.m_frameBSprite);
        m_itemSprite = std::move(other.m_itemSprite);
        m_hasItemTexture = other.m_hasItemTexture;
        m_titleText = std::move(other.m_titleText);
        m_levelTagText = std::move(other.m_levelTagText);
        m_descriptionText = std::move(other.m_descriptionText);
        m_banishBtnText = std::move(other.m_banishBtnText);
        m_arrowAnimData = std::move(other.m_arrowAnimData);
        m_arrowAnimator = std::move(other.m_arrowAnimator);
        m_leftArrow = std::move(other.m_leftArrow);
        m_rightArrow = std::move(other.m_rightArrow);
        m_option = std::move(other.m_option);
        m_optionIndex = other.m_optionIndex;
        m_showBanish = other.m_showBanish;
        m_isHovered = other.m_isHovered;
        m_isPressed = other.m_isPressed;
        m_pressedInside = other.m_pressedInside;
        m_cardBounds = other.m_cardBounds;
        m_banishBounds = other.m_banishBounds;

        if (!m_arrowAnimData.IsEmpty())
        {
            m_arrowAnimator.Play(&m_arrowAnimData, true);
            m_arrowAnimator.SetOriginMode(AnimationOriginMode::CenterEachFrame);
        }
    }
    return *this;
}

void LevelUpItemCard::SetOptionData(const LevelUpOption& option, int optionIndex, bool showBanish)
{
    m_option = option;
    m_optionIndex = optionIndex;
    m_showBanish = showBanish && (option.type != LevelUpOptionType::FloorChicken && option.type != LevelUpOptionType::GoldBag);

    // Re-bind animator pointer to current m_arrowAnimData
    if (!m_arrowAnimData.IsEmpty())
    {
        m_arrowAnimator.Play(&m_arrowAnimData, true);
        m_arrowAnimator.SetOriginMode(AnimationOriginMode::CenterEachFrame);
    }

    // Load item sprite from TextureAtlas
    m_hasItemTexture = false;
    std::string iconFrame = option.frameName;
    if (iconFrame.empty())
    {
        if (option.type == LevelUpOptionType::FloorChicken) iconFrame = "FloorChicken";
        else if (option.type == LevelUpOptionType::GoldBag) iconFrame = "CoinGold";
        else iconFrame = option.id;
    }

    AssetTextureData itemData = m_atlas.GetTextureData(iconFrame);
    if (!itemData.texture && iconFrame != option.id)
    {
        itemData = m_atlas.GetTextureData(option.id);
    }

    if (itemData.texture)
    {
        m_itemSprite.setTexture(*itemData.texture);
        m_itemSprite.setTextureRect(itemData.rect);
        m_itemSprite.setOrigin(itemData.rect.width / 2.0f, itemData.rect.height / 2.0f);
        m_hasItemTexture = true;
    }

    // Option title (name only, no number prefix)
    std::string titleStr = option.name;
    m_titleText.setString(titleStr);
    m_titleText.setFillColor(sf::Color::White);

    // Level Tag text: "New!" or "Level: X"
    if (option.type == LevelUpOptionType::FloorChicken)
    {
        m_levelTagText.setString("HP Recovery");
        m_levelTagText.setFillColor(sf::Color::White);
    }
    else if (option.type == LevelUpOptionType::GoldBag)
    {
        m_levelTagText.setString("Gold Reward");
        m_levelTagText.setFillColor(sf::Color::White);
    }
    else if (option.isNew)
    {
        m_levelTagText.setString("New!");
        m_levelTagText.setFillColor(sf::Color(225, 239, 68)); // Gold
    }
    else
    {
        m_levelTagText.setString("Level:" + std::to_string(option.currentLevel + 1));
        m_levelTagText.setFillColor(sf::Color(225, 239, 68));
    }

    m_descriptionText.setString(option.description);
    m_descriptionText.setFillColor(sf::Color::White);

    if (m_showBanish)
    {
        m_banishBtnText.setString("[BANISH]");
        m_banishBtnText.setFillColor(sf::Color::White);
    }
    else
    {
        m_banishBtnText.setString("");
    }
}

void LevelUpItemCard::SetLayout(const sf::Vector2f& position, float width, float height, float scaleX, float scaleY)
{
    setPosition(position);
    m_cardBounds = sf::FloatRect(position.x, position.y, width, height);

    // Corner scale of card is 1.4f scaled proportionally with scaleY
    float cardCornerScale = 1.4f * scaleY;
    m_bgNineSlice.SetSize(sf::Vector2f(width, height));
    m_bgNineSlice.SetCornerScale(cardCornerScale);
    m_bgNineSlice.setPosition(0.0f, 0.0f);
    m_bgNineSlice.Update();

    // 1. FrameB container measured at 1920x1080 baseline: 70 x 70 px at TOP-LEFT of card
    float frameBWidth = 70.0f * scaleX;
    float frameBHeight = 70.0f * scaleY;
    float padX = 6.0f * scaleX;
    float padY = 6.0f * scaleY; // Top-left position

    if (m_frameBSprite.getTexture())
    {
        sf::FloatRect nativeRect = m_frameBSprite.getLocalBounds();
        if (nativeRect.width > 0.0f && nativeRect.height > 0.0f)
        {
            m_frameBSprite.setScale(frameBWidth / nativeRect.width, frameBHeight / nativeRect.height);
        }
        m_frameBSprite.setPosition(padX, padY);
    }

    // 2. Center-align weapon/passive item sprite inside frameB
    sf::Vector2f frameBCenter(padX + frameBWidth / 2.0f, padY + frameBHeight / 2.0f);
    if (m_hasItemTexture && m_itemSprite.getTexture())
    {
        sf::FloatRect itemRect = m_itemSprite.getLocalBounds();
        if (itemRect.width > 0.0f && itemRect.height > 0.0f)
        {
            float maxIconDim = std::min(48.0f * scaleX, 48.0f * scaleY);
            float itemScale = std::min(maxIconDim / itemRect.width, maxIconDim / itemRect.height);
            m_itemSprite.setScale(itemScale, itemScale);
        }
        m_itemSprite.setPosition(frameBCenter);
    }

    // 3. Option Title text to the right of frameB
    float textStartX = padX + frameBWidth + 45.0f * scaleX;

    unsigned int titleSize = static_cast<unsigned int>(std::round(24.0f * scaleY));
    m_titleText.setCharacterSize(titleSize > 10 ? titleSize : 10);
    m_titleText.setPosition(textStartX, 8.0f * scaleY);

    // 4. Level Tag ("Level: X" / "New!") LEFT-ALIGNED at a fixed offset far from left edge (430px at 1080p)
    unsigned int tagSize = static_cast<unsigned int>(std::round(22.0f * scaleY));
    m_levelTagText.setCharacterSize(tagSize > 10 ? tagSize : 10);

    float tagX = 430.0f * scaleX;
    m_levelTagText.setPosition(tagX, 8.0f * scaleY);

    if (m_showBanish)
    {
        m_banishBtnText.setCharacterSize(tagSize > 10 ? tagSize : 10);
        sf::FloatRect bBounds = m_banishBtnText.getLocalBounds();
        sf::Vector2f bPos(width - bBounds.width - 20.0f * scaleX, 18.0f * scaleY);
        m_banishBtnText.setPosition(bPos);
        m_banishBounds = sf::FloatRect(position.x + bPos.x, position.y + bPos.y, bBounds.width, bBounds.height);
    }
    else
    {
        m_banishBounds = sf::FloatRect(0, 0, 0, 0);
    }

    // 5. Spinning Arrow Cursors driven by SpriteAnimator on left & right when hovered
    float arrowPadding = 35.0f * scaleX;
    m_leftArrow.setPosition(-arrowPadding, height / 2.0f);
    m_leftArrow.setScale(2.0f * scaleY, 2.0f * scaleY);

    m_rightArrow.setPosition(width + arrowPadding, height / 2.0f);
    m_rightArrow.setScale(-2.0f * scaleY, 2.0f * scaleY); // Flipped horizontally on right!

    // 6. Description text: larger size, left-aligned at 20.0f * scaleX,
    // vertically centered between the lower edge of icon frame and lower edge of card.
    m_descriptionText.setString(m_option.description);
    unsigned int descSize = static_cast<unsigned int>(std::round(21.0f * scaleY));
    m_descriptionText.setCharacterSize(descSize > 10 ? descSize : 10);

    float descLeftX = 20.0f * scaleX;
    float rightMargin = m_showBanish ? (m_banishBtnText.getLocalBounds().width + 35.0f * scaleX) : (25.0f * scaleX);
    float maxDescWidth = width - descLeftX - rightMargin;
    if (maxDescWidth > 40.0f)
    {
        UI::TextUtility::WrapText(m_descriptionText, maxDescWidth);
    }

    // Vertical center line between lower edge of icon frame (padY + frameBHeight) and lower edge of card (height)
    float iconLowerEdge = padY + frameBHeight;
    float cardLowerEdge = height;
    float centerY = (iconLowerEdge + cardLowerEdge) / 2.0f;

    sf::FloatRect descBounds = m_descriptionText.getLocalBounds();
    m_descriptionText.setOrigin(0.0f, descBounds.top + descBounds.height / 2.0f);
    m_descriptionText.setPosition(descLeftX, centerY);
}

void LevelUpItemCard::Update(float dt, const sf::RenderWindow& window)
{
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    m_isHovered = m_cardBounds.contains(mousePos);

    if (!m_isHovered)
    {
        m_pressedInside = false;
        m_isPressed = false;
    }
    else
    {
        m_isPressed = (m_isHovered && sf::Mouse::isButtonPressed(sf::Mouse::Left)) || m_pressedInside;
    }

    // Card background dimming visual feedback: Normal (White), Hover (subtle dim), Clicked (slightly dimmer)
    sf::Color bgTint = sf::Color::White;
    if (m_isPressed)
    {
        bgTint = sf::Color(215, 215, 215); // Slightly dimmer when clicking
    }
    else if (m_isHovered)
    {
        bgTint = sf::Color(240, 240, 240); // Subtle dim when hovering
    }

    m_bgNineSlice.SetColor(bgTint);
    m_bgNineSlice.Update();

    if (m_isHovered && !m_arrowAnimData.IsEmpty())
    {
        m_arrowAnimator.Update(dt, m_leftArrow);

        // Copy current frame texture, rect, and origin to right arrow safely
        if (m_leftArrow.getTexture())
        {
            m_rightArrow.setTexture(*m_leftArrow.getTexture());
            m_rightArrow.setTextureRect(m_leftArrow.getTextureRect());
            m_rightArrow.setOrigin(m_leftArrow.getOrigin());
        }
    }
}

bool LevelUpItemCard::HandleMouseEvent(
    const sf::Event& event,
    const sf::RenderWindow& window,
    const std::function<void(int)>& onSelect,
    const std::function<void(int)>& onBanish)
{
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        if (m_showBanish && m_banishBounds.contains(mousePos))
        {
            return true;
        }
        if (m_cardBounds.contains(mousePos))
        {
            m_pressedInside = true;
            return true;
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        bool wasPressed = m_pressedInside;
        m_pressedInside = false;

        if (m_showBanish && m_banishBounds.contains(mousePos))
        {
            if (onBanish)
            {
                onBanish(m_optionIndex);
            }
            return true;
        }

        if (wasPressed && m_cardBounds.contains(mousePos))
        {
            if (onSelect)
            {
                onSelect(m_optionIndex);
            }
            return true;
        }
    }
    return false;
}

void LevelUpItemCard::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_bgNineSlice, states);
    
    if (m_frameBSprite.getTexture())
    {
        target.draw(m_frameBSprite, states);
    }
    if (m_hasItemTexture && m_itemSprite.getTexture())
    {
        target.draw(m_itemSprite, states);
    }
    
    target.draw(m_titleText, states);
    target.draw(m_levelTagText, states);
    target.draw(m_descriptionText, states);
    
    if (m_showBanish)
    {
        target.draw(m_banishBtnText, states);
    }

    if (m_isHovered && !m_arrowAnimData.IsEmpty() && m_leftArrow.getTexture() && m_rightArrow.getTexture())
    {
        target.draw(m_leftArrow, states);
        target.draw(m_rightArrow, states);
    }
}
