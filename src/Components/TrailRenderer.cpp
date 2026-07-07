#include "TrailRenderer.h"
#include <cmath>
#include <algorithm>

TrailRenderer::TrailRenderer(float maxLifetime, float startWidth, float minVertexDistance)
    : m_maxLifetime(maxLifetime)
    , m_startWidth(startWidth)
    , m_minVertexDistanceSq(minVertexDistance * minVertexDistance)
{
}

void TrailRenderer::Update(float dt, const sf::Vector2f& currentPosition)
{
    // Update existing points
    for (auto it = m_points.begin(); it != m_points.end(); )
    {
        it->lifetime -= dt;
        if (it->lifetime <= 0.0f)
        {
            it = m_points.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // Add new point if distance threshold is met
    if (m_points.empty())
    {
        m_points.push_front({currentPosition, m_maxLifetime, m_maxLifetime});
    }
    else
    {
        sf::Vector2f diff = currentPosition - m_points.front().position;
        float distSq = diff.x * diff.x + diff.y * diff.y;
        if (distSq >= m_minVertexDistanceSq)
        {
            m_points.push_front({currentPosition, m_maxLifetime, m_maxLifetime});
        }
        else
        {
            // Just update the front point to smoothly track the projectile
            m_points.front().position = currentPosition;
            m_points.front().lifetime = m_maxLifetime;
        }
    }
}

void TrailRenderer::SetBaseColor(const sf::Color& color)
{
    m_baseColor = color;
}

void TrailRenderer::SetFadeStartRatio(float ratio)
{
    m_fadeStartRatio = std::max(0.0f, std::min(1.0f, ratio));
}

void TrailRenderer::SetMaxLifetime(float maxLifetime)
{
    m_maxLifetime = std::max(0.01f, maxLifetime);
}

void TrailRenderer::SetWidth(float startWidth)
{
    m_startWidth = startWidth;
}

void TrailRenderer::Clear()
{
    m_points.clear();
}

sf::Vector2f TrailRenderer::GetPerpendicular(const sf::Vector2f& p1, const sf::Vector2f& p2) const
{
    sf::Vector2f diff = p2 - p1;
    float length = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    if (length == 0.0f) return sf::Vector2f(0.0f, 0.0f);
    
    return sf::Vector2f(-diff.y / length, diff.x / length);
}

void TrailRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (m_points.size() < 2) return;

    sf::VertexArray va(sf::TriangleStrip, m_points.size() * 2);

    for (size_t i = 0; i < m_points.size(); ++i)
    {
        const TrailPoint& pt = m_points[i];
        
        // Calculate age progress (0.0 = newborn, 1.0 = dead)
        float progress = 1.0f - (pt.lifetime / pt.maxLifetime);
        
        // Constant width
        float currentWidth = m_startWidth;
        
        // Calculate alpha fade
        float baseAlpha = static_cast<float>(m_baseColor.a);
        float alpha = baseAlpha;
        if (progress > m_fadeStartRatio && m_fadeStartRatio < 1.0f)
        {
            float fadeProgress = (progress - m_fadeStartRatio) / (1.0f - m_fadeStartRatio);
            fadeProgress = std::max(0.0f, std::min(1.0f, fadeProgress));
            alpha = baseAlpha * (1.0f - fadeProgress);
        }
        else if (progress > 1.0f)
        {
            alpha = 0.0f;
        }
        
        sf::Color currentColor = m_baseColor;
        currentColor.a = static_cast<sf::Uint8>(alpha);
        
        // Calculate perpendicular normal for extrusion
        sf::Vector2f normal(0.0f, 1.0f);
        if (i == 0)
        {
            normal = GetPerpendicular(m_points[i].position, m_points[i+1].position);
        }
        else if (i == m_points.size() - 1)
        {
            normal = GetPerpendicular(m_points[i-1].position, m_points[i].position);
        }
        else
        {
            // Average normal between two segments
            sf::Vector2f n1 = GetPerpendicular(m_points[i-1].position, m_points[i].position);
            sf::Vector2f n2 = GetPerpendicular(m_points[i].position, m_points[i+1].position);
            normal = sf::Vector2f((n1.x + n2.x) * 0.5f, (n1.y + n2.y) * 0.5f);
            
            // Re-normalize
            float length = std::sqrt(normal.x * normal.x + normal.y * normal.y);
            if (length > 0.0f)
            {
                normal.x /= length;
                normal.y /= length;
            }
        }
        
        sf::Vector2f offset = normal * (currentWidth * 0.5f);
        
        va[i * 2].position = pt.position + offset;
        va[i * 2].color = currentColor;
        
        va[i * 2 + 1].position = pt.position - offset;
        va[i * 2 + 1].color = currentColor;
    }

    target.draw(va, states);
}
