#pragma once

#include <SFML/Graphics.hpp>
#include <functional>

enum class TransitionPhase
{
    Idle,
    FadingOut,
    Swapping,
    FadingIn
};

class ScreenTransitionManager
{
public:
    ScreenTransitionManager() = default;
    ~ScreenTransitionManager() = default;

    void StartTransition(float duration, sf::Color targetColor, std::function<void()> callbackFunction);
    void ProceedToFadeIn();
    
    void Update(float deltaTime);
    void Draw(sf::RenderTarget& renderTarget);

    bool IsTransitioning() const;

private:
    TransitionPhase m_phase = TransitionPhase::Idle;
    sf::Color m_transitionColor = sf::Color::Black;
    float m_transitionDuration = 0.0f;
    float m_currentTimer = 0.0f;
    float m_fadeRatio = 0.0f;
    std::function<void()> m_onOpaqueCallback;
};
