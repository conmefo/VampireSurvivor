#pragma once

#include "ITreasurePhaseState.h"
#include <SFML/Graphics.hpp>
#include <random>
#include <vector>
// Layout & visual parameters for Phase 2 opening animation, beam reels, and coin burst
struct TreasurePhase2Config
{
    float chestYOffset = 600.0f;          // Match Phase 1 chest position
    float chestScale = 7.5f;              // Match Phase 1 chest scale
    float chestOpenFrameDuration = 0.07f; // Seconds per frame (TreasureOpen_01..08)
    bool isLooping = false;               // Plays once through 01..08 and holds open

    float beamWidth = 105.0f;             // Width of each light reel beam
    float beamMaxHeight = 700.0f;         // Max vertical height
    float beamGrowthDuration = 0.35f;     // Seconds to grow up from chest per beam
    float beamStaggerDelay = 0.12f;       // Seconds delay between growth of consecutive reels (leftmost first)
    float beamAngleOffset = 21.0f;        // Degrees offset between adjacent beams (3 or 5 items)
    float beamOriginXOffset = 0.0f;       // Horizontal starting offset from chest center
    float beamOriginYOffset = 40.0f;      // Vertical starting offset from chest center
    sf::Color beamColor{17, 0, 217, 161}; // Glowing purple/violet color

    float scissorTopPadding = 10.0f;      // Lowers the top scissor clipping line inside frame
    float scissorBottomPadding = 10.0f;
    float scissorLeftPadding = 10.0f;
    float scissorRightPadding = 10.0f;

    float reelScrollSpeed = 1300.0f;      // Speed of icons scrolling upward along reels (px/sec)
    float reelIconSpacing = 110.0f;       // Distance between adjacent icons on a reel
    float reelIconScale = 3.25f;          // Scale of icons on the reel

    // Falling Coin Explosion Parameters
    std::size_t coinBurstCount = 60;      // Total erupting gold & silver coins
    float coinGravity = 1100.0f;          // Downward gravity acceleration
    float coinEjectionSpeedMin = 800.0f;  // Minimum eruption launch speed
    float coinEjectionSpeedMax = 1140.0f; // Maximum eruption launch speed
    float coinAngleMinDeg = -120.0f;      // Launch cone minimum angle in degrees (-90 = straight up)
    float coinAngleMaxDeg = -60.0f;       // Launch cone maximum angle in degrees (0 = right)
    float coinScaleMin = 2.0f;            // Minimum coin scale
    float coinScaleMax = 2.5f;            // Maximum coin scale
    float silverCoinRatio = 0.40f;        // Ratio of silver vs gold coins (0.40 = 40% silver)

    // Background Spreading Distance Falloff Glow Parameters
    float bgGlowMaxRadiusRatio = 1.20f;   // Max spread radius relative to half panel width
    float bgGlowGrowthDuration = 7.5f;    // Seconds for full spread
    float bgGlowStartAlphaRatio = 0.0f;   // Initial central brightness ratio (0.0 = starts soft/invisible, 0.3 = starts 30% bright)
    float bgGlowMaxAlpha = 220.0f;        // Max central alpha brightness (0..255)
    float bgGlowFalloffExponent = 1.0f;   // Distance falloff curve sharpness (1.0 = linear, 2.0 = smooth bell curve, 3.0+ = tight central beam)
    sf::Color bgGlowColor{255, 230, 140, 0}; // Warm golden glow base color

    // Rising Light Ellipse Aura Parameters
    std::size_t risingEllipseCount = 8;   // Number of floating light ellipses
    float ellipseBaseRadiusMin = 35.0f;   // Base horizontal radius min
    float ellipseBaseRadiusMax = 50.0f;   // Base horizontal radius max
    float ellipseAspectRatioMin = 1.05f;   // Squeeze aspect ratio min (radiusY / radiusX)
    float ellipseAspectRatioMax = 1.65f;   // Squeeze aspect ratio max (radiusY / radiusX)
    float ellipseSpeedYMin = 920.0f;      // Upward float speed min (px/sec)
    float ellipseSpeedYMax = 1350.0f;      // Upward float speed max (px/sec)
    sf::Color ellipseColor{255, 235, 160, 80}; // Soft glowing light color

    // Phase 2 Coin Count-Up Tally Speed Parameters
    float tallyDurationMin = 3.0f;        // Tally duration (seconds) for 100 gold (larger = slower)
    float tallyDurationMax = 8.5f;        // Tally duration (seconds) for 1000 gold (larger = slower)
    float glowFadeOutDuration = 0.40f;    // Seconds for glow to completely fade out after reaching target gold

    // Shared Card Layout & VFX Parameters (edit in ITreasurePhaseState.h to update Phase 2 and 3 together!)
    TreasureCardConfig cardConfig;

