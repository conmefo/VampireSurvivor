#pragma once

#include <SFML/Graphics.hpp>

namespace Collision {
float Length(const sf::Vector2f &value);
sf::Vector2f Normalize(const sf::Vector2f &value);

bool CircleIntersectsCircle(const sf::Vector2f &aPos, float aRadius,
                            const sf::Vector2f &bPos, float bRadius);

sf::Vector2f GetCircleSeparation(const sf::Vector2f &aPos, float aRadius,
                                 const sf::Vector2f &bPos, float bRadius);

sf::Vector2f GetCircleRectSeparation(const sf::Vector2f &circlePos,
                                     float circleRadius,
                                     const sf::FloatRect &rect);
} // namespace Collision
