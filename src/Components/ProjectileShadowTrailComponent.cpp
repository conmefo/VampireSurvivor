#include "ProjectileShadowTrailComponent.h"
#include <algorithm>
#include <cmath>

ProjectileShadowTrailComponent::ProjectileShadowTrailComponent(float delaySeconds, sf::Color tint)
    : m_delaySeconds(delaySeconds)
    , m_tint(tint)
{
}

void ProjectileShadowTrailComponent::SetDelay(float delaySeconds)
{
    m_delaySeconds = std::max(0.0f, delaySeconds);
}

void ProjectileShadowTrailComponent::SetTint(sf::Color tint)
{
    m_tint = tint;
}

void ProjectileShadowTrailComponent::Update(const sf::Vector2f& position, float rotation, const sf::Vector2f& scale, float dt)
{
    m_elapsedTime += dt;
    m_history.push_back({ m_elapsedTime, position, rotation, scale });

    // Keep history slightly beyond max delay to allow clean interpolation
    float maxAge = m_delaySeconds + 0.2f;
    while (!m_history.empty() && (m_elapsedTime - m_history.front().time) > maxAge)
    {
        m_history.pop_front();
    }
}

void ProjectileShadowTrailComponent::Draw(sf::RenderTarget& target, const sf::Sprite& baseSprite) const
{
    if (m_history.size() < 2 || m_delaySeconds <= 0.0001f)
    {
        return;
    }

    float targetTime = m_elapsedTime - m_delaySeconds;

    // Find bounding snapshots in history for time-based LERP
    const Snapshot* prev = nullptr;
    const Snapshot* next = nullptr;

    for (size_t i = 0; i < m_history.size() - 1; ++i)
    {
        if (m_history[i].time <= targetTime && m_history[i + 1].time >= targetTime)
        {
            prev = &m_history[i];
            next = &m_history[i + 1];
            break;
        }
    }

    if (!prev || !next)
    {
        // Fallback if targetTime is older than our stored history buffer
        prev = &m_history.front();
        next = &m_history.front();
    }

    float timeDelta = next->time - prev->time;
    float t = (timeDelta > 0.00001f) ? (targetTime - prev->time) / timeDelta : 0.0f;
    t = std::max(0.0f, std::min(1.0f, t));

    // Linear interpolation for position, rotation, and scale
    sf::Vector2f interpPos = prev->position + t * (next->position - prev->position);
    float interpRot = prev->rotation + t * (next->rotation - prev->rotation);
    sf::Vector2f interpScale = prev->scale + t * (next->scale - prev->scale);

    sf::Sprite shadowSprite = baseSprite;
    shadowSprite.setPosition(interpPos);
    shadowSprite.setRotation(interpRot);
    shadowSprite.setScale(interpScale);
    shadowSprite.setColor(m_tint);

    target.draw(shadowSprite);
}
