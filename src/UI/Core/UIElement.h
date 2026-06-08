#pragma once

#include <SFML/Graphics.hpp>

class UIElement
{
public:
    virtual ~UIElement() = default;

    virtual void Update(float deltaTime) = 0;
    virtual void Draw(sf::RenderTarget& target) = 0;
    virtual void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) {}

    virtual void SetPosition(const sf::Vector2f& pos);
    virtual void SetSize(const sf::Vector2f& size);

    sf::Vector2f GetPosition() const;
    sf::Vector2f GetSize() const;

    void SetActive(bool active);
    bool IsActive() const;

protected:
    sf::Vector2f m_position;
    sf::Vector2f m_size;
    bool m_isActive = true;
};
