#include "ScreenTransitionManager.h"
#include <algorithm>

void ScreenTransitionManager::StartTransition(float duration, sf::Color targetColor, std::function<void()> callbackFunction)
{
    m_transitionDuration = duration;
    m_transitionColor = targetColor;
    m_onOpaqueCallback = std::move(callbackFunction);
    
    m_phase = TransitionPhase::FadingOut;
    m_fadeRatio = 0.0f;
}

void ScreenTransitionManager::ProceedToFadeIn()
{
    if(m_phase == TransitionPhase::Swapping)
    {
        m_phase = TransitionPhase::FadingIn;
    }
}

void ScreenTransitionManager::Update(float deltaTime)
{
    if(m_phase == TransitionPhase::Idle || m_phase == TransitionPhase::Swapping)
    {
        return;
    }

    if(m_transitionDuration <= 0.0f)
    {
        if(m_phase == TransitionPhase::FadingOut)
        {
            m_fadeRatio = 1.0f;
            m_phase = TransitionPhase::Swapping;
            if(m_onOpaqueCallback)
            {
                m_onOpaqueCallback();
            }
        }
        else if(m_phase == TransitionPhase::FadingIn)
        {
            m_fadeRatio = 0.0f;
            m_phase = TransitionPhase::Idle;
        }
        return;
    }

    if(m_phase == TransitionPhase::FadingOut)
    {
        m_fadeRatio += deltaTime / m_transitionDuration;
        if(m_fadeRatio >= 1.0f)
        {
            m_fadeRatio = 1.0f;
            m_phase = TransitionPhase::Swapping;
            if(m_onOpaqueCallback)
            {
                m_onOpaqueCallback();
            }
        }
    }
    else if(m_phase == TransitionPhase::FadingIn)
    {
        m_fadeRatio -= deltaTime / m_transitionDuration;
        if(m_fadeRatio <= 0.0f)
        {
            m_fadeRatio = 0.0f;
            m_phase = TransitionPhase::Idle;
        }
    }
}

void ScreenTransitionManager::Draw(sf::RenderTarget& renderTarget)
{
    if(m_phase == TransitionPhase::Idle)
    {
        return;
    }

    sf::View view = renderTarget.getView();
    sf::RectangleShape overlay;
    overlay.setSize(view.getSize());
    overlay.setPosition(view.getCenter() - view.getSize() / 2.0f);

    sf::Color color = m_transitionColor;
    const float MAX_ALPHA = 255.0f;
    color.a = static_cast<sf::Uint8>(std::clamp(m_fadeRatio * MAX_ALPHA, 0.0f, MAX_ALPHA));
    overlay.setFillColor(color);

    renderTarget.draw(overlay);
}

bool ScreenTransitionManager::IsTransitioning() const
{
    return m_phase != TransitionPhase::Idle;
}
