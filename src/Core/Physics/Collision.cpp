#include "Collision.h"

#include <cmath>

float Collision::Length(const sf::Vector2f &value) {
  return std::sqrt(value.x * value.x + value.y * value.y);
}

sf::Vector2f Collision::Normalize(const sf::Vector2f &value) {
  float length = Length(value);
  if (length > 0.0001f) {
    return value / length;
  }
  return sf::Vector2f(1.0f, 0.0f);
}

bool Collision::CircleIntersectsCircle(const sf::Vector2f &aPos, float aRadius,
                                     const sf::Vector2f &bPos, float bRadius) {
  float distanceSquared =
      (aPos.x - bPos.x) * (aPos.x - bPos.x) + (aPos.y - bPos.y) * (aPos.y - bPos.y);
  float radiusSum = aRadius + bRadius;
  return distanceSquared < radiusSum * radiusSum;
}

sf::Vector2f Collision::GetCircleSeparation(const sf::Vector2f &aPos, float aRadius,
                                            const sf::Vector2f &bPos, float bRadius) {
  sf::Vector2f delta = aPos - bPos;
  float distance = Length(delta);
  float radiusSum = aRadius + bRadius;

  if (distance < radiusSum) {
    float overlap = radiusSum - distance;
    return Normalize(delta) * overlap;
  }
  return sf::Vector2f(0.0f, 0.0f);
}
