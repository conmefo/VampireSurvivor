#include "ExperienceGemManager.h"

#include <algorithm>
#include <cmath>

namespace
{
    float Length(const sf::Vector2f& value)
    {
        return std::sqrt(value.x * value.x + value.y * value.y);
    }

    sf::Vector2f Normalize(const sf::Vector2f& value)
    {
        const float length = Length(value);
        if(length <= 0.0001f)
        {
            return sf::Vector2f(0.0f, 0.0f);
        }

        return sf::Vector2f(value.x / length, value.y / length);
    }
}

void ExperienceGemManager::Initialize(TextureAtlas& atlas)
{
    m_gemTextureData = atlas.GetTextureData("GemBlue");
}

void ExperienceGemManager::Clear()
{
    m_gems.clear();
}

void ExperienceGemManager::Spawn(const sf::Vector2f& position, float value)
{
    if(value <= 0.0f || !m_gemTextureData.texture)
    {
        return;
    }

    ExperienceGem gem;
    gem.position = position;
    gem.value = value;
    gem.sprite.setTexture(*m_gemTextureData.texture);
    gem.sprite.setTextureRect(m_gemTextureData.rect);
    gem.sprite.setOrigin(
        static_cast<float>(m_gemTextureData.rect.width) / 2.0f,
        static_cast<float>(m_gemTextureData.rect.height) / 2.0f);
    gem.sprite.setScale(GemScale, GemScale);
    gem.sprite.setPosition(position);

    m_gems.push_back(gem);
}

void ExperienceGemManager::Update(float dt, Player& player)
{
    const sf::Vector2f playerPosition = player.GetPosition();
    const float collectDistance = player.GetCollisionRadius() + CollectRadius;

    for(ExperienceGem& gem : m_gems)
    {
        const sf::Vector2f toPlayer = playerPosition - gem.position;
        const float distance = Length(toPlayer);

        if(distance <= collectDistance)
        {
            player.AddExperience(gem.value);
            gem.value = 0.0f;
            continue;
        }

        if(distance <= MagnetRadius)
        {
            const float pull = 1.0f + (MagnetRadius - distance) / MagnetRadius;
            gem.position += Normalize(toPlayer) * MagnetSpeed * pull * dt;
            gem.sprite.setPosition(gem.position);
        }
    }

    m_gems.erase(
        std::remove_if(m_gems.begin(), m_gems.end(), [](const ExperienceGem& gem) {
            return gem.value <= 0.0f;
        }),
        m_gems.end());
}

void ExperienceGemManager::Draw(sf::RenderTarget& target) const
{
    for(const ExperienceGem& gem : m_gems)
    {
        target.draw(gem.sprite);
    }
}
