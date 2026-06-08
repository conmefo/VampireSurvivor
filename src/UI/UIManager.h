#pragma once

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Core/UIElement.h"

class UIManager
{
public:
    void AddElement(std::unique_ptr<UIElement> element);
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void Update(float dt);
    void Draw(sf::RenderTarget& target);

private:
    std::vector<std::unique_ptr<UIElement>> m_elements;
};
