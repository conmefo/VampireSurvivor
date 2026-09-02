#include "TreasureChestManager.h"

#include <algorithm>
#include <cmath>
#include <string>

namespace
{
constexpr int ArrowFrameCount = 8;
constexpr float ArrowFrameDuration = 0.075f;
constexpr float CollectionRadius = 42.0f;
constexpr float ChestScale = 1.0f;
constexpr float ArrowScale = 1.5f;
constexpr float ArrowOffsetY = 24.0f;
constexpr int MinimumGoldReward = 100;
constexpr int MaximumGoldReward = 200;

std::string MakeArrowFrameName(int index)
{
    return "arrow_0" + std::to_string(index);
}
}

void TreasureChestManager::Initialize(const TextureAtlas& atlas)
{
    m_chestData = atlas.GetTextureData("BoxOpen");
    if(!m_chestData.texture)
    {
        m_chestData = atlas.GetTextureData("BoxOpen.png");
    }

    m_arrowFrames.clear();
    for(int frameIndex = 1; frameIndex <= ArrowFrameCount; ++frameIndex)
    {
        const AssetTextureData arrowFrame =
            atlas.GetTextureData(MakeArrowFrameName(frameIndex));
        if(arrowFrame.texture)
        {
            m_arrowFrames.push_back(arrowFrame);
        }
    }
}

void TreasureChestManager::Clear()
{
    m_chests.clear();
}

void TreasureChestManager::Spawn(const sf::Vector2f& position)
{
    Chest* chest = nullptr;
    for(Chest& candidate : m_chests)
    {
        if(!candidate.active)
        {
            chest = &candidate;
            break;
        }
    }

    if(!chest)
    {
        m_chests.emplace_back();
        chest = &m_chests.back();
    }

    std::uniform_int_distribution<int> rewardDistribution(MinimumGoldReward, MaximumGoldReward);
    chest->position = position;
    chest->state = ChestState::Idle;
    chest->arrowFrameIndex = 0;
    chest->arrowFrameTimer = 0.0f;
    chest->goldReward = rewardDistribution(m_random);
    chest->active = true;

    if(m_chestData.texture)
    {
        chest->sprite.setTexture(*m_chestData.texture, false);
        chest->sprite.setTextureRect(m_chestData.rect);
        chest->sprite.setOrigin(
            static_cast<float>(m_chestData.rect.width) / 2.0f,
            static_cast<float>(m_chestData.rect.height) / 2.0f);
    }
    chest->sprite.setPosition(position);
    chest->sprite.setScale(ChestScale, ChestScale);
    chest->sprite.setColor(sf::Color::White);

    ApplyArrowFrame(*chest);
}

void TreasureChestManager::ApplyArrowFrame(Chest& chest) const
{
    if(m_arrowFrames.empty() || chest.arrowFrameIndex >= m_arrowFrames.size())
    {
        return;
    }

    const AssetTextureData& frame = m_arrowFrames[chest.arrowFrameIndex];
    if(!frame.texture)
    {
        return;
    }

    chest.arrowSprite.setTexture(*frame.texture, false);
    chest.arrowSprite.setTextureRect(frame.rect);
    chest.arrowSprite.setOrigin(
        static_cast<float>(frame.rect.width) / 2.0f,
        static_cast<float>(frame.rect.height) / 2.0f);
    chest.arrowSprite.setPosition(chest.position.x, chest.position.y - ArrowOffsetY);
    chest.arrowSprite.setRotation(90.0f); // Rotate 90 degrees to the right (pointing down to the chest)
    chest.arrowSprite.setScale(ArrowScale, ArrowScale);
    chest.arrowSprite.setColor(sf::Color::White);
}

void TreasureChestManager::Update(
    float dt,
    const sf::Vector2f& playerPosition,
    const std::function<void(int)>& onGoldCollected)
{
    dt = std::max(0.0f, dt);
    for(Chest& chest : m_chests)
    {
        if(!chest.active)
        {
            continue;
        }

        if(chest.state == ChestState::Idle)
        {
            const sf::Vector2f difference = playerPosition - chest.position;
            const float distanceSquared =
                difference.x * difference.x + difference.y * difference.y;
            if(distanceSquared <= CollectionRadius * CollectionRadius)
            {
                BeginOpening(chest, onGoldCollected);
                return;
            }

            // Animate arrow like a film projector pointing to the chest
            if(!m_arrowFrames.empty())
            {
                chest.arrowFrameTimer += dt;
                if(chest.arrowFrameTimer >= ArrowFrameDuration)
                {
                    chest.arrowFrameTimer -= ArrowFrameDuration;
                    chest.arrowFrameIndex = (chest.arrowFrameIndex + 1) % m_arrowFrames.size();
                    ApplyArrowFrame(chest);
                }
            }
        }
    }
}

void TreasureChestManager::Draw(sf::RenderTarget& target) const
{
    for(const Chest& chest : m_chests)
    {
        if(chest.active)
        {
            target.draw(chest.sprite);
            if(!m_arrowFrames.empty())
            {
                target.draw(chest.arrowSprite);
            }
        }
    }
}

std::size_t TreasureChestManager::GetActiveCount() const
{
    return static_cast<std::size_t>(std::count_if(
        m_chests.begin(),
        m_chests.end(),
        [](const Chest& chest) { return chest.active; }));
}

void TreasureChestManager::BeginOpening(
    Chest& chest,
    const std::function<void(int)>& onGoldCollected)
{
    // The original game moves directly into a full-screen treasure presentation
    // when the player touches a chest. Remove the world pickup before invoking the
    // callback so a resumed run cannot collect it twice.
    chest.active = false;
    if(onGoldCollected)
    {
        onGoldCollected(chest.goldReward);
    }
}
