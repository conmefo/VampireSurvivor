#include "StageHazardManager.h"

#include "../Player.h"

#include <algorithm>

void StageHazardManager::Clear()
{
    m_hazards.clear();
}

void StageHazardManager::SpawnCircle(const sf::Vector2f& position, float radius, float duration, float damage)
{
    m_hazards.push_back({position, radius, duration, damage, 0.0f});
}

void StageHazardManager::Update(float dt, const std::vector<std::unique_ptr<Player>>& players)
{
    for(Hazard& hazard : m_hazards)
    {
        hazard.remaining -= dt;
        hazard.tickTimer -= dt;
        if(hazard.tickTimer > 0.0f) continue;
        hazard.tickTimer = 0.5f;
        for(const auto& player : players)
        {
            if(!player || player->IsDead()) continue;
            const sf::Vector2f difference = player->GetCenterPosition() - hazard.position;
            const float hitRadius = player->GetCollisionRadius() + hazard.radius;
            if(difference.x * difference.x + difference.y * difference.y <= hitRadius * hitRadius)
            {
                player->TakeDamage(hazard.damage);
            }
        }
    }
    m_hazards.erase(
        std::remove_if(m_hazards.begin(), m_hazards.end(), [](const Hazard& hazard) { return hazard.remaining <= 0.0f; }),
        m_hazards.end());
}

void StageHazardManager::Draw(sf::RenderTarget& target) const
{
    for(const Hazard& hazard : m_hazards)
    {
        sf::CircleShape visual(hazard.radius);
        visual.setOrigin(hazard.radius, hazard.radius);
        visual.setPosition(hazard.position);
        visual.setFillColor(sf::Color(230, 55, 35, 72));
        visual.setOutlineColor(sf::Color(255, 165, 55, 210));
        visual.setOutlineThickness(2.0f);
        target.draw(visual);
    }
}
