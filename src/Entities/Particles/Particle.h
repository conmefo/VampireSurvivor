#pragma once

#include <SFML/Graphics.hpp>

namespace vs
{

struct Particle
{
    sf::Vector2f position;
    sf::Vector2f velocity;
    float lifetime = 0.0f;
    float maxLifetime = 1.0f;
    sf::Color color = sf::Color::White;
    float size = 1.0f;
    float initialSize = 1.0f;
    float rotation = 0.0f;
    float rotationSpeed = 0.0f;
    sf::IntRect textureRect; // The sprite slice for this particle

    sf::Vector2f offsetDir;
    float offsetDist = 0.0f;
    
    // Returns true if the particle is still alive
    bool IsAlive() const { return lifetime < maxLifetime; }
    
    // Returns 0.0 at start, 1.0 at end of life
    float GetNormalizedLifetime() const
    {
        if (maxLifetime <= 0.0f) return 1.0f;
        return lifetime / maxLifetime;
    }
};

} // namespace vs
