#include "SpriteAnimator.h"
#include <algorithm>
#include <iostream>

SpriteAnimator::SpriteAnimator()
    : m_currentAnim(nullptr)
    , m_elapsedTime(0.0f)
    , m_currentFrame(0)
    , m_speedMultiplier(1.0f)
    , m_isPlaying(false)
    , m_isPaused(false)
    , m_isFinished(false)
    , m_originMode(AnimationOriginMode::KeepExisting)
    , m_onComplete(nullptr)
    , m_onFrameChange(nullptr)
{
}

void SpriteAnimator::SetOnCompleteCallback(std::function<void()> callback)
{
    m_onComplete = std::move(callback);
}

void SpriteAnimator::SetOnFrameChangeCallback(std::function<void(int)> callback)
{
    m_onFrameChange = std::move(callback);
}

void SpriteAnimator::SetSpeedMultiplier(float speed)
{
    m_speedMultiplier = std::max(0.0f, speed);
}

void SpriteAnimator::Play(const AnimationData* anim, bool forceReset)
{
    if (!anim || anim->IsEmpty())
    {
        return;
    }

    if (m_currentAnim != anim || forceReset)
    {
        m_currentAnim = anim;
        m_elapsedTime = 0.0f;
        m_currentFrame = 0;
        m_isPlaying = true;
        m_isPaused = false;
        m_isFinished = false;
    }
}

void SpriteAnimator::Pause()
{
    if (m_isPlaying)
    {
        m_isPaused = true;
    }
}

void SpriteAnimator::Resume()
{
    if (m_isPlaying && m_isPaused)
    {
        m_isPaused = false;
    }
}

void SpriteAnimator::Stop()
{
    m_isPlaying = false;
    m_isPaused = false;
    m_isFinished = true;
    m_currentFrame = 0;
    m_elapsedTime = 0.0f;
}

void SpriteAnimator::Restart()
{
    if (m_currentAnim)
    {
        m_elapsedTime = 0.0f;
        m_currentFrame = 0;
        m_isPlaying = true;
        m_isPaused = false;
        m_isFinished = false;
    }
}

float SpriteAnimator::GetNormalizedProgress() const
{
    if (!m_currentAnim || m_currentAnim->IsEmpty())
    {
        return 0.0f;
    }

    float totalDur = m_currentAnim->GetTotalDuration();
    if (totalDur <= 0.0f)
    {
        return 1.0f;
    }

    float currentProgress = (m_currentFrame * m_currentAnim->frameDuration) + m_elapsedTime;
    float norm = currentProgress / totalDur;
    return std::clamp(norm, 0.0f, 1.0f);
}

void SpriteAnimator::ApplyFrame(sf::Sprite& targetSprite)
{
    if (!m_currentAnim || m_currentFrame < 0 || m_currentFrame >= static_cast<int>(m_currentAnim->frames.size()))
    {
        return;
    }

    const auto& frameData = m_currentAnim->frames[m_currentFrame];
    if (frameData.texture)
    {
        targetSprite.setTexture(*frameData.texture);
        targetSprite.setTextureRect(frameData.rect);

        if (m_originMode == AnimationOriginMode::CenterEachFrame)
        {
            targetSprite.setOrigin(frameData.rect.width / 2.0f, frameData.rect.height / 2.0f);
        }
        else if (m_originMode == AnimationOriginMode::FixedFirstFrameCenter)
        {
            const auto& f0 = m_currentAnim->frames[0];
            targetSprite.setOrigin(f0.rect.width / 2.0f, f0.rect.height / 2.0f);
        }
        else if (m_originMode == AnimationOriginMode::BottomCenterEachFrame)
        {
            targetSprite.setOrigin(frameData.rect.width / 2.0f, static_cast<float>(frameData.rect.height));
        }
    }
}

void SpriteAnimator::Update(float dt, sf::Sprite& targetSprite)
{
    if (!m_isPlaying || m_isPaused || !m_currentAnim || m_currentAnim->IsEmpty())
    {
        return;
    }

    // Apply current frame initially if target sprite texture is uninitialized
    if (targetSprite.getTexture() == nullptr)
    {
        ApplyFrame(targetSprite);
    }

    float effectiveDt = dt * m_speedMultiplier;
    m_elapsedTime += effectiveDt;

    if (m_elapsedTime >= m_currentAnim->frameDuration)
    {
        int initialFrame = m_currentFrame;

        while (m_elapsedTime >= m_currentAnim->frameDuration && m_currentAnim->frameDuration > 0.0f)
        {
            m_elapsedTime -= m_currentAnim->frameDuration;
            m_currentFrame++;
        }

        if (m_currentFrame >= static_cast<int>(m_currentAnim->frames.size()))
        {
            if (m_currentAnim->isLooping)
            {
                m_currentFrame = m_currentFrame % m_currentAnim->frames.size();
            }
            else
            {
                m_currentFrame = static_cast<int>(m_currentAnim->frames.size()) - 1;
                m_isPlaying = false;
                m_isFinished = true;
            }
        }

        if (m_currentFrame != initialFrame)
        {
            ApplyFrame(targetSprite);

            if (m_onFrameChange)
            {
                m_onFrameChange(m_currentFrame);
            }
        }

        if (m_isFinished && m_onComplete)
        {
            m_onComplete();
        }
    }
}
