#include "TreasurePhase2OpeningState.h"
#include <SFML/OpenGL.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>
#include <string>

void TreasurePhase2OpeningState::OnEnter(TreasureRewardViewContext& ctx)
{
    m_openFrames.clear();
    m_openFrontFrames.clear();
    for(int frameIndex = 1; frameIndex <= 8; ++frameIndex)
    {
        const std::string suffix = frameIndex < 10 ? "0" + std::to_string(frameIndex)
                                                   : std::to_string(frameIndex);
        const AssetTextureData openFrame = ctx.atlas.GetTextureData("TreasureOpen_" + suffix);
        if(openFrame.texture)
        {
            m_openFrames.push_back(openFrame);
        }

        const AssetTextureData frontFrame = ctx.atlas.GetTextureData("TreasureOpenFront_" + suffix);
        if(frontFrame.texture)
        {
            m_openFrontFrames.push_back(frontFrame);
        }
    }

    m_openFrameIndex = 0;
    m_openFrameTimer = 0.0f;
    m_chestOpened = false;

    m_beamTimer = 0.0f;
    m_beamGrowthFinished = false;
    m_bgGlowTimer = 0.0f;

    // Load static coin-spin-gold_01 icon for Phase 2 tally counter display
    m_goldCoinIconData = ctx.atlas.GetTextureData("coin-spin-gold_01");
    if(!m_goldCoinIconData.texture)
    {
        m_goldCoinIconData = ctx.atlas.GetTextureData("coin-spin-gold_01.png");
    }

    m_prizeBgData = ctx.atlas.GetTextureData("PrizeBG");
    if(!m_prizeBgData.texture) m_prizeBgData = ctx.atlas.GetTextureData("PrizeBG.png");

    m_hitStarData = ctx.atlas.GetTextureData("HitStar1");
    if(!m_hitStarData.texture) m_hitStarData = ctx.atlas.GetTextureData("HitStar1.png");

    m_ringVfxData = ctx.atlas.GetTextureData("sPFX_ring_64");
    if(!m_ringVfxData.texture) m_ringVfxData = ctx.atlas.GetTextureData("sPFX_ring_64.png");

    m_prizeCards.clear();
    int count = std::max(1, ctx.itemCount);
    m_prizeCards.resize(count);

    const auto& yOffsets = (count <= 1) ? m_config.cardConfig.cardYOffsets1Item
                         : (count == 3) ? m_config.cardConfig.cardYOffsets3Item
                                        : m_config.cardConfig.cardYOffsets5Item;

    for(int i = 0; i < count; ++i)
    {
        if(i < static_cast<int>(yOffsets.size()))
        {
            m_prizeCards[i].yOffset = yOffsets[i];
        }
        else
        {
            m_prizeCards[i].yOffset = -200.0f;
        }

        if(i < static_cast<int>(ctx.rewards.size()))
        {
            m_prizeCards[i].rewardData = ctx.rewards[i];
        }
        else
        {
            // Default candidate fallback rewards
            const std::vector<std::string> defaults = {"WandFire", "Garlic", "Whip", "HolyWater", "LightningRing"};
            m_prizeCards[i].rewardData.iconKey = defaults[i % defaults.size()];
            m_prizeCards[i].rewardData.itemName = defaults[i % defaults.size()];
        }
    }
    m_rewardTimer = 0.0f;

    // Determine target gold payout based on chest tier (1, 3, 5 items)
    std::mt19937 targetRng(1337 + static_cast<unsigned int>(ctx.itemCount * 777 + ctx.viewCenter.x));
    if(ctx.itemCount <= 1)
    {
        std::uniform_int_distribution<int> goldDist(100, 200);
        m_targetGold = goldDist(targetRng);
    }
    else if(ctx.itemCount == 3)
    {
        std::uniform_int_distribution<int> goldDist(300, 600);
        m_targetGold = goldDist(targetRng);
    }
    else
    {
        std::uniform_int_distribution<int> goldDist(500, 1000);
        m_targetGold = goldDist(targetRng);
    }

    m_currentGold = 0;
    m_goldProgress = 0.0f;
    m_tallyFinished = false;
    m_rewardTriggered = false;
    m_fadeOutTimer = 0.0f;

    // Adaptive tally rate using config parameters (tallyDurationMin to tallyDurationMax)
    float goldRatio = std::clamp(static_cast<float>(m_targetGold - 100) / 900.0f, 0.0f, 1.0f);
    float tallyDuration = m_config.tallyDurationMin + (m_config.tallyDurationMax - m_config.tallyDurationMin) * goldRatio;
    tallyDuration = std::max(0.2f, tallyDuration);
    m_goldTallyRate = static_cast<float>(m_targetGold) / tallyDuration;

    InitRisingEllipses(ctx);

    PopulateIconPool(ctx.atlas);
    m_reelIcons.clear();
    int beamCount = std::max(1, ctx.itemCount);
    m_reelIcons.resize(beamCount);

    std::mt19937 rng(1337 + ctx.itemCount);
    if(!m_iconPool.empty())
    {
        std::uniform_int_distribution<std::size_t> dist(0, m_iconPool.size() - 1);
        for(int r = 0; r < beamCount; ++r)
        {
            for(int k = 0; k < 16; ++k)
            {
                m_reelIcons[r].push_back(m_iconPool[dist(rng)]);
            }
        }
    }
    m_reelScrollOffset = 0.0f;

    UpdateLayout(ctx.viewSize, ctx.viewCenter, ctx);

    float scaleX = ctx.panelSize.x / 640.0f;
    float scaleY = ctx.panelSize.y / 860.0f;
    float chestCenterY = ctx.panelPos.y + m_config.chestYOffset * scaleY;
    InitCoinBurst(ctx.atlas, sf::Vector2f(ctx.viewCenter.x, chestCenterY), scaleX, scaleY);
}

