#pragma once

#include <SFML/Graphics.hpp>

class AnimationComponent
{
public:
    AnimationComponent(sf::Sprite& targetSprite);
    ~AnimationComponent() = default;

    void Initialize(const std::vector<sf::IntRect>& frames, float animationSpeed);
    void Update(float dt);
    void Reset();

private:
    sf::Sprite& m_targetSprite;
    std::vector<sf::IntRect> m_frames;
    int m_currentFrame;
    float m_animationSpeed;
    float m_animationTimer;
};
