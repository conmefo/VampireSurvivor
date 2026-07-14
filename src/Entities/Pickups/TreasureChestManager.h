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
        Idle,
        Opening,
        Open
    };

    struct Chest
    {
        sf::Sprite sprite;
        sf::Vector2f position;
        ChestState state = ChestState::Idle;
        std::size_t frameIndex = 0;
        float frameTimer = 0.0f;
        float openTimer = 0.0f;
        int goldReward = 0;
        bool active = false;
    };

    void ApplyFrame(Chest& chest, const std::vector<AssetTextureData>& frames) const;
    void BeginOpening(Chest& chest, const std::function<void(int)>& onGoldCollected);

    std::vector<AssetTextureData> m_idleFrames;
    std::vector<AssetTextureData> m_openFrames;
    std::vector<Chest> m_chests;
    std::mt19937 m_random{std::random_device{}()};
};
