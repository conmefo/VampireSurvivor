#pragma once

#include <SFML/Graphics.hpp>

namespace MathUtils
{
    enum class EaseType
    {
        Sine,
        Quad,
        Cubic,
        Quart,
        Quint,
        Expo,
        Circ,
        Back,
        Elastic,
        Bounce
    };

    enum class EaseMode
    {
        In,
        Out,
        InOut
    };

    float Lerp(float a, float b, float t);
    sf::Vector2f LerpVector(const sf::Vector2f& a, const sf::Vector2f& b, float t);
    sf::Color LerpColor(const sf::Color& a, const sf::Color& b, float t);

    float Clamp(float value, float min, float max);
    float Normalize(float value, float min, float max);

    float Ease(float t, EaseType type, EaseMode mode);
}
