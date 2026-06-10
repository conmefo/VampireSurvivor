#include "Collision.h"

#include <algorithm>
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

sf::Vector2f Collision::GetCircleRectSeparation(const sf::Vector2f &circlePos,
                                                float circleRadius,
                                                const sf::FloatRect &rect) {
  const float rectRight = rect.left + rect.width;
  const float rectBottom = rect.top + rect.height;

  const bool insideX = circlePos.x >= rect.left && circlePos.x <= rectRight;
  const bool insideY = circlePos.y >= rect.top && circlePos.y <= rectBottom;

  if (insideX && insideY) {
    const float pushLeft = circlePos.x - rect.left + circleRadius;
    const float pushRight = rectRight - circlePos.x + circleRadius;
    const float pushUp = circlePos.y - rect.top + circleRadius;
    const float pushDown = rectBottom - circlePos.y + circleRadius;

    float smallestPush = pushLeft;
    sf::Vector2f separation(-pushLeft, 0.0f);

    if (pushRight < smallestPush) {
      smallestPush = pushRight;
      separation = sf::Vector2f(pushRight, 0.0f);
    }
    if (pushUp < smallestPush) {
      smallestPush = pushUp;
      separation = sf::Vector2f(0.0f, -pushUp);
    }
    if (pushDown < smallestPush) {
      separation = sf::Vector2f(0.0f, pushDown);
    }

    return separation;
  }

  const sf::Vector2f closestPoint(
      std::max(rect.left, std::min(circlePos.x, rectRight)),
      std::max(rect.top, std::min(circlePos.y, rectBottom)));
  const sf::Vector2f delta = circlePos - closestPoint;
  const float distance = Length(delta);

  if (distance >= circleRadius) {
    return sf::Vector2f(0.0f, 0.0f);
  }

  return Normalize(delta) * (circleRadius - distance);
}
