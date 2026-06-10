#include "EnemyBase.h"

#include <cmath>
#include <utility>

namespace
{
    sf::Vector2f Normalize(const sf::Vector2f& value)
    {
        float length = std::sqrt(value.x * value.x + value.y * value.y);
        if(length <= 0.0001f)
        {
            return sf::Vector2f(0.0f, 0.0f);
        }

        return sf::Vector2f(value.x / length, value.y / length);
    }
}

EnemyBase::EnemyBase(EnemyType type)
    : EnemyBase(type, "BASIC")
{
}

EnemyBase::EnemyBase(EnemyType type, std::string definitionId)
    : m_targetPosition(0.0f, 0.0f),
      m_health(0.0f),
      m_type(type),
      m_definitionId(std::move(definitionId))
{
    m_body.setFillColor(sf::Color(180, 45, 60));
    m_body.setOutlineColor(sf::Color(60, 10, 18));
    m_body.setOutlineThickness(2.0f);
    m_body.setRadius(m_stats.collisionRadius);
    m_body.setOrigin(m_stats.collisionRadius, m_stats.collisionRadius);
}

void EnemyBase::Activate(const sf::Vector2f& position, const EnemyStats& stats)
{
    m_stats = stats;
    m_health = m_stats.maxHealth;
    m_collisionRadius = m_stats.collisionRadius;
    m_position = position;
    m_velocity = sf::Vector2f(0.0f, 0.0f);
    m_active = true;

    m_body.setRadius(m_collisionRadius);
    m_body.setOrigin(m_collisionRadius, m_collisionRadius);
    SyncBodyToPosition();
}

void EnemyBase::Deactivate()
{
    m_active = false;
    m_velocity = sf::Vector2f(0.0f, 0.0f);
}

void EnemyBase::Update(float dt)
{
    if(!IsAlive())
    {
        return;
    }

    UpdateAI(dt);
    SyncBodyToPosition();
}

void EnemyBase::Draw(sf::RenderTarget& target)
{
    if(!IsAlive())
    {
        return;
    }

    target.draw(m_body);
}

void EnemyBase::SetTarget(const sf::Vector2f& targetPosition)
{
    m_targetPosition = targetPosition;
}

const sf::Vector2f& EnemyBase::GetTarget() const
{
    return m_targetPosition;
}

void EnemyBase::TakeDamage(float damage)
{
    if(!IsAlive() || damage <= 0.0f)
    {
        return;
    }

    m_health -= damage;
    if(m_health <= 0.0f)
    {
        m_health = 0.0f;
        Deactivate();
    }
}

void EnemyBase::ApplyKnockback(const sf::Vector2f& force)
{
    if(!IsAlive())
    {
        return;
    }

    float safeMass = m_stats.mass <= 0.0f ? 1.0f : m_stats.mass;
    m_position += force / safeMass;
    SyncBodyToPosition();
}

bool EnemyBase::IsAlive() const
{
    return m_active && m_health > 0.0f;
}

bool EnemyBase::IsDead() const
{
    return !IsAlive();
}

float EnemyBase::GetHealth() const
{
    return m_health;
}

float EnemyBase::GetMaxHealth() const
{
    return m_stats.maxHealth;
}

float EnemyBase::GetDamage() const
{
    return m_stats.damage;
}

float EnemyBase::GetMass() const
{
    return m_stats.mass;
}

int EnemyBase::GetExpYield() const
{
    return m_stats.expYield;
}

EnemyType EnemyBase::GetType() const
{
    return m_type;
}

const std::string& EnemyBase::GetDefinitionId() const
{
    return m_definitionId;
}

void EnemyBase::UpdateAI(float dt)
{
    sf::Vector2f direction = Normalize(m_targetPosition - m_position);
    m_velocity = direction * m_stats.speed;
    m_position += m_velocity * dt;
}

void EnemyBase::SyncBodyToPosition()
{
    m_body.setPosition(m_position);
}
