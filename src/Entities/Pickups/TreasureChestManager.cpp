#include "TreasureChestManager.h"

#include <algorithm>
#include <cmath>
#include <string>

namespace
{
constexpr int AnimationFrameCount = 8;
constexpr float IdleFrameDuration = 0.13f;
constexpr float OpenFrameDuration = 0.08f;
constexpr float OpenHoldDuration = 0.35f;
constexpr float CollectionRadius = 42.0f;
constexpr float ChestScale = 1.45f;
constexpr int MinimumGoldReward = 100;
constexpr int MaximumGoldReward = 200;

std::string MakeFrameName(const char* prefix, int index)
{
    return std::string(prefix) + (index < 10 ? "0" : "") + std::to_string(index);
}
}

void TreasureChestManager::Initialize(const TextureAtlas& atlas)
{
    m_idleFrames.clear();
    m_openFrames.clear();

    for(int frameIndex = 1; frameIndex <= AnimationFrameCount; ++frameIndex)
    {
        const AssetTextureData idleFrame =
            atlas.GetTextureData(MakeFrameName("TreasureIdle_", frameIndex));
        if(idleFrame.texture)
        {
            m_idleFrames.push_back(idleFrame);
        }

        const AssetTextureData openFrame =
            atlas.GetTextureData(MakeFrameName("TreasureOpen_", frameIndex));
        if(openFrame.texture)
        {
            m_openFrames.push_back(openFrame);
        }
    }
}

void TreasureChestManager::Clear()
{
    m_chests.clear();
}

void TreasureChestManager::Spawn(const sf::Vector2f& position)
{
    if(m_idleFrames.empty())
    {
        return;
    }

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
    chest->frameIndex = 0;
    chest->frameTimer = 0.0f;
    chest->openTimer = 0.0f;
    chest->goldReward = rewardDistribution(m_random);
    chest->active = true;
    chest->sprite.setPosition(position);
    chest->sprite.setScale(ChestScale, ChestScale);
    chest->sprite.setColor(sf::Color::White);
    ApplyFrame(*chest, m_idleFrames);
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
                continue;
            }

            chest.frameTimer += dt;
            if(chest.frameTimer >= IdleFrameDuration && !m_idleFrames.empty())
            {
                chest.frameTimer -= IdleFrameDuration;
                chest.frameIndex = (chest.frameIndex + 1) % m_idleFrames.size();
                ApplyFrame(chest, m_idleFrames);
            }
            continue;
        }

        if(chest.state == ChestState::Opening)
        {
            chest.frameTimer += dt;
            while(chest.frameTimer >= OpenFrameDuration && chest.state == ChestState::Opening)
            {
                chest.frameTimer -= OpenFrameDuration;
                ++chest.frameIndex;
                if(chest.frameIndex >= m_openFrames.size())
                {
                    chest.state = ChestState::Open;
                    chest.frameIndex = m_openFrames.empty() ? 0 : m_openFrames.size() - 1;
                    break;
                }
                ApplyFrame(chest, m_openFrames);
            }
            continue;
        }

        chest.openTimer += dt;
        if(chest.openTimer >= OpenHoldDuration)
        {
            chest.active = false;
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

void TreasureChestManager::ApplyFrame(
    Chest& chest,
    const std::vector<AssetTextureData>& frames) const
{
    if(frames.empty() || chest.frameIndex >= frames.size())
    {
        return;
    }

    const AssetTextureData& frame = frames[chest.frameIndex];
    chest.sprite.setTexture(*frame.texture, false);
    chest.sprite.setTextureRect(frame.rect);
    chest.sprite.setOrigin(
        static_cast<float>(frame.rect.width) / 2.0f,
        static_cast<float>(frame.rect.height) / 2.0f);
    chest.sprite.setPosition(chest.position);
}

void TreasureChestManager::BeginOpening(
    Chest& chest,
    const std::function<void(int)>& onGoldCollected)
{
    chest.state = ChestState::Opening;
    chest.frameIndex = 0;
    chest.frameTimer = 0.0f;
    chest.openTimer = 0.0f;
    if(!m_openFrames.empty())
    {
        ApplyFrame(chest, m_openFrames);
    }
    if(onGoldCollected)
    {
        onGoldCollected(chest.goldReward);
    }
}
