#pragma once

#include "../Math/MathUtils.h"
#include <functional>

class Tweener
{
public:
    Tweener(float startValue = 0.0f, float endValue = 0.0f, float duration = 1.0f,
            MathUtils::EaseType easeType = MathUtils::EaseType::Quad,
            MathUtils::EaseMode easeMode = MathUtils::EaseMode::Out);
    ~Tweener() = default;

    void Start();
    void Pause();
    void Resume();
    void Stop();

    void Update(float dt);

    void SetStartValue(float value) { m_startValue = value; }
    void SetEndValue(float value) { m_endValue = value; }
    void SetDuration(float duration) { m_duration = duration; }
    void SetEase(MathUtils::EaseType easeType, MathUtils::EaseMode easeMode)
    {
        m_easeType = easeType;
        m_easeMode = easeMode;
    }
    void SetLoop(bool loop) { m_loop = loop; }
    void SetYoyo(bool yoyo) { m_yoyo = yoyo; }

    float GetValue() const { return m_currentValue; }
    bool IsRunning() const { return m_isRunning; }
    bool IsPaused() const { return m_isPaused; }

    void OnUpdate(std::function<void(float)> callback) { m_onUpdate = callback; }
    void OnComplete(std::function<void()> callback) { m_onComplete = callback; }

private:
    float m_startValue;
    float m_endValue;
    float m_currentValue;
    float m_elapsedTime;
    float m_duration;

    MathUtils::EaseType m_easeType;
    MathUtils::EaseMode m_easeMode;

    bool m_isRunning;
    bool m_isPaused;
    bool m_loop;
    bool m_yoyo;
    bool m_forward;

    std::function<void(float)> m_onUpdate;
    std::function<void()> m_onComplete;
};
