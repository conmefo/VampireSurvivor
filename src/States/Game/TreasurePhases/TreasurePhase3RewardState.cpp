#include "TreasurePhase3RewardState.h"
#include <SFML/OpenGL.hpp>
#include <cmath>
#include <algorithm>
#include <string>

void TreasurePhase3RewardState::OnEnter(TreasureRewardViewContext& ctx)
{
    m_chestOpenFrame = ctx.atlas.GetTextureData("TreasureOpen_08");
    if(!m_chestOpenFrame.texture) m_chestOpenFrame = ctx.atlas.GetTextureData("TreasureOpen_08.png");

    m_chestOpenFrontFrame = ctx.atlas.GetTextureData("TreasureOpenFront_08");
    if(!m_chestOpenFrontFrame.texture) m_chestOpenFrontFrame = ctx.atlas.GetTextureData("TreasureOpenFront_08.png");

    m_prizeBgData = ctx.atlas.GetTextureData("PrizeBG");
    if(!m_prizeBgData.texture) m_prizeBgData = ctx.atlas.GetTextureData("PrizeBG.png");

    m_hitStarData = ctx.atlas.GetTextureData("HitStar1");
    if(!m_hitStarData.texture) m_hitStarData = ctx.atlas.GetTextureData("HitStar1.png");

    m_ringVfxData = ctx.atlas.GetTextureData("sPFX_ring_64");
    if(!m_ringVfxData.texture) m_ringVfxData = ctx.atlas.GetTextureData("sPFX_ring_64.png");

    m_goldCoinIconData = ctx.atlas.GetTextureData("coin-spin-gold_01");
    if(!m_goldCoinIconData.texture) m_goldCoinIconData = ctx.atlas.GetTextureData("coin-spin-gold_01.png");

    // Setup "DONE" button & arrows (matching Phase 1 OPEN button styling)
    m_doneButtonBg.SetTexture(ctx.atlas, "button_c9_mouseover");
    m_doneButtonBg.SetMargins(10.0f, 10.0f, 10.0f, 10.0f);
    m_doneButtonBg.SetCornerScale(2.0f);

    m_doneButtonText.setFont(ctx.font);
    m_doneButtonText.setString("DONE");
    m_doneButtonText.setCharacterSize(28);
    m_doneButtonText.setFillColor(sf::Color::White);

    m_arrowAnimData.name = "arrow_cursor_spin";
    m_arrowAnimData.frameDuration = m_config.arrowFrameDuration;
    m_arrowAnimData.isLooping = true;
    m_arrowAnimData.frames.clear();
    for(int i = 1; i <= 8; ++i)
    {
        AssetTextureData data = ctx.atlas.GetTextureData("arrow_0" + std::to_string(i));
        if(data.texture)
        {
            m_arrowAnimData.frames.push_back(data);
        }
    }
    if(!m_arrowAnimData.IsEmpty())
    {
        m_arrowAnimator.Play(&m_arrowAnimData, true);
        m_arrowAnimator.SetOriginMode(AnimationOriginMode::CenterEachFrame);
    }

    // Initialize per-reel Prize Cards
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
            const std::vector<std::string> defaults = {"WandFire", "Garlic", "Whip", "HolyWater", "LightningRing"};
            m_prizeCards[i].rewardData.iconKey = defaults[i % defaults.size()];
            m_prizeCards[i].rewardData.itemName = defaults[i % defaults.size()];
        }
    }

    m_phaseTimer = 0.0f;
    m_doneAppearTimer = 0.0f;
    m_doneButtonActive = false;

    m_detailCardWidget.Init(ctx.atlas);
    m_currentCardIndex = 0;
    if(!m_prizeCards.empty())
    {
        m_detailCardWidget.SetCardInfo(m_prizeCards[0].rewardData, 0, static_cast<int>(m_prizeCards.size()), ctx.atlas);
    }

    UpdateLayout(ctx.viewSize, ctx.viewCenter, ctx);
}

