#include "UIManager.h"

void UIManager::AddElement(std::unique_ptr<UIElement> element)
{
    m_elements.push_back(std::move(element));
}

#include "../Core/WindowSettings.h"

void UIManager::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    // Filter out-of-bounds mouse events
    if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased || event.type == sf::Event::MouseMoved)
    {
        sf::Vector2i pixelPos;
        if (event.type == sf::Event::MouseMoved)
        {
            pixelPos = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
        }
        else
        {
            pixelPos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
        }

        sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
        
        // If the mouse is in the black pillarbox/letterbox area, drop click events
        // For movement, we still pass it so elements can un-hover, but elements themselves will see it's out of bounds.
        // Wait, the instruction says to flag them to prevent accidental activation.
        if (worldPos.x < 0.0f || worldPos.x > Core::VIRTUAL_WIDTH || worldPos.y < 0.0f || worldPos.y > Core::VIRTUAL_HEIGHT)
        {
            if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased)
            {
                return; // Drop click events outside the virtual canvas
            }
        }
    }

    for(auto& element : m_elements)
    {
        element->HandleEvent(event, window);
    }
}

void UIManager::Update(float dt)
{
    for(auto& element : m_elements)
    {
        element->Update(dt);
    }
}

void UIManager::Draw(sf::RenderTarget& target)
{
    for(auto& element : m_elements)
    {
        element->Draw(target);
    }
}
