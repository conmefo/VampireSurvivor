#pragma once

#include <SFML/Graphics.hpp>

#include "../../Core/Resources/TextureAtlas.h"

#include <functional>
#include <memory>
#include <vector>

class Player;

class CoinPickupManager
{
public:
    void Initialize(const TextureAtlas& atlas);
    void Clear();
    void SpawnCoin(const sf::Vector2f& position, int value);
    void SpawnHealing(const sf::Vector2f& position, float value);
    void Update(float dt, const std::vector<std::unique_ptr<Player>>& players);
    void Draw(sf::RenderTarget& target) const;
    void SetOnCoinCollected(std::function<void(int)> callback) { m_onCoinCollected = std::move(callback); }

private:
    enum class PickupType { Coin, Healing };
    struct Pickup
    {
        sf::Vector2f position;
        sf::Vector2f velocity{0.0f, 0.0f};
        float value = 0.0f;
        PickupType type = PickupType::Coin;
        bool attracted = false;
    };

    std::vector<Pickup> m_pickups;
    std::function<void(int)> m_onCoinCollected;
    AssetTextureData m_coinVisual;
    AssetTextureData m_healingVisual;
};
