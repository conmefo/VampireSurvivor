#include "UIScrollView.h"
#include <algorithm>

UIScrollView::UIScrollView(float viewWidth, float viewHeight, TextureAtlas& atlas)
    : m_viewWidth(viewWidth), m_viewHeight(viewHeight)
{
    m_size = sf::Vector2f(viewWidth, viewHeight);
    m_scrollbar = std::make_unique<UIScrollbar>(atlas);
}

void UIScrollView::SetContent(std::unique_ptr<UIElement> content)
{
    m_content = std::move(content);
}

void UIScrollView::SetPadding(float left, float top)
{
    m_paddingLeft = left;
    m_paddingTop = top;
}

void UIScrollView::Update(float deltaTime)
{
    if(m_content)
    {
        m_content->Update(deltaTime);
    }
}

void UIScrollView::SetPosition(const sf::Vector2f& pos)
{
    UIElement::SetPosition(pos);
    m_scrollbar->SetPosition(sf::Vector2f(pos.x + m_viewWidth, pos.y - 40.0f));
}

void UIScrollView::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    m_scrollbar->HandleEvent(event, window);

    float maxScroll = 0.0f;
    if(m_content)
    {
        maxScroll = std::max(0.0f, m_content->GetSize().y + m_paddingTop - m_viewHeight);
    }

    m_scrollbar->UpdateLayout(m_viewHeight, m_content ? (m_content->GetSize().y + m_paddingTop) : 0.0f);

    if(m_scrollbar->IsDragging())
    {
        m_scrollY = m_scrollbar->GetScrollRatio() * maxScroll;
    }
    else if(event.type == sf::Event::MouseWheelScrolled)
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
        
        if(worldPos.x >= m_position.x && worldPos.x <= m_position.x + m_viewWidth &&
           worldPos.y >= m_position.y && worldPos.y <= m_position.y + m_viewHeight)
        {
            m_scrollY -= event.mouseWheelScroll.delta * m_scrollSpeed;
            m_scrollY = std::max(0.0f, std::min(m_scrollY, maxScroll));
            
            if(maxScroll > 0.0f)
            {
                m_scrollbar->SetScrollRatio(m_scrollY / maxScroll);
            }
        }
    }
    
    if(m_content)
    {
        bool isMouseClickOrRelease = (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased);
        bool isMouseMove = (event.type == sf::Event::MouseMoved);

        if (isMouseClickOrRelease || isMouseMove)
        {
            sf::Vector2i mousePos;
            if (isMouseMove)
                mousePos = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
            else
                mousePos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);

            sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
            bool isInside = (worldPos.x >= m_position.x && worldPos.x <= m_position.x + m_viewWidth &&
                             worldPos.y >= m_position.y && worldPos.y <= m_position.y + m_viewHeight);

            if (!isInside)
            {
                if (isMouseClickOrRelease)
                {
                    // Do not pass click/release events outside the view
                    return; 
                }
                else if (isMouseMove)
                {
                    // Pass a fake mouse move event far away to clear any hovered state
                    sf::Event fakeEvent = event;
                    fakeEvent.mouseMove.x = -9999;
                    fakeEvent.mouseMove.y = -9999;
                    m_content->HandleEvent(fakeEvent, window);
                    return;
                }
            }
        }

        m_content->HandleEvent(event, window);
    }
}

void UIScrollView::Draw(sf::RenderTarget& target)
{
    if(!m_content)
    {
        return;
    }
    
    float nx = m_position.x / Core::VIRTUAL_WIDTH;
    float ny = m_position.y / Core::VIRTUAL_HEIGHT;
    float nw = m_viewWidth / Core::VIRTUAL_WIDTH;
    float nh = m_viewHeight / Core::VIRTUAL_HEIGHT;
    
    sf::View originalView = target.getView();
    sf::FloatRect windowViewport = originalView.getViewport();
    
    // Map our normalized coordinates through the letterboxed window viewport
    sf::FloatRect finalViewport(
        windowViewport.left + nx * windowViewport.width,
        windowViewport.top + ny * windowViewport.height,
        nw * windowViewport.width,
        nh * windowViewport.height
    );
    
    sf::View scrollView(sf::FloatRect(m_position.x, m_position.y, m_viewWidth, m_viewHeight));
    scrollView.setViewport(finalViewport);
    
    m_content->SetPosition(sf::Vector2f(m_position.x + m_paddingLeft, m_position.y - m_scrollY + m_paddingTop));
    
    target.setView(scrollView);
    
    m_content->Draw(target);
    
    target.setView(originalView);

    m_scrollbar->UpdateLayout(m_viewHeight, m_content->GetSize().y + m_paddingTop);
    m_scrollbar->Draw(target);
}