void TreasurePhase2OpeningState::HandleEvent(const sf::Event& event, const sf::RenderWindow* /*window*/, TreasureRewardViewContext& ctx)
{
    if(!m_chestOpened)
    {
        return;
    }

    const bool activated =
        (event.type == sf::Event::KeyPressed &&
         (event.key.code == sf::Keyboard::Enter ||
          event.key.code == sf::Keyboard::Space ||
          event.key.code == sf::Keyboard::Escape)) ||
        (event.type == sf::Event::MouseButtonPressed &&
         event.mouseButton.button == sf::Mouse::Left);

    if(activated)
    {
        if(!m_rewardTriggered)
        {
            // First click/key: trigger reward sequence (complete coin tally & start glow/coin fade)
            TriggerRewardSequence();
        }
        else
        {
            // Second click/key: instantly clear remaining coins & glow, proceed to Phase 3
            m_fadeOutTimer = m_config.glowFadeOutDuration;
            for(auto& coin : m_coins)
            {
                coin.active = false;
            }
            if(ctx.triggerReward) ctx.triggerReward();
            else if(ctx.finish) ctx.finish();
        }
    }
}

void TreasurePhase2OpeningState::Update(float dt, TreasureRewardViewContext& ctx)
{
    // Step 1: Update film projector chest opening animation (TreasureOpen_01..08)
    if(!m_chestOpened && !m_openFrames.empty())
    {
        m_openFrameTimer += dt;
        if(m_openFrameTimer >= m_config.chestOpenFrameDuration)
        {
            m_openFrameTimer -= m_config.chestOpenFrameDuration;
            if(m_openFrameIndex + 1 < m_openFrames.size())
            {
                m_openFrameIndex++;
            }
            else
            {
                m_chestOpened = true; // Finished opening animation, hold on TreasureOpen_08
            }
        }
    }

    // Step 2: Once chest is open, grow light beams, scroll icons, and update falling coin physics
    if(m_chestOpened)
    {
        m_bgGlowTimer += dt;

        // Update Phase 2 dynamic coin count-up tally
        if(!m_rewardTriggered && m_targetGold > 0)
        {
            m_goldProgress += dt * m_goldTallyRate;
            m_currentGold = std::min(m_targetGold, static_cast<int>(std::floor(m_goldProgress)));
            if(m_currentGold >= m_targetGold)
            {
                TriggerRewardSequence();
            }
        }

        if(m_rewardTriggered)
        {
            m_fadeOutTimer += dt;
        }

        // Update upward movement of rising light ellipses (lower edge to upper edge)
        float topY = ctx.viewCenter.y - ctx.viewSize.y * 0.5f;
        for(auto& ellipse : m_risingEllipses)
        {
            ellipse.position.y -= dt * ellipse.speedY;
            ellipse.pulsePhase += dt * 2.5f;

            if(ellipse.position.y < topY - ellipse.radius.y - 30.0f)
            {
                RespawnEllipse(ellipse, ctx, true);
            }
        }

        int beamCount = ctx.itemCount > 0 ? ctx.itemCount : 1;
        float totalGrowthDuration = static_cast<float>(beamCount - 1) * m_config.beamStaggerDelay + m_config.beamGrowthDuration;
        if(!m_beamGrowthFinished)
        {
            m_beamTimer += dt;
            if(m_beamTimer >= totalGrowthDuration)
            {
                m_beamGrowthFinished = true;
            }
        }

        // Continuously scroll item/weapon icons upward along the reels
        m_reelScrollOffset += dt * m_config.reelScrollSpeed;

        // Step 3: Update Gold and Silver falling coin particles (Continuous Recycling Pool)
        float scaleX = ctx.panelSize.x / 640.0f;
        float scaleY = ctx.panelSize.y / 860.0f;
        float chestCenterX = ctx.viewCenter.x;
        float chestCenterY = ctx.panelPos.y + m_config.chestYOffset * scaleY;
        float anchorX = chestCenterX + m_config.beamOriginXOffset * scaleX;
        float anchorY = chestCenterY + m_config.beamOriginYOffset * scaleY;
        sf::Vector2f anchor(anchorX, anchorY);
        float bottomLimit = ctx.viewCenter.y + ctx.viewSize.y * 0.5f;
        float gravity = m_config.coinGravity * scaleY;

        for(auto& coin : m_coins)
        {
            if(!coin.active) continue;

            if(coin.delay > 0.0f)
            {
                coin.delay -= dt;
                continue;
            }

            coin.life += dt;

            // Physics integration (gravity + velocity + angular tumbling)
            coin.velocity.y += gravity * dt;
            coin.position += coin.velocity * dt;
            coin.rotation += coin.angularVelocity * dt;

            // Recycle coin when it falls off-screen or reaches maxLife
            if(coin.life >= coin.maxLife || coin.position.y > bottomLimit)
            {
                // Stop generating new coins when reward sequence is triggered
                if(!m_rewardTriggered)
                {
                    RespawnCoin(coin, anchor, scaleY);
                    coin.delay = 0.0f; // Continuously fire back
                }
                else
                {
                    coin.active = false; // Despawn so 0 coins remain on view
                }
                continue;
            }

            // Sprite frame animation loop (01..05)
            const auto& animFrames = coin.isSilver ? m_silverCoinFrames : m_goldCoinFrames;
            if(!animFrames.empty())
            {
                coin.frameTimer += dt;
                if(coin.frameTimer >= coin.frameDuration)
                {
                    coin.frameTimer -= coin.frameDuration;
                    coin.frameIndex = (coin.frameIndex + 1) % animFrames.size();
                }
            }
        }

        // Officially switch to Phase 3 when glow has completely faded out AND all coins have cleared AND cards popped
        if(m_rewardTriggered)
        {
            m_rewardTimer += dt;

            for(std::size_t i = 0; i < m_prizeCards.size(); ++i)
            {
                auto& card = m_prizeCards[i];
                float startTime = static_cast<float>(i) * m_config.cardConfig.cardStaggerDelay;

                if(m_rewardTimer >= startTime && !card.popStarted)
                {
                    card.popStarted = true;
                    card.vfxActive = true;
                    card.vfxTimer = 0.0f;
                }

                if(card.popStarted)
                {
                    card.popTimer += dt;
                }

                if(card.vfxActive)
                {
                    card.vfxTimer += dt;
                    if(card.vfxTimer >= m_config.cardConfig.vfxDuration)
                    {
                        card.vfxActive = false;
                    }
                }
            }

            bool activeCoinsRemain = std::any_of(m_coins.begin(), m_coins.end(), [](const FallingCoinParticle& c){ return c.active; });
            bool glowFadedOut = (m_fadeOutTimer >= m_config.glowFadeOutDuration);
            bool cardsFinished = std::all_of(m_prizeCards.begin(), m_prizeCards.end(), [this](const PrizeCardPerReel& c){
                return c.popStarted && c.popTimer >= m_config.cardConfig.cardPopDuration;
            });

            if(glowFadedOut && !activeCoinsRemain && cardsFinished)
            {
                if(ctx.triggerReward) ctx.triggerReward();
                else if(ctx.finish) ctx.finish();
            }
        }
    }
}

