#include "UIScrollbar.h"
#include <algorithm>

UIScrollbar::UIScrollbar(TextureAtlas& atlas)
    : m_atlas(atlas)
{
    // Initialize track slices
    AssetTextureData trackData = m_atlas.GetTextureData("sliderrail_c4");
    if(trackData.texture)
    {
        m_trackSprite.setTexture(*trackData.texture);
        m_trackTop = sf::IntRect(trackData.rect.left, trackData.rect.top, 8, 8);
        m_trackMid = sf::IntRect(trackData.rect.left, trackData.rect.top + 8, 8, 16);
        m_trackBot = sf::IntRect(trackData.rect.left, trackData.rect.top + 24, 8, 8);
    }

    // Initialize thumb slices
    AssetTextureData thumbData = m_atlas.GetTextureData("menu_slider_button_strip3");
    if(thumbData.texture)
    {
        m_thumbSprite.setTexture(*thumbData.texture);
        m_thumbTop = sf::IntRect(thumbData.rect.left, thumbData.rect.top, 8, 4);
        m_thumbMid = sf::IntRect(thumbData.rect.left, thumbData.rect.top + 4, 8, 6);
        m_thumbBot = sf::IntRect(thumbData.rect.left, thumbData.rect.top + 10, 8, 4);
    }
}

void UIScrollbar::UpdateLayout(float viewportHeight, float contentHeight)
{
    m_viewportHeight = viewportHeight;
    m_contentHeight = contentHeight;
    m_trackHeight = viewportHeight;

    if(m_contentHeight <= m_viewportHeight)
    {
        m_thumbHeight = 20.0f;
        m_scrollRatio = 0.0f;
    }
    else
    {
        m_thumbHeight = 20.0f; // Fixed requested height
    }
}

void UIScrollbar::SetScrollRatio(float ratio)
{
    if(!m_isDragging)
    {
        m_scrollRatio = std::max(0.0f, std::min(ratio, 1.0f));
    }
}

void UIScrollbar::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    if(m_contentHeight <= m_viewportHeight)
    {
        return;
    }

    float travelRange = m_trackHeight - m_thumbHeight;
    float thumbY = m_position.y + m_scrollRatio * travelRange;
    float thumbX = m_position.x;
    float thumbWidth = 12.0f; // 8 * 1.5

    if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        if(worldPos.x >= thumbX && worldPos.x <= thumbX + thumbWidth &&
           worldPos.y >= thumbY && worldPos.y <= thumbY + m_thumbHeight)
        {
            m_isDragging = true;
            m_dragOffsetY = worldPos.y - thumbY;
        }
    }
    else if(event.type == sf::Event::MouseMoved)
    {
        if(m_isDragging)
        {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

            float newThumbY = worldPos.y - m_dragOffsetY;
            newThumbY = std::max(m_position.y, std::min(newThumbY, m_position.y + travelRange));

            if(travelRange > 0.0f)
            {
                m_scrollRatio = (newThumbY - m_position.y) / travelRange;
            }
        }
    }
    else if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
    {
        m_isDragging = false;
    }
}

void UIScrollbar::Update(float deltaTime)
{
}

void UIScrollbar::Draw(sf::RenderTarget& target)
{
    if(m_contentHeight <= m_viewportHeight)
    {
        return; // Don't draw if content fits
    }

    // Draw Track
    Draw3Slice(target, m_trackSprite, m_trackTop, m_trackMid, m_trackBot, m_position.x, m_position.y, m_trackHeight);

    // Draw Thumb
    float travelRange = m_trackHeight - m_thumbHeight;
    float thumbY = m_position.y + m_scrollRatio * travelRange;
    Draw3Slice(target, m_thumbSprite, m_thumbTop, m_thumbMid, m_thumbBot, m_position.x, thumbY, m_thumbHeight);
}

void UIScrollbar::Draw3Slice(sf::RenderTarget& target, sf::Sprite& sprite, const sf::IntRect& top, const sf::IntRect& mid, const sf::IntRect& bot, float x, float y, float targetHeight)
{
    float scaleX = 1.5f;

    // Draw Top
    sprite.setTextureRect(top);
    sprite.setPosition(x, y);
    sprite.setScale(scaleX, scaleX);
    target.draw(sprite);

    // Draw Middle
    float topHeight = top.height * scaleX;
    float botHeight = bot.height * scaleX;
    float midTargetHeight = targetHeight - topHeight - botHeight;

    if(midTargetHeight > 0.0f)
    {
        sprite.setTextureRect(mid);
        sprite.setPosition(x, y + topHeight);
        float scaleY = midTargetHeight / mid.height;
        sprite.setScale(scaleX, scaleY);
        target.draw(sprite);
    }

    // Draw Bottom
    sprite.setTextureRect(bot);
    sprite.setPosition(x, y + targetHeight - botHeight);
    sprite.setScale(scaleX, scaleX);
    target.draw(sprite);
}
