#pragma once

#include "ITreasurePhaseState.h"
#include "UI/Components/NineSliceComponent.h"
#include "Core/Animation/SpriteAnimator.h"
#include "Core/Animation/AnimationData.h"
#include <SFML/Graphics.hpp>
#include <vector>

// Layout & visual parameters for Phase 1 (Easy tuning for visual polish)
struct TreasurePhase1Config
{
    float titleYOffset = 65.0f;           // Distance from top of panel frame
    unsigned int titleFontSize = 47;
    sf::Color titleColor{255, 225, 82};
    sf::Color titleOutlineColor{98, 25, 3};
    float titleOutlineThickness = 3.0f;

    float chestYOffset = 600.0f;          // Distance from top of panel frame to chest center
    float chestScale = 7.5f;              // Scale multiplier for chest idle frames
    float chestFrameDuration = 0.09f;     // Seconds per frame in idle loop (01..08)
    float chestLoopDelay = 1.0f;         // Delay pause after completing 1 loop (seconds)

    float buttonYOffset = 765.0f;         // Distance from top of panel frame to OPEN button center
    float buttonWidth = 265.0f;
    float buttonHeight = 72.0f;
    unsigned int buttonFontSize = 37;

    float arrowPadding = 30.0f;           // Offset from left & right sides of OPEN button
    float arrowScale = 2.2f;              // Scale for spinning arrows
    float arrowFrameDuration = 0.08f;     // Speed of spinning arrow animation

    float buttonExitDuration = 0.20f;     // Duration of OPEN button shrink & rotate exit animation
};

class TreasurePhase1IdleState : public ITreasurePhaseState
{
public:
    TreasurePhase1IdleState() = default;
    ~TreasurePhase1IdleState() override = default;

    void OnEnter(TreasureRewardViewContext& ctx) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow* window, TreasureRewardViewContext& ctx) override;
    void Update(float dt, TreasureRewardViewContext& ctx) override;
    void UpdateLayout(const sf::Vector2f& viewSize, const sf::Vector2f& viewCenter, TreasureRewardViewContext& ctx) override;
    void Draw(sf::RenderTarget& target, const TreasureRewardViewContext& ctx) const override;
    void OnExit(TreasureRewardViewContext& ctx) override;

    TreasurePhase1Config& GetConfig() { return m_config; }
    const TreasurePhase1Config& GetConfig() const { return m_config; }

private:
    void ConfigureSprite(sf::Sprite& sprite, const AssetTextureData& data) const;
    void CenterText(sf::Text& text, float x, float y) const;
    void StartExitSequence();

    TreasurePhase1Config m_config;

    std::vector<AssetTextureData> m_idleFrames;
    sf::Text m_titleText;

    NineSliceComponent m_openButtonBg;
    sf::Text m_openButtonText;
    sf::FloatRect m_openButtonBounds;

    std::size_t m_idleFrameIndex = 0;
    float m_idleFrameTimer = 0.0f;
    float m_idleDelayTimer = 0.0f;
    bool m_idleInPause = false;

    // Exit transition animation state
    bool m_isExiting = false;
    float m_exitTimer = 0.0f;
    bool m_showArrows = true;

    AnimationData m_arrowAnimData;
    SpriteAnimator m_arrowAnimator;
    mutable sf::Sprite m_leftArrow;
    mutable sf::Sprite m_rightArrow;
    mutable sf::Sprite m_chestSprite;
};
