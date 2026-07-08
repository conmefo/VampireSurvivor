#pragma once

#include "../../Core/Resources/TextureAtlas.h"
#include "../Player.h"

#include <SFML/Graphics.hpp>
#include <vector>

class ExperienceGemManager
{
public:
    void Initialize(TextureAtlas& atlas);
    void Clear();
    void Spawn(const sf::Vector2f& position, float value);
    void Update(float dt, Player& player);
    void Draw(sf::RenderTarget& target) const;

private:
    struct ExperienceGem
    {
        sf::Sprite sprite;
        sf::Vector2f position;
        float value = 0.0f;
    };

    static constexpr float GemScale = 1.35f;
    static constexpr float CollectRadius = 24.0f;
    static constexpr float MagnetRadius = 96.0f;
    static constexpr float MagnetSpeed = 260.0f;

    AssetTextureData m_gemTextureData{nullptr, sf::IntRect()};
    std::vector<ExperienceGem> m_gems;
};
