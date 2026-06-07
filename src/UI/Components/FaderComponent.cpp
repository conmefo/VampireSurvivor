#include "FaderComponent.h"

FaderComponent::FaderComponent()
    : m_fadeRatio(1.0f)
    , m_targetRatio(1.0f)
    , m_transitionSpeed(0.0f)
    , m_isFading(false)
{
}

void FaderComponent::SetDuration(float seconds)
{
    if(seconds > 0.0f)
    {
        m_transitionSpeed = 1.0f / seconds;
    }
    else
    {
        m_transitionSpeed = 0.0f;
    }
}

void FaderComponent::FadeIn()
{
    m_targetRatio = 1.0f;
    m_isFading = true;
}

void FaderComponent::FadeOut()
{
    m_targetRatio = 0.0f;
    m_isFading = true;
}

void FaderComponent::FadeToRatio(float ratio)
{
    if(ratio < 0.0f)
    {
        m_targetRatio = 0.0f;
    }
    else if(ratio > 1.0f)
    {
        m_targetRatio = 1.0f;
    }
    else
    {
        m_targetRatio = ratio;
    }
    m_isFading = true;
}

void FaderComponent::Update(float deltaTime)
{
    if(!m_isFading)
    {
        return;
    }

    if(m_transitionSpeed <= 0.0f)
    {
        m_fadeRatio = m_targetRatio;
        m_isFading = false;
        return;
    }

    if(m_fadeRatio < m_targetRatio)
    {
        m_fadeRatio += m_transitionSpeed * deltaTime;
        if(m_fadeRatio >= m_targetRatio)
        {
            m_fadeRatio = m_targetRatio;
            m_isFading = false;
        }
    }
    else if(m_fadeRatio > m_targetRatio)
    {
        m_fadeRatio -= m_transitionSpeed * deltaTime;
        if(m_fadeRatio <= m_targetRatio)
        {
            m_fadeRatio = m_targetRatio;
            m_isFading = false;
        }
    }
    else
    {
        m_isFading = false;
    }
}

float FaderComponent::GetFadeRatio() const
{
    return m_fadeRatio;
}
