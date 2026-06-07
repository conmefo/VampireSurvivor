#pragma once

class FaderComponent
{
public:
    FaderComponent();
    ~FaderComponent() = default;

    void SetDuration(float seconds);
    void FadeIn();
    void FadeOut();
    void FadeToRatio(float ratio);
    void Update(float deltaTime);

    float GetFadeRatio() const;

private:
    float m_fadeRatio;
    float m_targetRatio;
    float m_transitionSpeed;
    bool m_isFading;
};
