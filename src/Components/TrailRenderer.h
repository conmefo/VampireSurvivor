#pragma once

#include <SFML/Graphics.hpp>
#include <deque>
#include <vector>

struct TrailPoint
{
    sf::Vector2f position;
    float lifetime;
    float maxLifetime;
};

struct GradientColorKey
{
    float time; // 0.0 to 1.0
    sf::Color color;
};

class TrailRenderer : public sf::Drawable
{
public:
    TrailRenderer(float maxLifetime, float startWidth, float minVertexDistance);

    void Update(float dt, const sf::Vector2f& currentPosition);

    void SetBaseColor(const sf::Color& color);
    void SetFadeStartRatio(float ratio);
    void SetMaxLifetime(float maxLifetime);
    void SetWidth(float startWidth);
    void Clear();

protected:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    sf::Vector2f GetPerpendicular(const sf::Vector2f& p1, const sf::Vector2f& p2) const;

private:
    std::deque<TrailPoint> m_points;
    sf::Color m_baseColor = sf::Color::White;
    
    float m_fadeStartRatio = 0.5f;
    float m_maxLifetime;
    float m_startWidth;
    float m_minVertexDistanceSq;
};