void TreasurePhase2OpeningState::UpdateLayout(const sf::Vector2f& viewSize, const sf::Vector2f& viewCenter, TreasureRewardViewContext& ctx)
{
    float scaleX = viewSize.x / 1920.0f;
    float scaleY = viewSize.y / 1080.0f;

    ctx.viewSize = viewSize;
    ctx.viewCenter = viewCenter;
    ctx.panelSize = sf::Vector2f(640.0f * scaleX, 860.0f * scaleY);
    ctx.panelPos = viewCenter - ctx.panelSize / 2.0f;

    float panelCornerScale = 1.4f * scaleY;
    ctx.nineSliceBg.SetSize(ctx.panelSize);
    ctx.nineSliceBg.SetCornerScale(panelCornerScale);
    ctx.nineSliceBg.setPosition(ctx.panelPos);
    ctx.nineSliceBg.Update();

    // Calculate positions for per-reel PrizeBG cards (locked onto each reel's exact center ray line!)
    int beamCount = std::max(1, ctx.itemCount);
    float chestCenterX = ctx.viewCenter.x;
    float chestCenterY = ctx.panelPos.y + m_config.chestYOffset * scaleY;
    float anchorX = chestCenterX + m_config.beamOriginXOffset * scaleX;
    float anchorY = chestCenterY + m_config.beamOriginYOffset * scaleY;
    constexpr float degToRad = 3.1415926535f / 180.0f;

    for(int i = 0; i < beamCount && i < static_cast<int>(m_prizeCards.size()); ++i)
    {
        float offsetMultiplier = static_cast<float>(i) - static_cast<float>(beamCount - 1) * 0.5f;
        float angleDeg = offsetMultiplier * m_config.beamAngleOffset;
        float angleRad = angleDeg * degToRad;

        float cardY = chestCenterY + m_prizeCards[i].yOffset * scaleY;
        float dy = anchorY - cardY;
        float dist = dy / std::cos(angleRad);
        float cardX = anchorX + dist * std::sin(angleRad);

        m_prizeCards[i].position = sf::Vector2f(cardX, cardY);
    }
}

