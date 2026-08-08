#pragma once

#include "TreasureRewardViewContext.h"
#include <SFML/Graphics.hpp>

struct PrizeCardPerReel
{
    float yOffset = 0.0f; // Configurable Y offset along this reel's middle line
    sf::Vector2f position;
    float popTimer = 0.0f;
    bool popStarted = false;

    // Item reward payload data associated with this card
    ChestRewardItem rewardData;

    // VFX Impact Burst instance for this card
    float vfxTimer = 0.0f;
    bool vfxActive = false;
};

// Shared Configurable Card Layout & VFX Parameters (used across Phase 2 and Phase 3!)
struct TreasureCardConfig
{
    // Configurable Y-offsets for PrizeBG cards along each reel's middle line (easily modified per reel!)
    // Negative values = distance upward along the reel line from chest center
    std::vector<float> cardYOffsets1Item = {
        -290.0f
    };

    std::vector<float> cardYOffsets3Item = {
        -430.0f,  // Reel 1 (Left)
        -290.0f,  // Reel 2 (Center)
        -430.0f   // Reel 3 (Right)
    };

    std::vector<float> cardYOffsets5Item = {
        -225.0f,  // Reel 1 (Far Left)
        -430.0f,  // Reel 2 (Mid Left)
        -290.0f,  // Reel 3 (Center)
        -430.0f,  // Reel 4 (Mid Right)
        -225.0f   // Reel 5 (Far Right)
    };

    float cardPopDuration = 0.20f;
    float cardStaggerDelay = 0.08f;
    float cardScale = 3.0f;
    float iconScaleOnCard = 1.0f; // Relative scale of item icon inside PrizeBG card (0.55 = 55% of card size)

    // VFX Impact Burst effect parameters (HitStar1 + sPFX_ring_64 from vfx.png)
    float vfxDuration = 0.20f;
    float vfxStartScale = 7.0f;
    float vfxEndScale = 0.0f;
    float vfxRotationSpeed = 360.0f;
};

class ITreasurePhaseState
{
public:
    virtual ~ITreasurePhaseState() = default;

    virtual void OnEnter(TreasureRewardViewContext& ctx) = 0;
    virtual void HandleEvent(const sf::Event& event, const sf::RenderWindow* window, TreasureRewardViewContext& ctx) = 0;
    virtual void Update(float dt, TreasureRewardViewContext& ctx) = 0;
    virtual void UpdateLayout(const sf::Vector2f& viewSize, const sf::Vector2f& viewCenter, TreasureRewardViewContext& ctx) = 0;
    virtual void DrawBackground(sf::RenderTarget& /*target*/, const TreasureRewardViewContext& /*ctx*/) const {}
    virtual void Draw(sf::RenderTarget& target, const TreasureRewardViewContext& ctx) const = 0;
    virtual void OnExit(TreasureRewardViewContext& ctx) = 0;
};
