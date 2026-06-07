#pragma once

class Spring
{
public:
    Spring(float tension = 100.0f, float damping = 10.0f, float startValue = 0.0f);
    ~Spring() = default;

    void Update(float dt);

    void SetTarget(float target);
    void SetValue(float value);
    void SetVelocity(float velocity);
    void SetTension(float tension);
    void SetDamping(float damping);
    void SetRestThreshold(float threshold);

    float GetValue() const { return m_currentValue; }
    float GetVelocity() const { return m_currentVelocity; }
    float GetTarget() const { return m_targetValue; }
    float GetTension() const { return m_tension; }
    float GetDamping() const { return m_damping; }
    bool IsAtRest() const { return m_isAtRest; }

private:
    float m_tension;
    float m_damping;
    float m_targetValue;
    float m_currentValue;
    float m_currentVelocity;
    float m_restThreshold;
    bool m_isAtRest;
};
