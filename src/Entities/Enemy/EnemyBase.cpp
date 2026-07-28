#include "EnemyBase.h"

#include <algorithm>
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

EnemyBase::EnemyBase(std::string definitionId)
    : m_targetPosition(0.0f, 0.0f),
      m_health(0.0f),
      m_isDying(false),
      m_deathTimer(0.0f),
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
    m_isDying = false;
    m_deathTimer = 0.0f;
    m_knockbackResistanceReduction = 0.0f;
    m_statusResistanceReduction = 0.0f;

    m_body.setRadius(m_collisionRadius);
    m_body.setOrigin(m_collisionRadius, m_collisionRadius);
    SyncBodyToPosition();
}

void EnemyBase::Deactivate()
{
    m_active = false;
    m_isDying = false;
    m_deathTimer = 0.0f;
    m_velocity = sf::Vector2f(0.0f, 0.0f);
}

void EnemyBase::Update(float dt)
{
    if(m_isDying)
    {
        UpdateDeath(dt);
        return;
    }

    if(!IsAlive())
    {
        return;
    }

    UpdateAI(dt);
    SyncBodyToPosition();
}

void EnemyBase::Draw(sf::RenderTarget& target)
{
    if(!m_active || (!IsAlive() && !m_isDying))
    {
        return;
    }

    m_body.setFillColor(GetRenderColor());
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

bool EnemyBase::TakeDamage(float damage, const sf::Vector2f& hitDirection)
{
    if(!IsAlive() || damage <= 0.0f)
    {
        return false;
    }

    m_health -= damage;
    if(m_health <= 0.0f)
    {
        m_health = 0.0f;
        StartDeathSequence(hitDirection);
        return true;
    }

    return false;
}

void EnemyBase::ApplyKnockback(const sf::Vector2f& force)
{
    if(!IsAlive())
    {
        return;
    }

    float safeMass = m_stats.mass <= 0.0f ? 1.0f : m_stats.mass;
    // Lower mass (or multiply force) based on knockback resistance reduction (each 0.1 reduction increases force by 10%)
    float knockbackMultiplier = 1.0f + m_knockbackResistanceReduction;
    m_position += (force * knockbackMultiplier) / safeMass;
    SyncBodyToPosition();
}

bool EnemyBase::IsAlive() const
{
    return m_active && !m_isDying && m_health > 0.0f;
}

bool EnemyBase::IsDead() const
{
    return !IsAlive();
}

bool EnemyBase::IsDying() const
{
    return m_active && m_isDying;
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

float EnemyBase::GetExpYield() const
{
    return m_stats.expYield;
}

sf::Vector2f EnemyBase::GetCollisionCenter() const
{
    return m_position + m_stats.collisionOffset;
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

void EnemyBase::UpdateDeath(float dt)
{
    m_deathTimer -= dt;
    m_position += m_velocity * dt;
    m_velocity *= std::max(0.0f, 1.0f - dt * 6.0f);
    SyncBodyToPosition();

    if(m_deathTimer <= 0.0f)
    {
        Deactivate();
    }
}

void EnemyBase::SyncBodyToPosition()
{
    m_body.setPosition(GetCollisionCenter());
}

sf::Color EnemyBase::GetRenderColor() const
{
    sf::Color color(
        static_cast<sf::Uint8>((m_stats.baseTint >> 16) & 0xFF),
        static_cast<sf::Uint8>((m_stats.baseTint >> 8) & 0xFF),
        static_cast<sf::Uint8>(m_stats.baseTint & 0xFF));
    float alpha = std::clamp(m_stats.baseAlpha, 0.0f, 1.0f);
    color.a = static_cast<sf::Uint8>(255.0f * alpha);
    return color;
}

void EnemyBase::StartDeathSequence(const sf::Vector2f& hitDirection)
{
    m_isDying = true;
    m_deathTimer = FallbackDeathDurationSeconds;

    sf::Vector2f direction = Normalize(hitDirection);
    if(direction.x == 0.0f && direction.y == 0.0f)
    {
        direction = Normalize(m_position - m_targetPosition);
    }

    m_velocity = direction * std::max(0.0f, m_stats.deathKnockback) * DeathKnockbackSpeedScale;
}
