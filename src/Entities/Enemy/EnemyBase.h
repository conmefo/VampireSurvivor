#pragma once

#include "../Agent.h"

#include <string>

struct EnemyStats
{
    float maxHealth = 10.0f;
    float speed = 60.0f;
    float damage = 1.0f;
    float mass = 1.0f;
    float collisionRadius = 14.0f;
    sf::Vector2f collisionOffset = sf::Vector2f(0.0f, 0.0f);
    float deathKnockback = 2.0f;
    float baseAlpha = 1.0f;
    float expYield = 1.0f;
    int baseTint = 16777215;

    // Optional combat profile. A zero attack range keeps the default chase/contact behavior.
    bool isRanged = false;
    float attackRange = 0.0f;
    float attackCooldown = 0.0f;
    float attackTelegraph = 0.65f;
    float projectileSpeed = 220.0f;
    float projectileDamage = 0.0f;
    float projectileLifetime = 6.0f;
    float projectileRadius = 7.0f;
};

class EnemyBase : public Agent
{
public:
    explicit EnemyBase(std::string definitionId = "BASIC");
    ~EnemyBase() override = default;

    virtual void Activate(const sf::Vector2f& position, const EnemyStats& stats);
    virtual void Deactivate();

    void Update(float dt) override;
    virtual void Draw(sf::RenderTarget& target) override;

    void SetTarget(const sf::Vector2f& targetPosition);
    const sf::Vector2f& GetTarget() const;

    bool TakeDamage(float damage, const sf::Vector2f& hitDirection = sf::Vector2f(0.0f, 0.0f));
    void ApplyKnockback(const sf::Vector2f& force);

    bool IsAlive() const;
    bool IsDead() const;
    bool IsDying() const;

    float GetHealth() const;
    float GetMaxHealth() const;
    float GetDamage() const;
    float GetMass() const;
    float GetExpYield() const;
    sf::Vector2f GetCollisionCenter() const;
    const std::string& GetDefinitionId() const;

    bool HasRangedAttack() const { return m_stats.isRanged && m_stats.attackCooldown > 0.0f; }
    float GetAttackRange() const { return m_stats.attackRange; }
    float GetAttackCooldown() const { return m_stats.attackCooldown; }
    float GetAttackTelegraph() const { return m_stats.attackTelegraph; }
    float GetProjectileSpeed() const { return m_stats.projectileSpeed; }
    float GetProjectileDamage() const { return m_stats.projectileDamage > 0.0f ? m_stats.projectileDamage : m_stats.damage; }
    float GetProjectileLifetime() const { return m_stats.projectileLifetime; }
    float GetProjectileRadius() const { return m_stats.projectileRadius; }

    // Modifiers for Garlic effects
    void ReduceKnockbackResistance(float amount) { m_knockbackResistanceReduction += amount; }
    void ReduceStatusResistance(float amount) { m_statusResistanceReduction += amount; }
    float GetKnockbackResistanceReduction() const { return m_knockbackResistanceReduction; }
    float GetStatusResistanceReduction() const { return m_statusResistanceReduction; }

protected:
    virtual void UpdateAI(float dt);
    virtual void UpdateDeath(float dt);
    void SyncBodyToPosition();
    sf::Color GetRenderColor() const;
    void StartDeathSequence(const sf::Vector2f& hitDirection);

    static constexpr float FallbackDeathDurationSeconds = 0.35f;
    static constexpr float DeathKnockbackSpeedScale = 70.0f;

    sf::Vector2f m_targetPosition;
    sf::CircleShape m_body;
    EnemyStats m_stats;
    float m_health;
    bool m_isDying;
    float m_deathTimer;
    std::string m_definitionId;

    float m_knockbackResistanceReduction = 0.0f;
    float m_statusResistanceReduction = 0.0f;
};
