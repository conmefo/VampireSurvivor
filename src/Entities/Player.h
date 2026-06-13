#pragma once

#include "Agent.h"
#include "../Components/AnimationComponent.h"
#include "../Core/Data/CharacterProfile.h"

class Player : public Agent
{
public:
    Player(const CharacterProfile& profile, const sf::Texture& texture, const std::vector<sf::IntRect>& frames);
    ~Player() override = default;

    void Update(float dt) override;
    void Draw(sf::RenderTarget& target) override;

private:
    static constexpr float ANIMATION_SPEED = 0.15f;
    static constexpr float BASE_MOVE_SPEED = 150.0f;

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
};
