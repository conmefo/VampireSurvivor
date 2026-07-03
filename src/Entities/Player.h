#pragma once

#include "Agent.h"
#include "../Components/AnimationComponent.h"
#include "../Core/Data/CharacterProfile.h"
#include "Weapons/WeaponInventory.h"
#include "../Core/Animation/Tweener.h"
#include <functional>

class Player : public Agent
{
public:
    Player(const CharacterProfile& profile, const sf::Texture& texture, const std::vector<sf::IntRect>& frames);
    ~Player() override = default;

    void Update(float dt) override;
    void Draw(sf::RenderTarget& target) override;

    void TakeDamage(float amount);
    void Heal(float amount);
    
    float GetCurrentHealth() const { return m_currentHealth; }
    float GetMaxHealth() const { return m_maxHealth; }
    bool IsDead() const { return m_isDead; }
    
    void Revive();

    void SetOnHitVfxCallback(std::function<void(const std::string&, sf::Vector2f)> callback) { m_onHitVfxCallback = std::move(callback); }

    WeaponInventory& GetWeaponInventory() { return m_weaponInventory; }
    sf::Vector2f GetFacingDirection() const { return m_currentDirection; }

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
    WeaponInventory m_weaponInventory;

    float m_currentHealth = 100.0f;
    float m_maxHealth = 100.0f;
    float m_invulnTimer = 0.0f;
    float m_flashTimer = 0.0f;
    bool m_isDead = false;

    Tweener m_deathScaleXTweener;
    Tweener m_deathScaleYTweener;
    Tweener m_deathColorTweener;

    std::function<void(const std::string&, sf::Vector2f)> m_onHitVfxCallback;
};