void TreasurePhase2OpeningState::DrawBackground(sf::RenderTarget& target, const TreasureRewardViewContext& ctx) const
{
    float glowFadeFactor = 1.0f;
    if(m_rewardTriggered)
    {
        float fadeRatio = std::clamp(m_fadeOutTimer / m_config.glowFadeOutDuration, 0.0f, 1.0f);
        glowFadeFactor = 1.0f - fadeRatio;
    }

    if(glowFadeFactor <= 0.0f)
    {
        return; // Background glow and ellipses completely disappeared
    }

    // Spreading Distance-based Light Glow across the ENTIRE view (drawn under background panel frame)
    if(m_chestOpened && m_bgGlowTimer > 0.0f)
    {
        float glowProgress = std::clamp(m_bgGlowTimer / m_config.bgGlowGrowthDuration, 0.0f, 1.0f);
        float currentRadius = (ctx.viewSize.x * 0.5f * m_config.bgGlowMaxRadiusRatio) * glowProgress;
        float currentAlpha = m_config.bgGlowMaxAlpha * (m_config.bgGlowStartAlphaRatio + (1.0f - m_config.bgGlowStartAlphaRatio) * glowProgress) * glowFadeFactor;

        float centerX = ctx.viewCenter.x;
        float topY = ctx.viewCenter.y - ctx.viewSize.y * 0.5f;
        float bottomY = ctx.viewCenter.y + ctx.viewSize.y * 0.5f;

        constexpr std::size_t sliceCount = 32;
        sf::VertexArray glowMesh(sf::TriangleStrip, (sliceCount + 1) * 2);

        constexpr float pi = 3.1415926535f;

        for(std::size_t i = 0; i <= sliceCount; ++i)
        {
            float t = static_cast<float>(i) / static_cast<float>(sliceCount);
            float x = (centerX - currentRadius) + t * (currentRadius * 2.0f);

            float distance = std::abs(x - centerX);
            float distRatio = std::clamp(distance / currentRadius, 0.0f, 1.0f);

            // Distance-based bell-curve falloff with configurable exponent curve shape
            float cosVal = std::cos(distRatio * (pi * 0.5f));
            float falloff = std::pow(cosVal, m_config.bgGlowFalloffExponent);

            sf::Uint8 alpha = static_cast<sf::Uint8>(std::round(currentAlpha * falloff));
            sf::Color col = m_config.bgGlowColor;
            col.a = alpha;

            glowMesh[i * 2].position = sf::Vector2f(x, topY);
            glowMesh[i * 2].color = col;

            glowMesh[i * 2 + 1].position = sf::Vector2f(x, bottomY);
            glowMesh[i * 2 + 1].color = col;
        }

        target.draw(glowMesh, sf::BlendAdd);
    }

    // 2. Draw Rising Light Ellipses (vertical axis > horizontal axis, moving upward) over glow, under panel frame
    if(m_chestOpened && !m_risingEllipses.empty())
    {
        float globalGlowProgress = std::clamp(m_bgGlowTimer / m_config.bgGlowGrowthDuration, 0.0f, 1.0f);
        sf::Uint8 maxA = m_config.ellipseColor.a;

        constexpr std::size_t fanPoints = 24;
        constexpr float pi = 3.1415926535f;

        for(const auto& ellipse : m_risingEllipses)
        {
            // Soft alpha pulse + fade-in with background glow + fade-out near target
            float pulse = 0.85f + 0.15f * std::sin(ellipse.pulsePhase);
            sf::Uint8 currentA = static_cast<sf::Uint8>(std::round(maxA * globalGlowProgress * pulse * glowFadeFactor));

            if(currentA <= 0) continue;

            sf::VertexArray fanMesh(sf::TriangleFan, fanPoints + 2);

            // Solid filled ellipse (no soft edge fade)
            sf::Color fillCol = ellipse.color;
            fillCol.a = currentA;

            fanMesh[0].position = ellipse.position;
            fanMesh[0].color = fillCol;

            for(std::size_t p = 0; p <= fanPoints; ++p)
            {
                float angle = (static_cast<float>(p) / static_cast<float>(fanPoints)) * (2.0f * pi);
                float px = ellipse.position.x + std::cos(angle) * ellipse.radius.x;
                float py = ellipse.position.y + std::sin(angle) * ellipse.radius.y; // radiusY > radiusX

                fanMesh[p + 1].position = sf::Vector2f(px, py);
                fanMesh[p + 1].color = fillCol;
            }

            target.draw(fanMesh, sf::BlendAdd);
        }
    }
}

