#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include "AnimationData.h"

enum class AnimationOriginMode
{
    KeepExisting,         // Preserve target sprite's origin set by calling entity/view (Default)
    CenterEachFrame,      // Recalculate (width/2, height/2) on every frame step
    FixedFirstFrameCenter, // Lock origin to frame 0's center (width/2, height/2)
    BottomCenterEachFrame  // Recalculate (width/2, height) on every frame step
};

class SpriteAnimator
{
public:
    SpriteAnimator();
    ~SpriteAnimator() = default;

    // Playback control
    void Play(const AnimationData* anim, bool forceReset = false);
    void Pause();
    void Resume();
    void Stop();
    void Restart();

    // Configuration
    void SetSpeedMultiplier(float speed);
    float GetSpeedMultiplier() const { return m_speedMultiplier; }

    void SetOriginMode(AnimationOriginMode mode) { m_originMode = mode; }
    AnimationOriginMode GetOriginMode() const { return m_originMode; }

    // Backward compatibility helper
    void SetAutoCenterOrigin(bool autoCenter)
    {
        m_originMode = autoCenter ? AnimationOriginMode::CenterEachFrame : AnimationOriginMode::KeepExisting;
    }

    void SetOnCompleteCallback(std::function<void()> callback);
    void SetOnFrameChangeCallback(std::function<void(int frameIndex)> callback);

    // Update pass
    void Update(float dt, sf::Sprite& targetSprite);

    // Queries
    bool IsPlaying() const { return m_isPlaying && !m_isPaused; }
    bool IsPaused() const { return m_isPaused; }
    bool IsFinished() const { return m_isFinished; }
    int GetCurrentFrameIndex() const { return m_currentFrame; }
    float GetNormalizedProgress() const;
    const AnimationData* GetCurrentAnimation() const { return m_currentAnim; }

private:
    void ApplyFrame(sf::Sprite& targetSprite);

    const AnimationData* m_currentAnim = nullptr;
    float m_elapsedTime = 0.0f;
    int m_currentFrame = 0;
    float m_speedMultiplier = 1.0f;
    bool m_isPlaying = false;
    bool m_isPaused = false;
    bool m_isFinished = false;
    AnimationOriginMode m_originMode = AnimationOriginMode::KeepExisting;

    std::function<void()> m_onComplete;
    std::function<void(int)> m_onFrameChange;
};
