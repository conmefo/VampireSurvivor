#include "Tweener.h"
#include <utility>

Tweener::Tweener(float startValue, float endValue, float duration,
                 MathUtils::EaseType easeType, MathUtils::EaseMode easeMode)
    : m_startValue(startValue)
    , m_endValue(endValue)
    , m_currentValue(startValue)
    , m_elapsedTime(0.0f)
    , m_duration(duration)
    , m_easeType(easeType)
    , m_easeMode(easeMode)
    , m_isRunning(false)
    , m_isPaused(false)
    , m_loop(false)
    , m_yoyo(false)
    , m_forward(true)
    , m_onUpdate(nullptr)
    , m_onComplete(nullptr)
{
}

void Tweener::Start()
{
    m_elapsedTime = 0.0f;
    m_isRunning = true;
    m_isPaused = false;
    m_forward = true;
    m_currentValue = m_startValue;
}

void Tweener::Pause()
{
    m_isPaused = true;
}

void Tweener::Resume()
{
    if(m_isRunning)
    {
        m_isPaused = false;
    }
}

void Tweener::Stop()
{
    m_isRunning = false;
    m_isPaused = false;
}

void Tweener::Update(float dt)
{
    if(!m_isRunning || m_isPaused)
    {
        return;
    }

    if(m_duration <= 0.0f)
    {
        m_currentValue = m_endValue;
        if(m_onUpdate)
        {
            m_onUpdate(m_currentValue);
        }
        if(m_onComplete)
        {
            m_onComplete();
        }
        m_isRunning = false;
        return;
    }

    m_elapsedTime += dt;
    float t = m_elapsedTime / m_duration;

    if(t >= 1.0f)
    {
        t = 1.0f;
        float easedT = MathUtils::Ease(t, m_easeType, m_easeMode);
        m_currentValue = MathUtils::Lerp(m_startValue, m_endValue, easedT);

        if(m_onUpdate)
        {
            m_onUpdate(m_currentValue);
        }

        if(m_loop)
        {
            m_elapsedTime = 0.0f;
            if(m_yoyo)
            {
                std::swap(m_startValue, m_endValue);
                m_forward = !m_forward;
            }
            else
            {
                m_currentValue = m_startValue;
            }
        }
        else
        {
            m_isRunning = false;
            if(m_onComplete)
            {
                m_onComplete();
            }
        }
    }
    else
    {
        float easedT = MathUtils::Ease(t, m_easeType, m_easeMode);
        m_currentValue = MathUtils::Lerp(m_startValue, m_endValue, easedT);

        if(m_onUpdate)
        {
            m_onUpdate(m_currentValue);
        }
    }
}
