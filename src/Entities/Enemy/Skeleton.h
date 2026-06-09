#pragma once

#include "EnemyBase.h"

class Skeleton : public EnemyBase
{
public:
    Skeleton();
    ~Skeleton() override = default;

    static EnemyStats GetDefaultStats();

    void Activate(const sf::Vector2f& position, const EnemyStats& stats) override;
    void Update(float dt) override;
    void Draw(sf::RenderTarget& target) override;

private:
    void ConfigureSprite();
    void UpdateAnimation(float dt);
    void SyncSpriteToPosition();

    sf::Sprite m_sprite;
    float m_animationTimer;
    float m_frameDuration;
    int m_currentFrame;
    bool m_hasSprites;
};