void TreasurePhase2OpeningState::Draw(sf::RenderTarget& target, const TreasureRewardViewContext& ctx) const
{
    float scaleX = ctx.panelSize.x / 640.0f;
    float scaleY = ctx.panelSize.y / 860.0f;
    float chestCenterX = ctx.viewCenter.x;
    float chestCenterY = ctx.panelPos.y + m_config.chestYOffset * scaleY;
    float chestBottomY = chestCenterY + 11.0f * m_config.chestScale * scaleY;
    float anchorX = chestCenterX + m_config.beamOriginXOffset * scaleX;
    float anchorY = chestCenterY + m_config.beamOriginYOffset * scaleY;

    // 1. Draw Chest Base Sprite (TreasureOpen_01..08) with bottom-center origin
    if(!m_openFrames.empty() && m_openFrameIndex < m_openFrames.size())
    {
        ConfigureSpriteBottomOrigin(m_chestSprite, m_openFrames[m_openFrameIndex]);
        m_chestSprite.setPosition(chestCenterX, chestBottomY);
        m_chestSprite.setScale(m_config.chestScale * scaleY, m_config.chestScale * scaleY);
        target.draw(m_chestSprite);
    }

    // 2. Draw Light Beam Reels & Upward Scrolling Item Icons (scissored to background panel inner boundary)
    if(m_chestOpened && m_beamTimer > 0.0f)
    {
        int beamCount = ctx.itemCount;
        if(beamCount <= 0) beamCount = 1;

        // Apply scissor padding offsets to sit cleanly inside the frame border:
        sf::Vector2f scissorPos = ctx.panelPos + sf::Vector2f(m_config.scissorLeftPadding * scaleX, m_config.scissorTopPadding * scaleY);
        sf::Vector2f scissorSize = ctx.panelSize - sf::Vector2f((m_config.scissorLeftPadding + m_config.scissorRightPadding) * scaleX, (m_config.scissorTopPadding + m_config.scissorBottomPadding) * scaleY);

        sf::Vector2i topLeftPixels = target.mapCoordsToPixel(scissorPos);
        sf::Vector2i bottomRightPixels = target.mapCoordsToPixel(scissorPos + scissorSize);

        int scissorX = std::min(topLeftPixels.x, bottomRightPixels.x);
        int scissorWidth = std::abs(bottomRightPixels.x - topLeftPixels.x);
        int scissorY = static_cast<int>(target.getSize().y) - std::max(topLeftPixels.y, bottomRightPixels.y);
        int scissorHeight = std::abs(bottomRightPixels.y - topLeftPixels.y);

        target.pushGLStates();
        glEnable(GL_SCISSOR_TEST);
        glScissor(scissorX, scissorY, scissorWidth, scissorHeight);

        constexpr float degToRad = 3.1415926535f / 180.0f;

        for(int i = 0; i < beamCount; ++i)
        {
            // Sequential leftmost-first reel growth calculation
            float beamStartTime = static_cast<float>(i) * m_config.beamStaggerDelay;
            float beamProgress = 0.0f;
            if(m_beamTimer > beamStartTime)
            {
                float t = (m_beamTimer - beamStartTime) / m_config.beamGrowthDuration;
                t = std::clamp(t, 0.0f, 1.0f);
                beamProgress = t * t * (3.0f - 2.0f * t); // Smoothstep curve
            }
            float currentHeight = m_config.beamMaxHeight * scaleY * beamProgress;

            if(currentHeight <= 0.0f)
            {
                continue; // Beam hasn't started growing yet
            }

            float offsetMultiplier = static_cast<float>(i) - static_cast<float>(beamCount - 1) * 0.5f;
            float angleDeg = offsetMultiplier * m_config.beamAngleOffset;
            float angleRad = angleDeg * degToRad;

            // Draw Light Beam Shaft using 3 distinct color sets (1-item, 3-item, 5-item)
            sf::Color bColor = m_config.beamColor;
            if(beamCount == 1)
            {
                bColor = m_config.colors1Item[0];
            }
            else if(beamCount == 3 && i >= 0 && i < 3)
            {
                bColor = m_config.colors3Item[i];
            }
            else if(beamCount >= 5 && i >= 0 && i < 5)
            {
                bColor = m_config.colors5Item[i];
            }
            else if(i >= 0 && i < 5)
            {
                bColor = m_config.colors5Item[i];
            }

            sf::RectangleShape beam(sf::Vector2f(m_config.beamWidth * scaleX, currentHeight));
            beam.setOrigin(m_config.beamWidth * scaleX * 0.5f, currentHeight); // Anchor at bottom center
            beam.setPosition(anchorX, anchorY);
            beam.setRotation(angleDeg);
            beam.setFillColor(bColor);
            target.draw(beam, sf::BlendAdd);

            // Draw Scrolling Weapon Icons moving upward along this reel (disappear when reward sequence triggered)
            if(!m_rewardTriggered && i < static_cast<int>(m_reelIcons.size()) && !m_reelIcons[i].empty())
            {
                const auto& reelPool = m_reelIcons[i];
                const std::size_t poolSize = reelPool.size();
                float spacing = m_config.reelIconSpacing * scaleY;
                float totalSpan = poolSize * spacing;

                for(std::size_t k = 0; k < poolSize; ++k)
                {
                    float baseDist = k * spacing + m_reelScrollOffset * scaleY;
                    float d = std::fmod(baseDist, totalSpan);

                    if(d > 0.0f && d <= currentHeight)
                    {
                        float iconX = anchorX + d * std::sin(angleRad);
                        float iconY = anchorY - d * std::cos(angleRad);

                        ConfigureSprite(m_reelIconSprite, reelPool[k]);
                        m_reelIconSprite.setPosition(iconX, iconY);
                        m_reelIconSprite.setRotation(0.0f); // Upright icon orientation
                        m_reelIconSprite.setScale(m_config.reelIconScale * scaleY, m_config.reelIconScale * scaleY);
                        target.draw(m_reelIconSprite);
                    }
                }
            }
        }

        glDisable(GL_SCISSOR_TEST);
        target.popGLStates();
        target.resetGLStates(); // Fix: flush SFML's stale texture cache after raw GL state manipulation
    }

    // 3. Draw Falling Gold & Silver Coins (UNCLIPPED - free to spill past background panel edges!)
    if(m_chestOpened)
    {
        for(const auto& coin : m_coins)
        {
            if(!coin.active || coin.delay > 0.0f)
            {
                continue;
            }

            const auto& animFrames = coin.isSilver ? m_silverCoinFrames : m_goldCoinFrames;
            if(!animFrames.empty() && coin.frameIndex < animFrames.size())
            {
                ConfigureSprite(m_coinSprite, animFrames[coin.frameIndex]);
                m_coinSprite.setPosition(coin.position);
                m_coinSprite.setRotation(coin.rotation);
                m_coinSprite.setScale(coin.scale * scaleY, coin.scale * scaleY);
                target.draw(m_coinSprite);
            }
        }
    }

    // 4. Draw Chest Front Lip Overlay (TreasureOpenFront_08) OVER beams & coins to hide bottom of beams/coins inside chest cavity!
    if(!m_openFrontFrames.empty())
    {
        std::size_t frontIdx = std::min(m_openFrameIndex, m_openFrontFrames.size() - 1);
        if(m_openFrontFrames[frontIdx].texture)
        {
            ConfigureSpriteBottomOrigin(m_chestFrontSprite, m_openFrontFrames[frontIdx]);
            m_chestFrontSprite.setPosition(chestCenterX, chestBottomY);
            m_chestFrontSprite.setScale(m_config.chestScale * scaleY, m_config.chestScale * scaleY);
            target.draw(m_chestFrontSprite);
        }
    }

    // 5. Draw Phase 2 Coin Count-Up Counter (number text + static coin-spin-gold_01 icon to its right)
    if(m_chestOpened)
    {
        std::string goldStr = std::to_string(m_currentGold);
        m_goldCounterText.setFont(ctx.font);
        m_goldCounterText.setString(goldStr);
        m_goldCounterText.setCharacterSize(static_cast<unsigned int>(std::round(44.0f * scaleY)));
        m_goldCounterText.setFillColor(sf::Color::White);
        // m_goldCounterText.setOutlineColor(sf::Color(20, 20, 20));
        // m_goldCounterText.setOutlineThickness(2.0f * scaleY);

        sf::FloatRect textBounds = m_goldCounterText.getLocalBounds();
        float textWidth = textBounds.width;
        float textHeight = textBounds.height;

        float iconSpacing = 10.0f * scaleX;
        float iconSize = 46.0f * scaleY;

        float totalWidth = textWidth + iconSpacing + iconSize;
        float startX = ctx.viewCenter.x - totalWidth * 0.5f;
        float counterY = ctx.panelPos.y + ctx.panelSize.y - 115.0f * scaleY;

        // Position text centered vertically on counterY
        m_goldCounterText.setPosition(startX - textBounds.left, counterY - textBounds.top - textBounds.height * 0.5f);
        target.draw(m_goldCounterText);

        if(m_goldCoinIconData.texture && m_goldCoinIconData.rect.width > 0 && m_goldCoinIconData.rect.height > 0)
        {
            ConfigureSprite(m_goldCoinIconSprite, m_goldCoinIconData);
            m_goldCoinIconSprite.setOrigin(
                static_cast<float>(m_goldCoinIconData.rect.width) * 0.5f,
                static_cast<float>(m_goldCoinIconData.rect.height) * 0.5f);
            float iconX = startX + textWidth + iconSpacing + iconSize * 0.5f;
            float iconY = counterY;
            m_goldCoinIconSprite.setPosition(iconX, iconY);
            float iconScale = (iconSize / static_cast<float>(m_goldCoinIconData.rect.height));
            m_goldCoinIconSprite.setScale(iconScale, iconScale);
            target.draw(m_goldCoinIconSprite);
        }
    }

    // 6. Draw Per-Reel PrizeBG Cards & VFX Impact Bursts (pops in TriggerReward in Phase 2)
    if(m_rewardTriggered && m_prizeBgData.texture)
    {
        for(const auto& card : m_prizeCards)
        {
            if(!card.popStarted) continue;

            float popProgress = std::clamp(card.popTimer / m_config.cardConfig.cardPopDuration, 0.0f, 1.0f);
            float smoothPop = popProgress * popProgress * (3.0f - 2.0f * popProgress); // Smoothstep curve
            float cardScaleCurrent = m_config.cardConfig.cardScale * scaleY * smoothPop;

            ConfigureSprite(m_prizeBgSprite, m_prizeBgData);
            m_prizeBgSprite.setPosition(card.position);
            m_prizeBgSprite.setOrigin(
                static_cast<float>(m_prizeBgData.rect.width) * 0.5f,
                static_cast<float>(m_prizeBgData.rect.height) * 0.5f); // Center origin
            m_prizeBgSprite.setScale(cardScaleCurrent, cardScaleCurrent);
            target.draw(m_prizeBgSprite);

            // Draw Item Icon center-aligned on PrizeBG card
            if(!card.rewardData.iconKey.empty())
            {
                std::string keyNoExt = card.rewardData.iconKey;
                if(keyNoExt.size() > 4 && keyNoExt.substr(keyNoExt.size() - 4) == ".png")
                {
                    keyNoExt = keyNoExt.substr(0, keyNoExt.size() - 4);
                }

                AssetTextureData iconData = ctx.atlas.GetTextureData(keyNoExt);
                if(!iconData.texture) iconData = ctx.atlas.GetTextureData(card.rewardData.iconKey);

                if(iconData.texture && iconData.rect.width > 0 && iconData.rect.height > 0)
                {
                    ConfigureSprite(m_reelIconSprite, iconData);
                    m_reelIconSprite.setPosition(card.position);
                    m_reelIconSprite.setOrigin(
                        static_cast<float>(iconData.rect.width) * 0.5f,
                        static_cast<float>(iconData.rect.height) * 0.5f); // Center origin
                    float itemScale = cardScaleCurrent * m_config.cardConfig.iconScaleOnCard;
                    m_reelIconSprite.setScale(itemScale, itemScale);
                    target.draw(m_reelIconSprite);
                }
            }

            // Draw center-aligned VFX Impact Burst (HitStar1 + sPFX_ring_64)
            if(card.vfxActive)
            {
                float vfxProgress = std::clamp(card.vfxTimer / m_config.cardConfig.vfxDuration, 0.0f, 1.0f);
                float currentVfxScale = (m_config.cardConfig.vfxStartScale + (m_config.cardConfig.vfxEndScale - m_config.cardConfig.vfxStartScale) * vfxProgress) * scaleY;
                float currentRotation = vfxProgress * m_config.cardConfig.vfxRotationSpeed;
                sf::Uint8 vfxAlpha = static_cast<sf::Uint8>(std::round(255.0f * (1.0f - vfxProgress)));

                if(m_hitStarData.texture)
                {
                    ConfigureSprite(m_hitStarSprite, m_hitStarData);
                    m_hitStarSprite.setPosition(card.position);
                    m_hitStarSprite.setOrigin(
                        static_cast<float>(m_hitStarData.rect.width) * 0.5f,
                        static_cast<float>(m_hitStarData.rect.height) * 0.5f);
                    m_hitStarSprite.setScale(currentVfxScale, currentVfxScale);
                    m_hitStarSprite.setRotation(currentRotation);
                    m_hitStarSprite.setColor(sf::Color(255, 255, 255, vfxAlpha));
                    target.draw(m_hitStarSprite, sf::BlendAdd);
                }

                if(m_ringVfxData.texture)
                {
                    ConfigureSprite(m_ringVfxSprite, m_ringVfxData);
                    m_ringVfxSprite.setPosition(card.position);
                    m_ringVfxSprite.setOrigin(
                        static_cast<float>(m_ringVfxData.rect.width) * 0.5f,
                        static_cast<float>(m_ringVfxData.rect.height) * 0.5f);
                    m_ringVfxSprite.setScale(currentVfxScale * 1.1f, currentVfxScale * 1.1f);
                    m_ringVfxSprite.setRotation(-currentRotation * 0.5f);
                    m_ringVfxSprite.setColor(sf::Color(255, 255, 255, vfxAlpha));
                    target.draw(m_ringVfxSprite, sf::BlendAdd);
                }
            }
        }
    }
}

