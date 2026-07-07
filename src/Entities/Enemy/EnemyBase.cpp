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
    m_hitFlash.Init(stats.baseTint);
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
    m_hitFlash.Update(dt);
    SyncBodyToPosition();
}

void EnemyBase::Draw(sf::RenderTarget& target)
{
    if(!IsAlive())
    {
        return;
    }

    m_body.setFillColor(m_hitFlash.GetCurrentColor());
    target.draw(m_body);
    DrawHealthBar(target);
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

void EnemyBase::TriggerHitFlash(const HitVfxProfile& profile)
{
    m_hitFlash.TriggerFlash(profile);
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

void EnemyBase::DrawHealthBar(sf::RenderTarget& target) const
{
    if(m_stats.maxHealth <= 0.0f)
    {
        return;
    }

    constexpr float BarHeight = 4.0f;
    const float barWidth = std::max(24.0f, m_collisionRadius * 2.2f);
    const float healthRatio = std::clamp(m_health / m_stats.maxHealth, 0.0f, 1.0f);
    const sf::Vector2f barPosition(
        std::round(m_position.x - barWidth / 2.0f),
        std::round(m_position.y - m_collisionRadius - 10.0f));

    sf::RectangleShape background(sf::Vector2f(barWidth, BarHeight));
    background.setPosition(barPosition);
    background.setFillColor(sf::Color(20, 10, 10, 190));
    background.setOutlineColor(sf::Color(0, 0, 0, 180));
    background.setOutlineThickness(1.0f);
    target.draw(background);

    sf::RectangleShape fill(sf::Vector2f(std::max(0.0f, barWidth * healthRatio), BarHeight));
    fill.setPosition(barPosition);
    fill.setFillColor(healthRatio > 0.35f ? sf::Color(80, 220, 95, 230) : sf::Color(230, 65, 55, 230));
    target.draw(fill);
}
