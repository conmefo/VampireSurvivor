#pragma once

#include "../../Core/Resources/TextureAtlas.h"

#include <SFML/Graphics.hpp>
#include <functional>
#include <random>
#include <vector>

class TreasureChestManager
{
public:
    void Initialize(const TextureAtlas& atlas);
    void Clear();
    void Spawn(const sf::Vector2f& position);
    void Update(
        float dt,
        const sf::Vector2f& playerPosition,
        const std::function<void(int)>& onGoldCollected);
    void Draw(sf::RenderTarget& target) const;

    std::size_t GetActiveCount() const;

private:
    enum class ChestState
    {
        Idle
    };

    struct Chest
    {
        sf::Sprite sprite;
        sf::Sprite arrowSprite;
        sf::Vector2f position;
        ChestState state = ChestState::Idle;
        std::size_t arrowFrameIndex = 0;
        float arrowFrameTimer = 0.0f;
        int goldReward = 0;
        bool active = false;
    };

    void ApplyArrowFrame(Chest& chest) const;
    void BeginOpening(Chest& chest, const std::function<void(int)>& onGoldCollected);

    AssetTextureData m_chestData;
    std::vector<AssetTextureData> m_arrowFrames;
    std::vector<Chest> m_chests;
    std::mt19937 m_random{std::random_device{}()};
};