void TreasurePhase2OpeningState::OnExit(TreasureRewardViewContext& /*ctx*/)
{
}

void TreasurePhase2OpeningState::ConfigureSprite(sf::Sprite& sprite, const AssetTextureData& data) const
{
    if(!data.texture)
    {
        return;
    }
    sprite.setTexture(*data.texture, false);
    sprite.setTextureRect(data.rect);
    sprite.setOrigin(
        static_cast<float>(data.rect.width) * 0.5f,
        static_cast<float>(data.rect.height) * 0.5f);
}

void TreasurePhase2OpeningState::ConfigureSpriteBottomOrigin(sf::Sprite& sprite, const AssetTextureData& data) const
{
    if(!data.texture)
    {
        return;
    }
    sprite.setTexture(*data.texture, false);
    sprite.setTextureRect(data.rect);
    sprite.setOrigin(
        static_cast<float>(data.rect.width) * 0.5f,
        static_cast<float>(data.rect.height)); // Bottom-center origin
}

void TreasurePhase2OpeningState::PopulateIconPool(TextureAtlas& atlas)
{
    m_iconPool.clear();
    const std::vector<std::string> candidateKeys = {
        "Whip", "Whip2",
        "WandHoly", "WandHoly2",
        "Knife", "Knife2",
        "Axe", "Scythe",
        "Cross", "HeavenSword",
        "HolyBook", "UnholyBook",
        "WandFire", "Hellfire",
        "Garlic", "OrbOrange",
        "HolyWater", "Water2",
        "Diamond2", "Carnage",
        "LightningRing", "SongOfMana", "Song"
    };

    for(const auto& key : candidateKeys)
    {
        if(key.length() >= 4 && key.substr(key.length() - 4) == ".png")
        {
            continue; // Skip any string ending with .png
        }
        AssetTextureData data = atlas.GetTextureData(key);
        if(data.texture && data.rect.width > 0 && data.rect.height > 0)
        {
            m_iconPool.push_back(data);
        }
    }
}