    // 3 Distinct Color Sets for 1-Item, 3-Item, and 5-Item Chests:
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
};

class TreasurePhase2OpeningState : public ITreasurePhaseState
{
public:
    TreasurePhase2OpeningState() = default;
    ~TreasurePhase2OpeningState() override = default;

    void OnEnter(TreasureRewardViewContext& ctx) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow* window, TreasureRewardViewContext& ctx) override;
    void Update(float dt, TreasureRewardViewContext& ctx) override;
    void UpdateLayout(const sf::Vector2f& viewSize, const sf::Vector2f& viewCenter, TreasureRewardViewContext& ctx) override;
    void DrawBackground(sf::RenderTarget& target, const TreasureRewardViewContext& ctx) const override;
    void Draw(sf::RenderTarget& target, const TreasureRewardViewContext& ctx) const override;
    void OnExit(TreasureRewardViewContext& ctx) override;

    TreasurePhase2Config& GetConfig() { return m_config; }
    const TreasurePhase2Config& GetConfig() const { return m_config; }

private:
    struct RisingLightEllipse
    {
        sf::Vector2f position;   // Position in view coordinates
        sf::Vector2f radius;     // (radiusX, radiusY) where radiusY > radiusX
        float speedY = 180.0f;
        float pulsePhase = 0.0f;
        sf::Color color;
    };

    struct FallingCoinParticle
    {
        sf::Vector2f position;
        sf::Vector2f velocity;
        float rotation = 0.0f;
        float angularVelocity = 0.0f;
        float delay = 0.0f;
        float scale = 1.0f;

        bool isSilver = false;
        std::size_t frameIndex = 0;
        float frameTimer = 0.0f;
        float frameDuration = 0.06f;

        bool active = false;
        float life = 0.0f;
        float maxLife = 2.5f;
    };

    void ConfigureSprite(sf::Sprite& sprite, const AssetTextureData& data) const;
    void ConfigureSpriteBottomOrigin(sf::Sprite& sprite, const AssetTextureData& data) const;
    void PopulateIconPool(TextureAtlas& atlas);
    void InitCoinBurst(TextureAtlas& atlas, const sf::Vector2f& chestCenter, float scaleX, float scaleY);
    void RespawnCoin(FallingCoinParticle& coin, const sf::Vector2f& anchor, float scaleY);
    void InitRisingEllipses(const TreasureRewardViewContext& ctx);
    void RespawnEllipse(RisingLightEllipse& ellipse, const TreasureRewardViewContext& ctx, bool atBottom = true);
    void TriggerRewardSequence();

    TreasurePhase2Config m_config;
    mutable std::mt19937 m_coinRng;
    std::vector<AssetTextureData> m_openFrames;
    std::vector<AssetTextureData> m_openFrontFrames;

    std::size_t m_openFrameIndex = 0;
    float m_openFrameTimer = 0.0f;
    bool m_chestOpened = false;

    float m_beamTimer = 0.0f;
    bool m_beamGrowthFinished = false;
    float m_bgGlowTimer = 0.0f;
    std::vector<RisingLightEllipse> m_risingEllipses;

    std::vector<AssetTextureData> m_iconPool;
    std::vector<std::vector<AssetTextureData>> m_reelIcons;
    float m_reelScrollOffset = 0.0f;

    // Falling Coin Burst Frames & Particles (Gold & Silver)
    std::vector<AssetTextureData> m_goldCoinFrames;
    std::vector<AssetTextureData> m_silverCoinFrames;
    std::vector<FallingCoinParticle> m_coins;

    // Phase 2 Coin Counter Tally State
    int m_targetGold = 0;
    int m_currentGold = 0;
    float m_goldProgress = 0.0f;
    float m_goldTallyRate = 0.0f;
    bool m_tallyFinished = false;
    bool m_rewardTriggered = false;
    float m_fadeOutTimer = 0.0f;

    // PrizeBG Cards & VFX (Triggered in Phase 2)
    AssetTextureData m_prizeBgData;
    AssetTextureData m_hitStarData;
    AssetTextureData m_ringVfxData;
    std::vector<PrizeCardPerReel> m_prizeCards;
    float m_rewardTimer = 0.0f;

    AssetTextureData m_goldCoinIconData; // Static coin-spin-gold_01 texture data

    mutable sf::Sprite m_chestSprite;
    mutable sf::Sprite m_chestFrontSprite;
    mutable sf::Sprite m_reelIconSprite;
    mutable sf::Sprite m_coinSprite;
    mutable sf::Sprite m_goldCoinIconSprite;
    mutable sf::Text m_goldCounterText;
    mutable sf::Sprite m_prizeBgSprite;
    mutable sf::Sprite m_hitStarSprite;
    mutable sf::Sprite m_ringVfxSprite;
};
