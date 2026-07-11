#pragma once

#include <SFML/Graphics.hpp>

#include "../Core/Resources/TextureAtlas.h"

#include <array>
#include <string>
#include <vector>

class DamageNumberManager
{
public:
    void Initialize(const TextureAtlas& atlas);
    void Clear();

    void Spawn(float damage, const sf::Vector2f& position);
    void Update(float dt);
    void Draw(sf::RenderTarget& target) const;

private:
    struct DigitGlyph
    {
        const sf::Texture* texture = nullptr;
        sf::IntRect rect;
    };

    struct DamageNumber
    {
        std::vector<sf::Sprite> digits;
        sf::Vector2f velocity;
        float age = 0.0f;
        float lifetime = 0.75f;
    };

    static std::string FormatDamage(float damage);

    std::array<DigitGlyph, 10> m_digits;
    bool m_ready = false;
    std::vector<DamageNumber> m_numbers;
    unsigned int m_spawnCounter = 0;
};