void TreasurePhase2OpeningState::InitCoinBurst(TextureAtlas& atlas, const sf::Vector2f& chestCenter, float scaleX, float scaleY)
{
    m_goldCoinFrames.clear();
    m_silverCoinFrames.clear();

    std::cout << "\n================ COIN SPRITE DEBUG INFO ================\n";
    for(int i = 1; i <= 5; ++i)
    {
        std::string suffix = "0" + std::to_string(i);
        std::string goldKey = "coin-spin-gold_" + suffix;
        std::string silverKey = "coin-spin-silver_" + suffix;

        std::string goldTexId, silverTexId;
        sf::IntRect goldRect, silverRect;
        bool hasGold = atlas.GetRegionInfo(goldKey, goldTexId, goldRect);
        bool hasSilver = atlas.GetRegionInfo(silverKey, silverTexId, silverRect);

        AssetTextureData goldData = atlas.GetTextureData(goldKey);
        AssetTextureData silverData = atlas.GetTextureData(silverKey);
        if(goldData.texture) m_goldCoinFrames.push_back(goldData);
        if(silverData.texture) m_silverCoinFrames.push_back(silverData);

        if(hasGold && goldData.texture)
        {
            unsigned int texH = goldData.texture->getSize().y;
            int origY = static_cast<int>(texH) - goldRect.top - goldRect.height;
            std::cout << "[GOLD  0" << i << "] Key: " << goldKey 
                      << " | Spritesheet: " << goldTexId << " (" << goldData.texture->getSize().x << "x" << texH << ")"
                      << " | JSON (x,y,w,h): (" << goldRect.left << ", " << origY << ", " << goldRect.width << ", " << goldRect.height << ")"
                      << " | SFML Rect (left,top,w,h): (" << goldRect.left << ", " << goldRect.top << ", " << goldRect.width << ", " << goldRect.height << ")\n";
        }
        else
        {
            std::cout << "[GOLD  0" << i << "] Key: " << goldKey << " NOT FOUND IN ATLAS!\n";
        }

        if(hasSilver && silverData.texture)
        {
            unsigned int texH = silverData.texture->getSize().y;
            int origY = static_cast<int>(texH) - silverRect.top - silverRect.height;
            std::cout << "[SILVER 0" << i << "] Key: " << silverKey 
                      << " | Spritesheet: " << silverTexId << " (" << silverData.texture->getSize().x << "x" << texH << ")"
                      << " | JSON (x,y,w,h): (" << silverRect.left << ", " << origY << ", " << silverRect.width << ", " << silverRect.height << ")"
                      << " | SFML Rect (left,top,w,h): (" << silverRect.left << ", " << silverRect.top << ", " << silverRect.width << ", " << silverRect.height << ")\n";
        }
        else
        {
            std::cout << "[SILVER 0" << i << "] Key: " << silverKey << " NOT FOUND IN ATLAS!\n";
        }
    }
    std::cout << "========================================================\n\n" << std::flush;

    if(m_goldCoinFrames.empty())
    {
        AssetTextureData goldFallback = atlas.GetTextureData("CoinGold");
        if(!goldFallback.texture) goldFallback = atlas.GetTextureData("CoinGold.png");
        if(goldFallback.texture) m_goldCoinFrames.push_back(goldFallback);
    }

    if(m_silverCoinFrames.empty())
    {
        AssetTextureData silverFallback = atlas.GetTextureData("CoinSilver");
        if(!silverFallback.texture) silverFallback = atlas.GetTextureData("CoinSilver.png");
        if(silverFallback.texture) m_silverCoinFrames.push_back(silverFallback);
    }

    m_coins.clear();
    m_coins.reserve(m_config.coinBurstCount);

    m_coinRng.seed(1337 + static_cast<unsigned int>(chestCenter.x));
    std::uniform_real_distribution<float> initialDelayDist(0.0f, 1.5f);

    float anchorX = chestCenter.x + m_config.beamOriginXOffset * scaleX;
    float anchorY = chestCenter.y + m_config.beamOriginYOffset * scaleY;
    sf::Vector2f anchor(anchorX, anchorY);

    for(std::size_t i = 0; i < m_config.coinBurstCount; ++i)
    {
        FallingCoinParticle coin;
        RespawnCoin(coin, anchor, scaleY);
        coin.delay = initialDelayDist(m_coinRng); // Stagger initial emission across 1.5s
        m_coins.push_back(coin);
    }
}

