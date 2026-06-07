#include "MathUtils.h"
#include <cmath>

namespace MathUtils
{
    const float PI = 3.1415926535f;

    float Lerp(float a, float b, float t)
    {
        return a + (b - a) * t;
    }

    sf::Vector2f LerpVector(const sf::Vector2f& a, const sf::Vector2f& b, float t)
    {
        return a + (b - a) * t;
    }

    sf::Color LerpColor(const sf::Color& a, const sf::Color& b, float t)
    {
        sf::Uint8 r = static_cast<sf::Uint8>(a.r + (b.r - a.r) * t);
        sf::Uint8 g = static_cast<sf::Uint8>(a.g + (b.g - a.g) * t);
        sf::Uint8 bVal = static_cast<sf::Uint8>(a.b + (b.b - a.b) * t);
        sf::Uint8 alpha = static_cast<sf::Uint8>(a.a + (b.a - a.a) * t);
        return sf::Color(r, g, bVal, alpha);
    }

    float Clamp(float value, float min, float max)
    {
        if(value < min)
        {
            return min;
        }
        if(value > max)
        {
            return max;
        }
        return value;
    }

    float Normalize(float value, float min, float max)
    {
        if(std::abs(max - min) < 0.0001f)
        {
            return 0.0f;
        }
        return (value - min) / (max - min);
    }

    static float BounceOut(float t)
    {
        if(t < 1.0f / 2.75f)
        {
            return 7.5625f * t * t;
        }
        else if(t < 2.0f / 2.75f)
        {
            t -= 1.5f / 2.75f;
            return 7.5625f * t * t + 0.75f;
        }
        else if(t < 2.5f / 2.75f)
        {
            t -= 2.25f / 2.75f;
            return 7.5625f * t * t + 0.9375f;
        }
        else
        {
            t -= 2.625f / 2.75f;
            return 7.5625f * t * t + 0.984375f;
        }
    }

    static float SineEaseIn(float t) { return 1.0f - std::cos(t * PI / 2.0f); }
    static float SineEaseOut(float t) { return std::sin(t * PI / 2.0f); }
    static float SineEaseInOut(float t) { return -(std::cos(PI * t) - 1.0f) / 2.0f; }

    static float QuadEaseIn(float t) { return t * t; }
    static float QuadEaseOut(float t) { return 1.0f - (1.0f - t) * (1.0f - t); }
    static float QuadEaseInOut(float t) { return t < 0.5f ? 2.0f * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f; }

    static float CubicEaseIn(float t) { return t * t * t; }
    static float CubicEaseOut(float t) { return 1.0f - std::pow(1.0f - t, 3.0f); }
    static float CubicEaseInOut(float t) { return t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f; }

