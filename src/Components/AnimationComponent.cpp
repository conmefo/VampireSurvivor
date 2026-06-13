#include "AnimationComponent.h"

AnimationComponent::AnimationComponent(sf::Sprite& targetSprite)
    : m_targetSprite(targetSprite),
      m_currentFrame(0),
      m_animationSpeed(0.1f),
      m_animationTimer(0.0f)
{
}

void AnimationComponent::Initialize(const std::vector<sf::IntRect>& frames, float animationSpeed)
{
    m_frames = frames;
    m_animationSpeed = animationSpeed;
    
    Reset();
}

void AnimationComponent::Update(float dt)
{
    if(m_frames.size() <= 1)
    {
        return;
    }

    m_animationTimer += dt;
    if(m_animationTimer >= m_animationSpeed)
    {
        m_animationTimer -= m_animationSpeed;
        m_currentFrame = (m_currentFrame + 1) % m_frames.size();

        m_targetSprite.setTextureRect(m_frames[m_currentFrame]);
    }
}

void AnimationComponent::Reset()
{
    m_currentFrame = 0;
    m_animationTimer = 0.0f;
    if(!m_frames.empty())
    {
        m_targetSprite.setTextureRect(m_frames[0]);
    }
}