void TreasurePhase3RewardState::HandleEvent(const sf::Event& event, const sf::RenderWindow* window, TreasureRewardViewContext& ctx)
{
    if(!m_doneButtonActive)
    {
        return;
    }

    bool triggered = false;
    if(event.type == sf::Event::KeyPressed &&
       (event.key.code == sf::Keyboard::Enter ||
        event.key.code == sf::Keyboard::Space ||
        event.key.code == sf::Keyboard::Escape))
    {
        triggered = true;
    }
    else if(event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left)
    {
        if(window)
        {
            sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
            sf::Vector2f worldPos = window->mapPixelToCoords(mousePos);
            if(m_doneButtonBounds.contains(worldPos))
            {
                triggered = true;
            }
        }
        else
        {
            triggered = true;
        }
    }

    if(triggered)
    {
        if(m_detailCardWidget.IsIdle())
        {
            m_detailCardWidget.StartShrink();
        }
        else if(m_currentCardIndex + 1 >= m_prizeCards.size() && (m_detailCardWidget.IsShrinking() || m_detailCardWidget.IsHidden()))
        {
            if(ctx.finish) ctx.finish();
        }
    }
}

void TreasurePhase3RewardState::Update(float dt, TreasureRewardViewContext& ctx)
{
    m_phaseTimer += dt;

    // Update per-reel Prize Cards pop & VFX timers
    for(std::size_t i = 0; i < m_prizeCards.size(); ++i)
    {
        auto& card = m_prizeCards[i];
        float startTime = static_cast<float>(i) * m_config.cardConfig.cardStaggerDelay;

        if(m_phaseTimer >= startTime && !card.popStarted)
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

    // Update "DONE" button appear timer & Detail Card
    if(m_phaseTimer >= m_config.doneAppearDelay)
    {
        m_doneButtonActive = true;
        m_doneAppearTimer += dt;

        if(m_detailCardWidget.IsHidden() && m_currentCardIndex < m_prizeCards.size())
        {
            m_detailCardWidget.StartGrow();
        }
    }

    bool wasShrinking = m_detailCardWidget.IsShrinking();
    m_detailCardWidget.Update(dt);

    if(wasShrinking && m_detailCardWidget.IsHidden())
    {
        m_currentCardIndex++;
        if(m_currentCardIndex < m_prizeCards.size())
        {
            m_detailCardWidget.SetCardInfo(m_prizeCards[m_currentCardIndex].rewardData, static_cast<int>(m_currentCardIndex), static_cast<int>(m_prizeCards.size()), ctx.atlas);
            m_detailCardWidget.StartGrow();
        }
        else
        {
            if(ctx.finish) ctx.finish();
        }
    }

    if(!m_arrowAnimData.IsEmpty())
    {
        m_arrowAnimator.Update(dt, m_leftArrow);
        if(m_leftArrow.getTexture())
        {
            m_rightArrow.setTexture(*m_leftArrow.getTexture());
            m_rightArrow.setTextureRect(m_leftArrow.getTextureRect());
            m_rightArrow.setOrigin(m_leftArrow.getOrigin());
        }
    }

    UpdateLayout(ctx.viewSize, ctx.viewCenter, ctx);
}

void TreasurePhase3RewardState::UpdateLayout(const sf::Vector2f& viewSize, const sf::Vector2f& viewCenter, TreasureRewardViewContext& ctx)
{
    float scaleX = viewSize.x / 1920.0f;
    float scaleY = viewSize.y / 1080.0f;

    ctx.viewSize = viewSize;
    ctx.viewCenter = viewCenter;
    ctx.panelSize = sf::Vector2f(640.0f * scaleX, 860.0f * scaleY);
    ctx.panelPos = viewCenter - ctx.panelSize / 2.0f;

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

    // Update "DONE" Button Layout (Reverse of Phase 1 OPEN button)
    float btnW = m_config.buttonWidth * scaleX;
    float btnH = m_config.buttonHeight * scaleY;
    sf::Vector2f btnCenter(viewCenter.x, ctx.panelPos.y + m_config.buttonYOffset * scaleY);
    sf::Vector2f btnPos = btnCenter - sf::Vector2f(btnW * 0.5f, btnH * 0.5f);

    m_doneButtonBg.SetSize(sf::Vector2f(btnW, btnH));
    m_doneButtonBg.setOrigin(btnW * 0.5f, btnH * 0.5f); // Center origin for spin scale-up!
    m_doneButtonBg.setPosition(btnCenter);
    m_doneButtonBg.Update();

    CenterText(m_doneButtonText, btnCenter.x, btnCenter.y);
    m_doneButtonBounds = sf::FloatRect(btnPos.x, btnPos.y, btnW, btnH);

    // Arrow cursors
    float pad = m_config.arrowPadding * scaleX;
    m_leftArrow.setPosition(btnPos.x - pad, btnCenter.y);
    m_leftArrow.setScale(m_config.arrowScale * scaleY, m_config.arrowScale * scaleY);

    m_rightArrow.setPosition(btnPos.x + btnW + pad, btnCenter.y);
    m_rightArrow.setScale(-m_config.arrowScale * scaleY, m_config.arrowScale * scaleY);

    m_detailCardWidget.UpdateLayout(viewSize, viewCenter, ctx.panelPos, ctx.panelSize);
}

void TreasurePhase3RewardState::Draw(sf::RenderTarget& target, const TreasureRewardViewContext& ctx) const
{
    float scaleX = ctx.panelSize.x / 640.0f;
    float scaleY = ctx.panelSize.y / 860.0f;
    float chestCenterX = ctx.viewCenter.x;
    float chestCenterY = ctx.panelPos.y + m_config.chestYOffset * scaleY;
    float chestBottomY = chestCenterY + 11.0f * m_config.chestScale * scaleY;
    float anchorX = chestCenterX + m_config.beamOriginXOffset * scaleX;
    float anchorY = chestCenterY + m_config.beamOriginYOffset * scaleY;

    // 1. Draw Open Chest Base Frame
    if(m_chestOpenFrame.texture)
    {
        ConfigureSprite(m_chestSprite, m_chestOpenFrame);
        m_chestSprite.setPosition(chestCenterX, chestBottomY);
        m_chestSprite.setScale(m_config.chestScale * scaleY, m_config.chestScale * scaleY);
        target.draw(m_chestSprite);
    }

    // 2. Draw Light Beam Reels (standing upright behind prize cards)
    {
        int beamCount = std::max(1, ctx.itemCount);
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

        float shrinkProgress = m_doneButtonActive ? std::clamp(m_doneAppearTimer / std::max(0.01f, m_config.doneAppearDuration), 0.0f, 1.0f) : 0.0f;
        float currentBeamWidth = m_config.beamWidth * scaleX * (1.0f - shrinkProgress);

        if(currentBeamWidth > 0.1f)
        {
            for(int i = 0; i < beamCount; ++i)
            {
                float offsetMultiplier = static_cast<float>(i) - static_cast<float>(beamCount - 1) * 0.5f;
                float angleDeg = offsetMultiplier * m_config.beamAngleOffset;
                float currentHeight = m_config.beamMaxHeight * scaleY;

                sf::Color bColor = m_config.colors1Item[0];
                if(beamCount == 3 && i >= 0 && i < 3) bColor = m_config.colors3Item[i];
                else if(beamCount >= 5 && i >= 0 && i < 5) bColor = m_config.colors5Item[i];

                sf::RectangleShape beam(sf::Vector2f(currentBeamWidth, currentHeight));
                beam.setOrigin(currentBeamWidth * 0.5f, currentHeight);
                beam.setPosition(anchorX, anchorY);
                beam.setRotation(angleDeg);
                beam.setFillColor(bColor);
                target.draw(beam, sf::BlendAdd);
            }
        }

        glDisable(GL_SCISSOR_TEST);
        target.popGLStates();
        target.resetGLStates();
    }

    // 3. Draw Top Gold Counter (Gold text without outline + static coin-spin-gold_01 icon aligned on same baseline)
    {
        std::string goldStr = std::to_string(ctx.goldReward);
        m_goldCounterText.setFont(ctx.font);
        m_goldCounterText.setString(goldStr);
        m_goldCounterText.setCharacterSize(static_cast<unsigned int>(std::round(44.0f * scaleY)));
        m_goldCounterText.setFillColor(sf::Color(255, 220, 40));
        m_goldCounterText.setOutlineThickness(0.0f); // No outline as requested!

        sf::FloatRect textBounds = m_goldCounterText.getLocalBounds();
        float textWidth = textBounds.width;
        float textHeight = textBounds.height;

        float iconSpacing = 12.0f * scaleX;
        float iconSize = 46.0f * scaleY;

        float totalWidth = textWidth + iconSpacing + iconSize;
        float startX = ctx.viewCenter.x - totalWidth * 0.5f;
        float counterY = ctx.panelPos.y + 45.0f * scaleY;

        // Set origin of text to its local center-y for precise baseline alignment
        m_goldCounterText.setOrigin(textBounds.left, textBounds.top + textHeight * 0.5f);
        m_goldCounterText.setPosition(startX, counterY);
        target.draw(m_goldCounterText);

        if(m_goldCoinIconData.texture)
        {
            ConfigureSprite(m_goldCoinIconSprite, m_goldCoinIconData);
            m_goldCoinIconSprite.setOrigin(
                static_cast<float>(m_goldCoinIconData.rect.width) * 0.5f,
                static_cast<float>(m_goldCoinIconData.rect.height) * 0.5f); // Center origin

            float iconX = startX + textWidth + iconSpacing + iconSize * 0.5f;
            m_goldCoinIconSprite.setPosition(iconX, counterY); // Exact same Y baseline line!
            float iconScale = (iconSize / static_cast<float>(m_goldCoinIconData.rect.height));
            m_goldCoinIconSprite.setScale(iconScale, iconScale);
            target.draw(m_goldCoinIconSprite);
        }
    }

    // 3. Draw Per-Reel PrizeBG Cards (rendered statically at full size since pop animation played in Phase 2)
    if(m_prizeBgData.texture)
    {
        for(const auto& card : m_prizeCards)
        {
            float cardScaleCurrent = m_config.cardConfig.cardScale * scaleY;

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
                    sf::Sprite itemIconSprite;
                    ConfigureSprite(itemIconSprite, iconData);
                    itemIconSprite.setPosition(card.position);
                    itemIconSprite.setOrigin(
                        static_cast<float>(iconData.rect.width) * 0.5f,
                        static_cast<float>(iconData.rect.height) * 0.5f); // Center origin
                    float itemScale = cardScaleCurrent * m_config.cardConfig.iconScaleOnCard;
                    itemIconSprite.setScale(itemScale, itemScale);
                    target.draw(itemIconSprite);
                }
            }
        }
    }

    // 5. Draw Foreground Chest Lip Overlay (TreasureOpenFront_08)
    if(m_chestOpenFrontFrame.texture)
    {
        ConfigureSprite(m_chestFrontSprite, m_chestOpenFrontFrame);
        m_chestFrontSprite.setPosition(ctx.viewCenter.x, chestBottomY);
        m_chestFrontSprite.setScale(m_config.chestScale * scaleY, m_config.chestScale * scaleY);
        target.draw(m_chestFrontSprite);
    }

    // 6. Draw "DONE" Button with Appear Effect (rotate + scale UP - reverse of OPEN button exit animation)
    if(m_doneButtonActive)
    {
        float t = std::clamp(m_doneAppearTimer / m_config.doneAppearDuration, 0.0f, 1.0f);
        float smoothT = t * t * (3.0f - 2.0f * t); // Smoothstep curve
        float scale = smoothT;
        float angle = (1.0f - smoothT) * -180.0f;

        if(scale > 0.0f)
        {
            sf::Vector2f btnCenter(ctx.viewCenter.x, ctx.panelPos.y + m_config.buttonYOffset * scaleY);

            m_doneButtonBg.setPosition(btnCenter);
            m_doneButtonBg.setRotation(angle);
            m_doneButtonBg.setScale(scale, scale);
            target.draw(m_doneButtonBg);

            CenterText(m_doneButtonText, btnCenter.x, btnCenter.y);
            m_doneButtonText.setRotation(angle);
            m_doneButtonText.setScale(scale, scale);
            target.draw(m_doneButtonText);

            if(m_leftArrow.getTexture() && m_rightArrow.getTexture())
            {
                m_leftArrow.setRotation(angle);
                m_leftArrow.setScale(m_config.arrowScale * scaleY * scale, m_config.arrowScale * scaleY * scale);
                target.draw(m_leftArrow);

                m_rightArrow.setRotation(angle);
                m_rightArrow.setScale(-m_config.arrowScale * scaleY * scale, m_config.arrowScale * scaleY * scale);
                target.draw(m_rightArrow);
            }
        }
    }

    // 7. Draw Bottom Treasure Item Detail Card (frame1_c2, top-edge grow/shrink)
    m_detailCardWidget.Draw(target, ctx.font);
}

void TreasurePhase3RewardState::OnExit(TreasureRewardViewContext& /*ctx*/)
{
}

void TreasurePhase3RewardState::ConfigureSprite(sf::Sprite& sprite, const AssetTextureData& data) const
{
    if(!data.texture)
    {
        return;
    }
    sprite.setTexture(*data.texture, false);
    sprite.setTextureRect(data.rect);
    sprite.setOrigin(
        static_cast<float>(data.rect.width) * 0.5f,
        static_cast<float>(data.rect.height)); // Bottom-center origin by default
}

void TreasurePhase3RewardState::CenterText(sf::Text& text, float x, float y) const
{
    const sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(
        std::floor(bounds.left + bounds.width * 0.5f),
        std::floor(bounds.top + bounds.height * 0.5f));
    text.setPosition(std::floor(x), std::floor(y));
}
