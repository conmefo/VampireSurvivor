#pragma once

#include <SFML/Graphics.hpp>

class Agent
{
public:
    Agent();
    virtual ~Agent() = default;

    virtual void Update(float dt) = 0;
    virtual void Draw(sf::RenderTarget& target) = 0;

    void SetPosition(const sf::Vector2f& position);
    const sf::Vector2f& GetPosition() const;

    void SetVelocity(const sf::Vector2f& velocity);
    const sf::Vector2f& GetVelocity() const;

    void SetCollisionRadius(float radius);
    float GetCollisionRadius() const;
    sf::FloatRect GetBounds() const;

    void SetActive(bool active);
    bool IsActive() const;

protected:
    sf::Vector2f m_position;
    sf::Vector2f m_velocity;
    float m_collisionRadius;
    bool m_active;
};
