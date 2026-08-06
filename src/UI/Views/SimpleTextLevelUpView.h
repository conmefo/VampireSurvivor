#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>
#include "../Components/NineSliceComponent.h"
#include "../Components/LevelUpItemCard.h"
#include "../../Core/LevelUp/LevelUpOption.h"
#include "../../Core/Resources/TextureAtlas.h"
#include "../../Core/Animation/AnimationData.h"
#include "../../Core/Animation/SpriteAnimator.h"

struct FallingGem
{
    sf::Vector2f position;
    float fallSpeed = 50.0f;     // px/sec (slow constant falling)
    float rotation = 0.0f;       // static initial rotation angle
    float currentScale = 1.8f;
    float initialScale = 1.8f;
    float shrinkSpeed = 0.35f;  // scale reduction per sec
    sf::IntRect textureRect;
};

struct CardTargetInfo
{
    sf::Vector2f targetPosition;
    float width = 0.0f;
    float height = 0.0f;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
};

class SimpleTextLevelUpView
{
public:
    SimpleTextLevelUpView(const TextureAtlas& atlas, const sf::Font& font);
    ~SimpleTextLevelUpView() = default;

    void SetOnSelectOption(std::function<void(int)> callback) { m_onSelectOption = callback; }
    void SetOnReroll(std::function<void()> callback) { m_onReroll = callback; }
    void SetOnSkip(std::function<void()> callback) { m_onSkip = callback; }
    void SetOnBanish(std::function<void(int)> callback) { m_onBanish = callback; }

    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void UpdateChoices(
        const std::vector<LevelUpOption>& choices,
        int pendingLevelUps,
        int rerollCharges,
        int skipCharges,
        int banishCharges);

    void Update(float dt);
    void UpdateLayout(const sf::Vector2f& viewSize, const sf::Vector2f& viewCenter);
    void Draw(sf::RenderWindow& window);

private:
    void InitFallingGems();
    void RespawnGem(FallingGem& gem, bool randomY = false);
    void InitVfxAnimator();

    const TextureAtlas& m_atlas;
    const sf::Font& m_font;
    sf::RectangleShape m_dimOverlay;
    NineSliceComponent m_nineSliceBg;
    
    sf::Text m_titleText;
    sf::Text m_subtitleText;
    
    std::vector<LevelUpItemCard> m_cards;

    // Batched single-draw-call falling gem particle engine
    std::vector<FallingGem> m_fallingGems;
    sf::VertexArray m_gemVertices;
    const sf::Texture* m_gemAtlasTexture = nullptr;
    std::vector<sf::IntRect> m_gemRects;

    sf::Text m_controlsText;
    sf::Text m_luckTipText;

    std::function<void(int)> m_onSelectOption;
    std::function<void()> m_onReroll;
    std::function<void()> m_onSkip;
    std::function<void(int)> m_onBanish;

    int m_rerollCharges = 0;
    int m_skipCharges = 0;
    int m_banishCharges = 0;

    const sf::RenderWindow* m_window = nullptr;
    sf::Vector2f m_lastViewSize{1920.0f, 1080.0f};
    sf::Vector2f m_lastViewCenter{960.0f, 540.0f};

    // --- Configurable Entry Animation Parameters ---
    float m_bgAnimDuration = 0.25f;      // Duration (sec) for background panel scale & rotate
    float m_bgStartAngle = -90.0f;       // Initial rotation angle (deg) for background panel
    float m_cardSwipeDuration = 0.20f;   // Duration (sec) for each card swipe
    float m_cardStaggerDelay = 0.08f;    // Stagger delay offset (sec) between consecutive cards
    float m_vfxFrameDuration = 0.06f;    // Frame duration (sec) for Burst VFX film projector
    float m_vfxScale = 5.3f;             // Scale multiplier for Burst VFX film projector
    float m_vfxOffsetX = 0.0f;           // Horizontal offset (px at 1080p baseline) relative to viewCenter
    float m_vfxOffsetYRatio = 0.50f;     // Vertical offset ratio relative to panelHeight (0.0 = center, 0.28 = lower middle)

    // Entry Animation State
    bool m_isAppearing = false;
    float m_animTimer = 0.0f;
    std::vector<CardTargetInfo> m_cardTargets;

    // Burst VFX Film Projector (Burst1 .. Burst6)
    AnimationData m_burstAnimData;
    SpriteAnimator m_vfxAnimator;
    sf::Sprite m_burstSprite;
};
