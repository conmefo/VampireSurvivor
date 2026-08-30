#pragma once

#include <SFML/Graphics.hpp>

#include <memory>
#include <vector>

class Player;

class StageHazardManager
{
public:
    void Clear();
    void SpawnCircle(const sf::Vector2f& position, float radius, float duration, float damage);
    void Update(float dt, const std::vector<std::unique_ptr<Player>>& players);
    void Draw(sf::RenderTarget& target) const;

private:
    struct Hazard
    {
        sf::Vector2f position;
        float radius = 32.0f;
        float remaining = 1.0f;
        float damage = 1.0f;
        float tickTimer = 0.0f;
    };
    std::vector<Hazard> m_hazards;
};
