#include "SpriteAnimator.h"
#include <iostream>

SpriteAnimator::SpriteAnimator()
    : m_currentAnim(nullptr)
    , m_elapsedTime(0.0f)
    , m_currentFrame(0)
    , m_isPlaying(false)
    , m_onComplete(nullptr)
{
}

void SpriteAnimator::SetOnCompleteCallback(std::function<void()> callback)
{
    m_onComplete = std::move(callback);
}

void SpriteAnimator::Play(const AnimationData* anim, bool forceReset)
{
    if(!anim)
    {
        return;
    }

    if(m_currentAnim != anim || forceReset)
    {
        m_currentAnim = anim;
        m_elapsedTime = 0.0f;
        m_currentFrame = 0;
        m_isPlaying = true;
    }
}

void SpriteAnimator::Update(float dt, sf::Sprite& targetSprite)
{
    if(!m_isPlaying || !m_currentAnim || m_currentAnim->frames.empty())
    {
        return;
    }

    m_elapsedTime += dt;

    if(m_elapsedTime >= m_currentAnim->frameDuration)
    {
        // Handle frames dropping if lag occurs
        while(m_elapsedTime >= m_currentAnim->frameDuration)
        {
            m_elapsedTime -= m_currentAnim->frameDuration;
            m_currentFrame++;
        }

        if(m_currentFrame >= static_cast<int>(m_currentAnim->frames.size()))
        {
            if(m_currentAnim->isLooping)
            {
                m_currentFrame = m_currentFrame % m_currentAnim->frames.size();
            }
            else
            {
                m_currentFrame = static_cast<int>(m_currentAnim->frames.size()) - 1;
                m_isPlaying = false;
                
                if(m_onComplete)
                {
                    m_onComplete();
                }
            }
        }
    }

    if(m_currentFrame < static_cast<int>(m_currentAnim->frames.size()))
    {
        const AssetTextureData& frameData = m_currentAnim->frames[m_currentFrame];
        if(frameData.texture)
        {
            targetSprite.setTexture(*frameData.texture);
            targetSprite.setTextureRect(frameData.rect);
        }
    }
}
