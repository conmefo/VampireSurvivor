#pragma once

#include "../Agent.h"
#include "EnemyType.h"

#include <string>

struct EnemyStats
{
    float maxHealth = 10.0f;
    float speed = 60.0f;
    float damage = 1.0f;
    float mass = 1.0f;
    float collisionRadius = 14.0f;
    int expYield = 1;
};

class EnemyBase : public Agent
{
public:
    explicit EnemyBase(EnemyType type = EnemyType::Basic);
    EnemyBase(EnemyType type, std::string definitionId);
    ~EnemyBase() override = default;

    virtual void Activate(const sf::Vector2f& position, const EnemyStats& stats);
    virtual void Deactivate();

    void Update(float dt) override;
    virtual void Draw(sf::RenderTarget& target) override;

    void SetTarget(const sf::Vector2f& targetPosition);
    const sf::Vector2f& GetTarget() const;

    void TakeDamage(float damage);
    void ApplyKnockback(const sf::Vector2f& force);

    bool IsAlive() const;
    bool IsDead() const;

    float GetHealth() const;
    float GetMaxHealth() const;
    float GetDamage() const;
    float GetMass() const;
    int GetExpYield() const;
    EnemyType GetType() const;
    const std::string& GetDefinitionId() const;

protected:
    virtual void UpdateAI(float dt);
    void SyncBodyToPosition();

    sf::Vector2f m_targetPosition;
    sf::CircleShape m_body;
    EnemyStats m_stats;
    float m_health;
    EnemyType m_type;
    std::string m_definitionId;
};
