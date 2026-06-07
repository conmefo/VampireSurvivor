#include "Spring.h"
#include <cmath>

Spring::Spring(float tension, float damping, float startValue)
    : m_tension(tension)
    , m_damping(damping)
    , m_targetValue(startValue)
    , m_currentValue(startValue)
    , m_currentVelocity(0.0f)
    , m_restThreshold(0.001f)
    , m_isAtRest(true)
{
}

void Spring::Update(float dt)
{
    if(m_isAtRest)
    {
        return;
    }

    float forceSpring = -m_tension * (m_currentValue - m_targetValue);
    float forceDamping = -m_damping * m_currentVelocity;
    float acceleration = forceSpring + forceDamping;

    m_currentVelocity += acceleration * dt;
    m_currentValue += m_currentVelocity * dt;

    if(std::abs(m_currentVelocity) < m_restThreshold && std::abs(m_currentValue - m_targetValue) < m_restThreshold)
    {
        m_currentValue = m_targetValue;
        m_currentVelocity = 0.0f;
        m_isAtRest = true;
    }
}

void Spring::SetTarget(float target)
{
    if(m_targetValue != target)
    {
        m_targetValue = target;
        m_isAtRest = false;
    }
}

void Spring::SetValue(float value)
{
    m_currentValue = value;
    m_isAtRest = false;
}

void Spring::SetVelocity(float velocity)
{
    m_currentVelocity = velocity;
    m_isAtRest = false;
}

void Spring::SetTension(float tension)
{
    m_tension = tension;
}

void Spring::SetDamping(float damping)
{
    m_damping = damping;
}

void Spring::SetRestThreshold(float threshold)
{
    m_restThreshold = threshold;
}
