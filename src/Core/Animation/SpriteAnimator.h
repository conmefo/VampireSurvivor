#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include "AnimationData.h"

class SpriteAnimator
{
public:
    SpriteAnimator();
    ~SpriteAnimator() = default;

    void Play(const AnimationData* anim, bool forceReset = false);
    void Update(float dt, sf::Sprite& targetSprite);
    
    void SetOnCompleteCallback(std::function<void()> callback);

private:
    const AnimationData* m_currentAnim;
    float m_elapsedTime;
    int m_currentFrame;
    bool m_isPlaying;
    std::function<void()> m_onComplete;
};
