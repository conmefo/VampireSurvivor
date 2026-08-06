#pragma once

#include "Agent.h"
#include "../Components/AnimationComponent.h"
#include "../Core/Data/CharacterProfile.h"
#include "Weapons/WeaponInventory.h"
#include "../Core/Animation/Tweener.h"
#include <functional>

extern float g_PlayerSpeedMultiplier;

class Player : public Agent
{
public:
    Player(const CharacterProfile& profile, const sf::Texture& texture, const std::vector<sf::IntRect>& frames);
    ~Player() override = default;

    void Update(float dt) override;
    void Draw(sf::RenderTarget& target) override;

    void TakeDamage(float amount);
    void Heal(float amount);
    void AddExperience(float amount);
    void GrantInvulnerability(float duration);
    
    float GetCurrentHealth() const { return m_currentHealth; }
    float GetMaxHealth() const { return m_maxHealth; }
    float GetExperience() const { return m_experience; }
    int GetLevel() const { return m_level; }
    float GetTargetExperience() const;
    float GetExpProgressRatio() const;
    bool IsDead() const { return m_isDead; }
    
    void Revive();

    void ApplyGlobalBuffs(const class PlayerProgressionManager& progression, const class PowerUpDataManager& powerUpData);

    float GetMightMultiplier() const { return 1.0f + m_mightBuff; }
    float GetAreaMultiplier() const { return 1.0f + m_areaBuff; }
    float GetDurationMultiplier() const { return 1.0f + m_durationBuff; }
    float GetCooldownMultiplier() const { return 1.0f - m_cooldownBuff; }
    int GetBonusAmount() const { return m_amountBuff; }
    float GetMoveSpeedMultiplier() const { return m_moveSpeedMultiplier; }
    float GetMagnetRadius() const { return m_baseMagnetRadius * GetMagnetMultiplier(); }
    float GetMagnetMultiplier() const { return m_magnetMultiplier; }
    void AddMagnetBonus(float bonus) { m_magnetMultiplier *= (1.0f + bonus); }
    void AddAttractorbLevel(float bonus) { AddMagnetBonus(bonus); }
    void SetMagnetMultiplier(float mult) { m_magnetMultiplier = mult; }

    void SetOnHitVfxCallback(std::function<void(const std::string&, sf::Vector2f)> callback) { m_onHitVfxCallback = std::move(callback); }
    void SetOnLevelUpCallback(std::function<void()> callback) { m_onLevelUpCallback = std::move(callback); }

    WeaponInventory& GetWeaponInventory() { return m_weaponInventory; }
    sf::Vector2f GetFacingDirection() const { return m_currentDirection; }
    sf::Vector2f GetBottomPosition() const {
        sf::FloatRect bounds = m_sprite.getGlobalBounds();
        return sf::Vector2f(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height);
    }
    sf::Vector2f GetCenterPosition() const {
        sf::FloatRect bounds = m_sprite.getGlobalBounds();
        return sf::Vector2f(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
    }

private:
    void OnHpReachedZero();

    static constexpr float ANIMATION_SPEED = 0.15f;
    static constexpr float BASE_MOVE_SPEED = 150.0f;
    static constexpr float I_FRAME_DURATION = 0.5f;
    static constexpr float HIT_FLASH_DURATION = 0.1f;

    struct TrailNode
    {
        sf::Sprite sprite;
        float alpha;
    };

    sf::Sprite m_sprite;
    AnimationComponent m_animator;
    float m_moveSpeedMultiplier;
    
    float m_baseScaleX = 1.0f;
    float m_baseScaleY = 1.0f;
    
    sf::Vector2f m_currentDirection;
    bool m_isMoving = false;
    WeaponInventory m_weaponInventory;

    float m_currentHealth = 100.0f;
    float m_maxHealth = 100.0f;
    float m_experience = 0.0f;
    int m_level = 1;
    float m_invulnTimer = 0.0f;
    float m_flashTimer = 0.0f;
    bool m_isDead = false;

    Tweener m_deathScaleXTweener;
    Tweener m_deathScaleYTweener;
    Tweener m_deathColorTweener;

    float m_mightBuff = 0.0f;
    float m_areaBuff = 0.0f;
    float m_durationBuff = 0.0f;
    float m_cooldownBuff = 0.0f;
    int m_amountBuff = 0;
    float m_baseMagnetRadius = 96.0f;
    float m_magnetMultiplier = 1.0f;

    std::function<void(const std::string&, sf::Vector2f)> m_onHitVfxCallback;
    std::function<void()> m_onLevelUpCallback;
};
