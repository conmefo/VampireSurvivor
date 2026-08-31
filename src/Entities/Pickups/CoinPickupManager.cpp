#include "CoinPickupManager.h"

#include "../Player.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace
{
constexpr float CollectRadius = 22.0f;
constexpr float MagnetSpeed = 520.0f;

float Length(const sf::Vector2f& value)
{
    return std::sqrt(value.x * value.x + value.y * value.y);
}
}

void CoinPickupManager::Initialize(const TextureAtlas& atlas)
{
    m_coinVisual = atlas.GetTextureData("coin-spin-gold_01");
    m_healingVisual = atlas.GetTextureData("Roast");
}

void CoinPickupManager::Clear()
{
    m_pickups.clear();
}

void CoinPickupManager::SpawnCoin(const sf::Vector2f& position, int value)
{
    if(value > 0)
    {
        m_pickups.push_back({position, {0.0f, 0.0f}, static_cast<float>(value), PickupType::Coin});
    }
}

void CoinPickupManager::SpawnHealing(const sf::Vector2f& position, float value)
{
    if(value > 0.0f)
    {
        m_pickups.push_back({position, {0.0f, 0.0f}, value, PickupType::Healing});
    }
}

void CoinPickupManager::Update(float dt, const std::vector<std::unique_ptr<Player>>& players)
{
    for(Pickup& pickup : m_pickups)
    {
        Player* nearest = nullptr;
        float nearestDistanceSquared = std::numeric_limits<float>::max();
        for(const auto& player : players)
        {
            if(!player || player->IsDead()) continue;
            const sf::Vector2f difference = player->GetCenterPosition() - pickup.position;
            const float distanceSquared = difference.x * difference.x + difference.y * difference.y;
            if(distanceSquared < nearestDistanceSquared)
            {
                nearestDistanceSquared = distanceSquared;
                nearest = player.get();
            }
        }

        if(!nearest) continue;

        const sf::Vector2f toPlayer = nearest->GetCenterPosition() - pickup.position;
        const float distance = Length(toPlayer);
        if(distance <= nearest->GetCollisionRadius() + CollectRadius)
        {
            if(pickup.type == PickupType::Coin && m_onCoinCollected)
            {
                m_onCoinCollected(static_cast<int>(pickup.value));
            }
            else if(pickup.type == PickupType::Healing)
            {
                nearest->Heal(pickup.value);
            }
            pickup.value = 0.0f;
            continue;
        }

        if(distance <= nearest->GetMagnetRadius())
        {
            pickup.attracted = true;
            pickup.velocity = toPlayer * (MagnetSpeed / std::max(distance, 1.0f));
        }
        if(pickup.attracted)
        {
            pickup.position += pickup.velocity * dt;
        }
    }

    m_pickups.erase(
        std::remove_if(m_pickups.begin(), m_pickups.end(), [](const Pickup& pickup) { return pickup.value <= 0.0f; }),
        m_pickups.end());
}

void CoinPickupManager::Draw(sf::RenderTarget& target) const
{
    for(const Pickup& pickup : m_pickups)
    {
        const bool coin = pickup.type == PickupType::Coin;
        const AssetTextureData& visual = coin ? m_coinVisual : m_healingVisual;
        if(!visual.texture || visual.rect.width <= 0 || visual.rect.height <= 0)
        {
            continue;
        }

        sf::Sprite sprite;
        sprite.setTexture(*visual.texture);
        sprite.setTextureRect(visual.rect);
        sprite.setOrigin(
            static_cast<float>(visual.rect.width) * 0.5f,
            static_cast<float>(visual.rect.height) * 0.5f);
        sprite.setPosition(pickup.position);

        const float targetHeight = coin ? 24.0f : 32.0f;
        const float scale = targetHeight / static_cast<float>(visual.rect.height);
        sprite.setScale(scale, scale);
        target.draw(sprite);
    }
}
