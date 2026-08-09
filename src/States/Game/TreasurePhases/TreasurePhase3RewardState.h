#pragma once

#include "ITreasurePhaseState.h"
#include "TreasureItemDetailCardWidget.h"
#include "UI/Components/NineSliceComponent.h"
#include "Core/Resources/TextureAtlas.h"
#include "Core/Animation/SpriteAnimator.h"
#include "Core/Animation/AnimationData.h"
#include <SFML/Graphics.hpp>
#include <vector>

// Configurable layout and animation timing parameters for Phase 3 Reward Reveal
struct TreasurePhase3Config
{
    float chestYOffset = 600.0f;
    float chestScale = 7.5f;

    // Reel Light Beams Parameters (matching Phase 2)
    float beamWidth = 105.0f;
    float beamMaxHeight = 700.0f;
    float beamAngleOffset = 21.0f;
    float beamOriginXOffset = 0.0f;
    float beamOriginYOffset = 40.0f;

    float scissorTopPadding = 10.0f;
    float scissorBottomPadding = 10.0f;
    float scissorLeftPadding = 10.0f;
    float scissorRightPadding = 10.0f;

    // 3 Distinct Color Sets for 1-Item, 3-Item, and 5-Item Chest light beams:
    sf::Color colors1Item[1] = {
        sf::Color(17, 0, 217, 161)
    };
    sf::Color colors3Item[3] = {
        sf::Color(239, 18, 253, 143),
        sf::Color(137, 13, 254, 140),
        sf::Color(239, 18, 253, 143)
    };
    sf::Color colors5Item[5] = {
        sf::Color(241, 254, 40, 137),
        sf::Color(220, 137, 25, 137),
        sf::Color(202, 16, 12, 137),
        sf::Color(220, 137, 25, 137),
        sf::Color(24, 254, 40, 137)
    };

    // Shared Card Layout & VFX Parameters (edit in ITreasurePhaseState.h to update Phase 2 and 3 together!)
    TreasureCardConfig cardConfig;

    // DONE Button Parameters
    float buttonYOffset = 770.0f;
    float buttonWidth = 230.0f;
    float buttonHeight = 72.0f;
    float doneAppearDelay = 0.0f;
    float doneAppearDuration = 0.13f;
    float arrowPadding = 30.0f;
    float arrowScale = 2.5f;
    float arrowFrameDuration = 0.08f;
};

class TreasurePhase3RewardState : public ITreasurePhaseState
{
public:
    TreasurePhase3RewardState() = default;
    ~TreasurePhase3RewardState() override = default;

    void OnEnter(TreasureRewardViewContext& ctx) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow* window, TreasureRewardViewContext& ctx) override;
    void Update(float dt, TreasureRewardViewContext& ctx) override;
    void UpdateLayout(const sf::Vector2f& viewSize, const sf::Vector2f& viewCenter, TreasureRewardViewContext& ctx) override;
    void Draw(sf::RenderTarget& target, const TreasureRewardViewContext& ctx) const override;
    void OnExit(TreasureRewardViewContext& ctx) override;

private:
    void ConfigureSprite(sf::Sprite& sprite, const AssetTextureData& data) const;
    void CenterText(sf::Text& text, float x, float y) const;

    TreasurePhase3Config m_config;

    // Assets
    AssetTextureData m_chestOpenFrame;
    AssetTextureData m_chestOpenFrontFrame;
    AssetTextureData m_prizeBgData;
    AssetTextureData m_hitStarData;
    AssetTextureData m_ringVfxData;
    AssetTextureData m_goldCoinIconData;

    // Per-reel Prize Cards & VFX
    std::vector<PrizeCardPerReel> m_prizeCards;
    float m_phaseTimer = 0.0f;

    // "DONE" Button (Reverse of Phase 1 OPEN button)
    mutable NineSliceComponent m_doneButtonBg;
    mutable sf::Text m_doneButtonText;
    sf::FloatRect m_doneButtonBounds;
    float m_doneAppearTimer = 0.0f;
    bool m_doneButtonActive = false;

    // Arrow cursors
    AnimationData m_arrowAnimData;
    SpriteAnimator m_arrowAnimator;
    mutable sf::Sprite m_leftArrow;
    mutable sf::Sprite m_rightArrow;

    // Gold Counter Display (Top of screen, gold text)
    mutable sf::Text m_goldCounterText;
    mutable sf::Sprite m_goldCoinIconSprite;

    // Bottom Treasure Item Detail Card (frame1_c2, 772x165px, top-edge grow/shrink)
    mutable TreasureItemDetailCardWidget m_detailCardWidget;
    std::size_t m_currentCardIndex = 0;

    // Exit transition animation state (when closing the treasure)
    bool m_isExiting = false;
    float m_exitTimer = 0.0f;
    float m_exitDuration = 0.17f;

    mutable sf::Sprite m_chestSprite;
    mutable sf::Sprite m_chestFrontSprite;
    mutable sf::Sprite m_prizeBgSprite;
    mutable sf::Sprite m_hitStarSprite;
    mutable sf::Sprite m_ringVfxSprite;
};
