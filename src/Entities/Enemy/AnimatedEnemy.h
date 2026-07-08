#pragma once

#include "EnemyDefinition.h"

class AnimatedEnemy : public EnemyBase
{
public:
    explicit AnimatedEnemy(const EnemyDefinition& definition);
    ~AnimatedEnemy() override = default;

    void Activate(const sf::Vector2f& position, const EnemyStats& stats) override;
    void Update(float dt) override;
    void Draw(sf::RenderTarget& target) override;

private:
    void ApplyFrame();
    void ApplyDeathFrame();
    void ApplyAnimationFrame(const EnemyAnimationDefinition& animation, int frameIndex);
    void UpdateAnimation(float dt);
    void UpdateDeath(float dt) override;
    void SyncSpriteToPosition();

    const EnemyDefinition& m_definition;
    const EnemyAnimationDefinition* m_currentAnimation;
    sf::Sprite m_sprite;
    float m_animationTimer;
    int m_currentFrame;
    float m_deathAnimationTimer;
    int m_deathFrame;
};
