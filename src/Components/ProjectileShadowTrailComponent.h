#pragma once

#include <SFML/Graphics.hpp>
#include <deque>

class ProjectileShadowTrailComponent
{
public:
    struct Snapshot
    {
        float time;
        sf::Vector2f position;
        float rotation;
        sf::Vector2f scale;
    };

    ProjectileShadowTrailComponent(float delaySeconds = 0.05f, sf::Color tint = sf::Color(15, 20, 140, 210));
    ~ProjectileShadowTrailComponent() = default;

    void SetDelay(float delaySeconds);
    void SetTint(sf::Color tint);

    void Update(const sf::Vector2f& position, float rotation, const sf::Vector2f& scale, float dt);
    void Draw(sf::RenderTarget& target, const sf::Sprite& baseSprite) const;

private:
    std::deque<Snapshot> m_history;
    float m_elapsedTime = 0.0f;
    float m_delaySeconds;
    sf::Color m_tint;
};
