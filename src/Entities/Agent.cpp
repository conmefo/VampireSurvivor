#include "Agent.h"

Agent::Agent()
    : m_position(0.0f, 0.0f),
      m_velocity(0.0f, 0.0f),
      m_collisionRadius(12.0f),
      m_active(false)
{
}

void Agent::SetPosition(const sf::Vector2f& position)
{
    m_position = position;
}

const sf::Vector2f& Agent::GetPosition() const
{
    return m_position;
}

void Agent::SetVelocity(const sf::Vector2f& velocity)
{
    m_velocity = velocity;
}

const sf::Vector2f& Agent::GetVelocity() const
{
    return m_velocity;
}

void Agent::SetCollisionRadius(float radius)
{
    m_collisionRadius = radius;
}

float Agent::GetCollisionRadius() const
{
    return m_collisionRadius;
}

sf::FloatRect Agent::GetBounds() const
{
    return sf::FloatRect(
        m_position.x - m_collisionRadius,
        m_position.y - m_collisionRadius,
        m_collisionRadius * 2.0f,
        m_collisionRadius * 2.0f);
}

void Agent::SetActive(bool active)
{
    m_active = active;
}

bool Agent::IsActive() const
{
    return m_active;
}