void TreasurePhase2OpeningState::RespawnCoin(FallingCoinParticle& coin, const sf::Vector2f& anchor, float scaleY)
{
    constexpr float degToRad = 3.1415926535f / 180.0f;
    std::uniform_real_distribution<float> angleDist(m_config.coinAngleMinDeg * degToRad, m_config.coinAngleMaxDeg * degToRad);
    std::uniform_real_distribution<float> speedDist(m_config.coinEjectionSpeedMin, m_config.coinEjectionSpeedMax);
    std::uniform_real_distribution<float> scaleDist(m_config.coinScaleMin, m_config.coinScaleMax);
    std::uniform_real_distribution<float> rotDist(0.0f, 360.0f);
    std::uniform_real_distribution<float> angVelDist(-450.0f, 450.0f);
    std::uniform_real_distribution<float> frameDurDist(0.04f, 0.08f);
    std::uniform_real_distribution<float> coinTypeDist(0.0f, 1.0f);

    coin.position = anchor;
    float angle = angleDist(m_coinRng);
    float speed = speedDist(m_coinRng) * scaleY;
    coin.velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
    coin.rotation = rotDist(m_coinRng);
    coin.angularVelocity = angVelDist(m_coinRng);
    coin.scale = scaleDist(m_coinRng);
    coin.frameDuration = frameDurDist(m_coinRng);
    coin.isSilver = (coinTypeDist(m_coinRng) < m_config.silverCoinRatio);
    coin.life = 0.0f;
    coin.maxLife = 2.5f;

    const auto& animFrames = coin.isSilver ? m_silverCoinFrames : m_goldCoinFrames;
    if(!animFrames.empty())
    {
        std::uniform_int_distribution<std::size_t> frameDist(0, animFrames.size() - 1);
        coin.frameIndex = frameDist(m_coinRng);
        coin.active = true;
    }
}

void TreasurePhase2OpeningState::InitRisingEllipses(const TreasureRewardViewContext& ctx)
{
    m_risingEllipses.clear();
    m_risingEllipses.resize(m_config.risingEllipseCount);

    float bottomY = ctx.viewCenter.y + ctx.viewSize.y * 0.5f;

    for(std::size_t i = 0; i < m_risingEllipses.size(); ++i)
    {
        RespawnEllipse(m_risingEllipses[i], ctx, true);
        // Stagger initial Y positions below lower edge of view so they float up sequentially from the bottom
        m_risingEllipses[i].position.y = bottomY + m_risingEllipses[i].radius.y + 20.0f + static_cast<float>(i) * 110.0f;
    }
}

void TreasurePhase2OpeningState::RespawnEllipse(RisingLightEllipse& ellipse, const TreasureRewardViewContext& ctx, bool /*atBottom*/)
{
    float scaleY = ctx.panelSize.y / 860.0f;
    float bottomY = ctx.viewCenter.y + ctx.viewSize.y * 0.5f;

    std::uniform_real_distribution<float> radXDist(m_config.ellipseBaseRadiusMin, m_config.ellipseBaseRadiusMax);
    std::uniform_real_distribution<float> aspectDist(m_config.ellipseAspectRatioMin, m_config.ellipseAspectRatioMax);
    std::uniform_real_distribution<float> speedDist(m_config.ellipseSpeedYMin, m_config.ellipseSpeedYMax);
    std::uniform_real_distribution<float> phaseDist(0.0f, 6.2831853f);
    std::uniform_real_distribution<float> xDist(-ctx.viewSize.x * 0.45f, ctx.viewSize.x * 0.45f);

    float baseRadiusX = radXDist(m_coinRng) * scaleY;
    float aspect = aspectDist(m_coinRng);

    ellipse.radius.x = baseRadiusX;
    ellipse.radius.y = baseRadiusX * aspect; // Always proportionally squeezed vertically
    ellipse.speedY = speedDist(m_coinRng) * scaleY;
    ellipse.pulsePhase = phaseDist(m_coinRng);

    ellipse.position.x = ctx.viewCenter.x + xDist(m_coinRng);
    ellipse.position.y = bottomY + ellipse.radius.y + 20.0f; // Always spawn below lower edge of view

    ellipse.color = m_config.ellipseColor;
}

void TreasurePhase2OpeningState::TriggerRewardSequence()
{
    if(m_rewardTriggered) return;
    m_rewardTriggered = true;
    m_tallyFinished = true;
    m_currentGold = m_targetGold;
    m_goldProgress = static_cast<float>(m_targetGold);
    m_fadeOutTimer = 0.0f;
}
