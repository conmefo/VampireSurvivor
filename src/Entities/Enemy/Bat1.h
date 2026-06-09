#pragma once

#include "EnemyBase.h"

class Bat1 : public EnemyBase
{
public:
    Bat1();
    ~Bat1() override = default;

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