    static float QuartEaseIn(float t) { return t * t * t * t; }
    static float QuartEaseOut(float t) { return 1.0f - std::pow(1.0f - t, 4.0f); }
    static float QuartEaseInOut(float t) { return t < 0.5f ? 8.0f * t * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 4.0f) / 2.0f; }

    static float QuintEaseIn(float t) { return t * t * t * t * t; }
    static float QuintEaseOut(float t) { return 1.0f - std::pow(1.0f - t, 5.0f); }
    static float QuintEaseInOut(float t) { return t < 0.5f ? 16.0f * t * t * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 5.0f) / 2.0f; }

    static float ExpoEaseIn(float t) { return t == 0.0f ? 0.0f : std::pow(2.0f, 10.0f * t - 10.0f); }
    static float ExpoEaseOut(float t) { return t == 1.0f ? 1.0f : 1.0f - std::pow(2.0f, -10.0f * t); }
    static float ExpoEaseInOut(float t)
    {
        if(t == 0.0f)
        {
            return 0.0f;
        }
        if(t == 1.0f)
        {
            return 1.0f;
        }
        return t < 0.5f ? std::pow(2.0f, 20.0f * t - 10.0f) / 2.0f : (2.0f - std::pow(2.0f, -20.0f * t + 10.0f)) / 2.0f;
    }

    static float CircEaseIn(float t) { return 1.0f - std::sqrt(1.0f - t * t); }
    static float CircEaseOut(float t) { return std::sqrt(1.0f - std::pow(t - 1.0f, 2.0f)); }
    static float CircEaseInOut(float t) { return t < 0.5f ? (1.0f - std::sqrt(1.0f - std::pow(2.0f * t, 2.0f))) / 2.0f : (std::sqrt(1.0f - std::pow(-2.0f * t + 2.0f, 2.0f)) + 1.0f) / 2.0f; }

    static float BackEaseIn(float t)
    {
        const float c1 = 1.70158f;
        const float c3 = c1 + 1.0f;
        return c3 * t * t * t - c1 * t * t;
    }
    static float BackEaseOut(float t)
    {
        const float c1 = 1.70158f;
        const float c3 = c1 + 1.0f;
        return 1.0f + c3 * std::pow(t - 1.0f, 3.0f) + c1 * std::pow(t - 1.0f, 2.0f);
    }
    static float BackEaseInOut(float t)
    {
        const float c1 = 1.70158f;
        const float c2 = c1 * 1.525f;
        return t < 0.5f ? (std::pow(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f : (std::pow(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
    }

    static float ElasticEaseIn(float t)
    {
        const float c4 = (2.0f * PI) / 3.0f;
        if(t == 0.0f)
        {
            return 0.0f;
        }
        if(t == 1.0f)
        {
            return 1.0f;
        }
        return -std::pow(2.0f, 10.0f * t - 10.0f) * std::sin((t * 10.0f - 10.75f) * c4);
    }
    static float ElasticEaseOut(float t)
    {
        const float c4 = (2.0f * PI) / 3.0f;
        if(t == 0.0f)
        {
            return 0.0f;
        }
        if(t == 1.0f)
        {
            return 1.0f;
        }
        return std::pow(2.0f, -10.0f * t) * std::sin((t * 10.0f - 0.75f) * c4) + 1.0f;
    }
    static float ElasticEaseInOut(float t)
    {
        const float c5 = (2.0f * PI) / 4.5f;
        if(t == 0.0f)
        {
            return 0.0f;
        }
        if(t == 1.0f)
        {
            return 1.0f;
        }
        return t < 0.5f ? -(std::pow(2.0f, 20.0f * t - 10.0f) * std::sin((20.0f * t - 11.125f) * c5)) / 2.0f : (std::pow(2.0f, -20.0f * t + 10.0f) * std::sin((20.0f * t - 11.125f) * c5)) / 2.0f + 1.0f;
    }

    static float BounceEaseIn(float t) { return 1.0f - BounceOut(1.0f - t); }
    static float BounceEaseOut(float t) { return BounceOut(t); }
    static float BounceEaseInOut(float t) { return t < 0.5f ? (1.0f - BounceOut(1.0f - 2.0f * t)) / 2.0f : (1.0f + BounceOut(2.0f * t - 1.0f)) / 2.0f; }

    float Ease(float t, EaseType type, EaseMode mode)
    {
        t = Clamp(t, 0.0f, 1.0f);

        switch(type)
        {
            case EaseType::Sine:
                switch(mode)
                {
                    case EaseMode::In: return SineEaseIn(t);
                    case EaseMode::Out: return SineEaseOut(t);
                    case EaseMode::InOut: return SineEaseInOut(t);
                }
                break;
            case EaseType::Quad:
                switch(mode)
                {
                    case EaseMode::In: return QuadEaseIn(t);
                    case EaseMode::Out: return QuadEaseOut(t);
                    case EaseMode::InOut: return QuadEaseInOut(t);
                }
                break;
            case EaseType::Cubic:
                switch(mode)
                {
                    case EaseMode::In: return CubicEaseIn(t);
                    case EaseMode::Out: return CubicEaseOut(t);
                    case EaseMode::InOut: return CubicEaseInOut(t);
                }
                break;
            case EaseType::Quart:
                switch(mode)
                {
                    case EaseMode::In: return QuartEaseIn(t);
                    case EaseMode::Out: return QuartEaseOut(t);
                    case EaseMode::InOut: return QuartEaseInOut(t);
                }
                break;
            case EaseType::Quint:
                switch(mode)
                {
                    case EaseMode::In: return QuintEaseIn(t);
                    case EaseMode::Out: return QuintEaseOut(t);
                    case EaseMode::InOut: return QuintEaseInOut(t);
                }
                break;
            case EaseType::Expo:
                switch(mode)
                {
                    case EaseMode::In: return ExpoEaseIn(t);
                    case EaseMode::Out: return ExpoEaseOut(t);
                    case EaseMode::InOut: return ExpoEaseInOut(t);
                }
                break;
            case EaseType::Circ:
                switch(mode)
                {
                    case EaseMode::In: return CircEaseIn(t);
                    case EaseMode::Out: return CircEaseOut(t);
                    case EaseMode::InOut: return CircEaseInOut(t);
                }
                break;
            case EaseType::Back:
                switch(mode)
                {
                    case EaseMode::In: return BackEaseIn(t);
                    case EaseMode::Out: return BackEaseOut(t);
                    case EaseMode::InOut: return BackEaseInOut(t);
                }
                break;
            case EaseType::Elastic:
                switch(mode)
                {
                    case EaseMode::In: return ElasticEaseIn(t);
                    case EaseMode::Out: return ElasticEaseOut(t);
                    case EaseMode::InOut: return ElasticEaseInOut(t);
                }
                break;
            case EaseType::Bounce:
                switch(mode)
                {
                    case EaseMode::In: return BounceEaseIn(t);
                    case EaseMode::Out: return BounceEaseOut(t);
                    case EaseMode::InOut: return BounceEaseInOut(t);
                }
                break;
        }
        return t;
    }
}
