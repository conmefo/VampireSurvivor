#include "SimpleTextLevelUpView.h"
#include "../../Core/Resources/TextureAtlas.h"
#include <cmath>
#include <iostream>

SimpleTextLevelUpView::SimpleTextLevelUpView(const TextureAtlas& atlas, const sf::Font& font)
    : m_atlas(atlas)
    , m_font(font)
{
    m_dimOverlay.setFillColor(sf::Color(0, 0, 0, 200));

    // Outer level up panel frame: frame5_c4
    m_nineSliceBg.SetTexture(atlas, "frame5_c4");
    m_nineSliceBg.SetMargins(12.0f, 12.0f, 12.0f, 12.0f);
    m_nineSliceBg.SetCornerScale(1.4f);

    m_titleText.setFont(m_font);
    m_titleText.setString("LEVEL UP!");
    m_titleText.setCharacterSize(38);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setOutlineColor(sf::Color::White);
    m_titleText.setOutlineThickness(0.7f);

    m_subtitleText.setFont(m_font);
    m_subtitleText.setCharacterSize(20);
    m_subtitleText.setFillColor(sf::Color::White);
    m_subtitleText.setOutlineColor(sf::Color::White);
    m_subtitleText.setOutlineThickness(0.7f);

    m_controlsText.setFont(m_font);
    m_controlsText.setCharacterSize(18);
    m_controlsText.setFillColor(sf::Color::White);
    m_controlsText.setOutlineColor(sf::Color::White);
    m_controlsText.setOutlineThickness(0.7f);

    m_luckTipText.setFont(m_font);
    m_luckTipText.setFillColor(sf::Color::White);
    m_luckTipText.setOutlineColor(sf::Color::White);
    m_luckTipText.setOutlineThickness(0.5f);

    InitVfxAnimator();
    InitFallingGems();
}

void SimpleTextLevelUpView::InitVfxAnimator()
{
    m_burstAnimData.name = "level_up_vfx_burst";
    m_burstAnimData.isLooping = false; // Film 6 sprites once, no loop
    m_burstAnimData.frames.clear();

    for (int i = 1; i <= 6; ++i)
    {
        AssetTextureData data = m_atlas.GetTextureData("Burst" + std::to_string(i));
        if (data.texture)
        {
            m_burstAnimData.frames.push_back(data);
        }
    }

    m_vfxAnimator.SetOriginMode(AnimationOriginMode::CenterEachFrame);
}

void SimpleTextLevelUpView::UpdateChoices(
    const std::vector<LevelUpOption>& choices,
    int pendingLevelUps,
    int rerollCharges,
    int skipCharges,
    int banishCharges)
{
    m_rerollCharges = rerollCharges;
    m_skipCharges = skipCharges;
    m_banishCharges = banishCharges;

    if (pendingLevelUps > 1)
    {
        m_subtitleText.setString("Choose an upgrade (" + std::to_string(pendingLevelUps) + " pending):");
    }
    else
    {
        m_subtitleText.setString("Choose an upgrade:");
    }

    // Display Luck tip if current turn has less than 4 choices
    if (choices.size() < 4)
    {
        m_luckTipText.setString("Increase your Luck for a chance to get 4 choices");
    }
    else
    {
        m_luckTipText.setString("");
    }

    m_cards.clear();

    for (size_t i = 0; i < choices.size(); ++i)
    {
        LevelUpItemCard card(m_atlas, m_font);
        card.SetOptionData(choices[i], static_cast<int>(i), banishCharges > 0);
        m_cards.push_back(std::move(card));
    }

    std::string controlsStr = "Hotkeys: [1-" + std::to_string(choices.size()) + "] Select";
    if (m_rerollCharges > 0)
    {
        controlsStr += " | [R] Reroll (" + std::to_string(m_rerollCharges) + ")";
    }
    if (m_skipCharges > 0)
    {
        controlsStr += " | [S] Skip (" + std::to_string(m_skipCharges) + ")";
    }
    if (m_banishCharges > 0)
    {
        controlsStr += " | [B] Banish (" + std::to_string(m_banishCharges) + ")";
    }
    m_controlsText.setString(controlsStr);

    // Trigger entry appearance animation sequence
    m_isAppearing = true;
    m_animTimer = 0.0f;

    // Calculate max entry animation duration: max(bg animation duration, total card swipe duration)
    float totalCardSwipeTime = ((m_cards.empty() ? 0.0f : static_cast<float>(m_cards.size() - 1)) * m_cardStaggerDelay) + m_cardSwipeDuration;
    float maxEntryDuration = std::max(m_bgAnimDuration, totalCardSwipeTime);

    // Burst VFX film projector: exactly 6 sprites spanning maxEntryDuration, no loop!
    size_t burstFrameCount = m_burstAnimData.GetFrameCount();
    if (burstFrameCount > 0)
    {
        m_burstAnimData.isLooping = false;
        m_burstAnimData.frameDuration = maxEntryDuration / static_cast<float>(burstFrameCount);
        m_vfxAnimator.Play(&m_burstAnimData, true);
    }

    if (m_lastViewSize.x > 0.0f && m_lastViewSize.y > 0.0f)
    {
        UpdateLayout(m_lastViewSize, m_lastViewCenter);
    }
}

void SimpleTextLevelUpView::InitFallingGems()
{
    // Persistent memory pool reuse: if pool is already allocated, reuse existing vectors
    if (!m_fallingGems.empty())
    {
        for (auto& gem : m_fallingGems)
        {
            RespawnGem(gem, false);
        }
        return;
    }

    m_gemRects.clear();
    static const std::vector<std::string> gemKeys = { "GemBlue", "GemGreen", "GemRed" };
    for (const auto& key : gemKeys)
    {
        AssetTextureData data = m_atlas.GetTextureData(key);
        if (data.texture)
        {
            m_gemAtlasTexture = data.texture;
            m_gemRects.push_back(data.rect);
        }
    }

    if (m_gemRects.empty())
    {
        AssetTextureData data = m_atlas.GetTextureData("GemBlue");
        if (data.texture)
        {
            m_gemAtlasTexture = data.texture;
            m_gemRects.push_back(data.rect);
        }
    }

    constexpr size_t gemCount = 400; // Persistent dense pool
    m_fallingGems.resize(gemCount);

    m_gemVertices.setPrimitiveType(sf::Quads);
    m_gemVertices.resize(gemCount * 4);

    for (auto& gem : m_fallingGems)
    {
        RespawnGem(gem, false);
    }
}

void SimpleTextLevelUpView::RespawnGem(FallingGem& gem, bool /*randomY*/)
{
    if (!m_gemRects.empty())
    {
        gem.textureRect = m_gemRects[rand() % m_gemRects.size()];
    }

    float viewWidth = m_lastViewSize.x > 100.0f ? m_lastViewSize.x : 1920.0f;
    float viewHeight = m_lastViewSize.y > 100.0f ? m_lastViewSize.y : 1080.0f;
    float scaleY = viewHeight / 1080.0f;

    gem.position.x = static_cast<float>(rand() % static_cast<int>(viewWidth));
    // Always spawn above the upper edge of the view screen
    gem.position.y = -30.0f * scaleY - static_cast<float>(rand() % static_cast<int>(270.0f * scaleY + 1.0f));

    // Static initial rotation (no spinning while falling)
    gem.rotation = static_cast<float>(rand() % 360);

    gem.initialScale = 2.5f + (rand() % 90) / 100.0f; // 2.5f to 3.4f
    gem.currentScale = gem.initialScale;
    gem.fallSpeed = (85.0f + (rand() % 60)) * scaleY;  // Resolution-independent slow fall
    gem.shrinkSpeed = 0.18f + (rand() % 13) / 100.0f;  // Scale reduction per sec
}

void SimpleTextLevelUpView::Update(float dt)
{
    float viewWidth = m_lastViewSize.x > 100.0f ? m_lastViewSize.x : 1920.0f;
    float viewHeight = m_lastViewSize.y > 100.0f ? m_lastViewSize.y : 1080.0f;
    float scaleX = viewWidth / 1920.0f;
    float scaleY = viewHeight / 1080.0f;
    float resScale = std::min(scaleX, scaleY);
    sf::Vector2f screenTopLeft = m_lastViewCenter - sf::Vector2f(viewWidth / 2.0f, viewHeight / 2.0f);

    size_t gemCount = m_fallingGems.size();
    if (m_gemVertices.getVertexCount() != gemCount * 4)
    {
        m_gemVertices.resize(gemCount * 4);
    }

    for (size_t i = 0; i < gemCount; ++i)
    {
        auto& gem = m_fallingGems[i];
        gem.position.y += gem.fallSpeed * dt;
        gem.currentScale -= gem.shrinkSpeed * dt;

        if (gem.currentScale <= 0.0f || gem.position.y > viewHeight + 40.0f * scaleY)
        {
            RespawnGem(gem, false);
        }

        float normScale = std::max(0.0f, gem.currentScale) * resScale;
        sf::Transform transform;
        transform.translate(screenTopLeft + gem.position);
        transform.rotate(gem.rotation);
        transform.scale(normScale, normScale);

        float halfW = gem.textureRect.width / 2.0f;
        float halfH = gem.textureRect.height / 2.0f;

        sf::Vector2f p0 = transform.transformPoint(-halfW, -halfH);
        sf::Vector2f p1 = transform.transformPoint(halfW, -halfH);
        sf::Vector2f p2 = transform.transformPoint(halfW, halfH);
        sf::Vector2f p3 = transform.transformPoint(-halfW, halfH);

        float u0 = static_cast<float>(gem.textureRect.left);
        float v0 = static_cast<float>(gem.textureRect.top);
        float u1 = u0 + static_cast<float>(gem.textureRect.width);
        float v1 = v0 + static_cast<float>(gem.textureRect.height);

        float alphaRatio = normScale / (gem.initialScale * resScale);
        sf::Uint8 alpha = static_cast<sf::Uint8>(std::clamp(alphaRatio * 255.0f, 0.0f, 255.0f));
        sf::Color color(255, 255, 255, alpha);

        m_gemVertices[i * 4 + 0] = sf::Vertex(p0, color, sf::Vector2f(u0, v0));
        m_gemVertices[i * 4 + 1] = sf::Vertex(p1, color, sf::Vector2f(u1, v0));
        m_gemVertices[i * 4 + 2] = sf::Vertex(p2, color, sf::Vector2f(u1, v1));
        m_gemVertices[i * 4 + 3] = sf::Vertex(p3, color, sf::Vector2f(u0, v1));
    }

    if (m_window)
    {
        for (auto& card : m_cards)
        {
            card.Update(dt, *m_window);
        }
    }

    if (m_isAppearing)
    {
        m_animTimer += dt;
        m_vfxAnimator.Update(dt, m_burstSprite);

        float panelWidth = 650.0f * scaleX;
        float panelHeight = 860.0f * scaleY;

        // 1. Background Panel: Scale up from center (0 -> 1) with rotation (-90 deg -> 0 deg)
        float bgT = std::clamp(m_animTimer / m_bgAnimDuration, 0.0f, 1.0f);
        float bgEase = 1.0f - std::pow(1.0f - bgT, 3.0f); // Cubic Out Easing
        float bgScale = bgEase;
        float bgAngle = m_bgStartAngle * (1.0f - bgEase);

        m_nineSliceBg.SetSize(sf::Vector2f(panelWidth, panelHeight));
        m_nineSliceBg.SetCornerScale(1.4f * scaleY);
        m_nineSliceBg.setOrigin(panelWidth / 2.0f, panelHeight / 2.0f);
        m_nineSliceBg.setPosition(m_lastViewCenter);
        m_nineSliceBg.setScale(bgScale, bgScale);
        m_nineSliceBg.setRotation(bgAngle);
        m_nineSliceBg.Update();

        // 2. Cards: Staggered swipe from offscreen right edge to target position
        float offscreenRightX = m_lastViewCenter.x + m_lastViewSize.x / 2.0f + 100.0f * scaleX;

        for (size_t i = 0; i < m_cards.size() && i < m_cardTargets.size(); ++i)
        {
            float cardStartTime = static_cast<float>(i) * m_cardStaggerDelay;
            float cardT = std::clamp((m_animTimer - cardStartTime) / m_cardSwipeDuration, 0.0f, 1.0f);
            float cardEase = 1.0f - std::pow(1.0f - cardT, 3.0f); // Cubic Out Easing

            float targetX = m_cardTargets[i].targetPosition.x;
            float currentX = offscreenRightX + (targetX - offscreenRightX) * cardEase;
            sf::Vector2f currentPos(currentX, m_cardTargets[i].targetPosition.y);

            m_cards[i].SetLayout(
                currentPos,
                m_cardTargets[i].width,
                m_cardTargets[i].height,
                m_cardTargets[i].scaleX,
                m_cardTargets[i].scaleY
            );
        }

        float totalAnimTime = ((m_cards.empty() ? 0.0f : static_cast<float>(m_cards.size() - 1)) * m_cardStaggerDelay) + m_cardSwipeDuration;
        totalAnimTime = std::max(totalAnimTime, m_bgAnimDuration);

        if (m_animTimer >= totalAnimTime)
        {
            m_isAppearing = false;
            // Lock background and cards to final static layout
            m_nineSliceBg.setScale(1.0f, 1.0f);
            m_nineSliceBg.setRotation(0.0f);
            sf::Vector2f panelPos = m_lastViewCenter - sf::Vector2f(panelWidth / 2.0f, panelHeight / 2.0f);
            m_nineSliceBg.setOrigin(0.0f, 0.0f);
            m_nineSliceBg.setPosition(panelPos);
            m_nineSliceBg.Update();

            for (size_t i = 0; i < m_cards.size() && i < m_cardTargets.size(); ++i)
            {
                m_cards[i].SetLayout(
                    m_cardTargets[i].targetPosition,
                    m_cardTargets[i].width,
                    m_cardTargets[i].height,
                    m_cardTargets[i].scaleX,
                    m_cardTargets[i].scaleY
                );
            }
        }
    }
}

void SimpleTextLevelUpView::UpdateLayout(const sf::Vector2f& viewSize, const sf::Vector2f& viewCenter)
{
    m_lastViewSize = viewSize;
    m_lastViewCenter = viewCenter;

    // Scale factors relative to 1920x1080 baseline
    float scaleX = viewSize.x / 1920.0f;
    float scaleY = viewSize.y / 1080.0f;

    m_dimOverlay.setSize(viewSize);
    m_dimOverlay.setPosition(viewCenter - viewSize / 2.0f);

    // Baseline panel size measured at 1920x1080: 650 x 860 px (frame5_c4)
    float panelWidth = 650.0f * scaleX;
    float panelHeight = 860.0f * scaleY;
    float panelCornerScale = 1.4f * scaleY;

    sf::Vector2f panelPos = viewCenter - sf::Vector2f(panelWidth / 2.0f, panelHeight / 2.0f);

    if (!m_isAppearing)
    {
        m_nineSliceBg.SetSize(sf::Vector2f(panelWidth, panelHeight));
        m_nineSliceBg.SetCornerScale(panelCornerScale);
        m_nineSliceBg.setOrigin(0.0f, 0.0f);
        m_nineSliceBg.setPosition(panelPos);
        m_nineSliceBg.setScale(1.0f, 1.0f);
        m_nineSliceBg.setRotation(0.0f);
        m_nineSliceBg.Update();
    }

    // Header positioning (scaled to panel height)
    unsigned int titleCharSize = static_cast<unsigned int>(std::round(38.0f * scaleY));
    m_titleText.setCharacterSize(titleCharSize > 12 ? titleCharSize : 12);
    sf::FloatRect titleBounds = m_titleText.getLocalBounds();
    m_titleText.setPosition(viewCenter.x - titleBounds.width / 2.0f, panelPos.y + 35.0f * scaleY);

    unsigned int subCharSize = static_cast<unsigned int>(std::round(20.0f * scaleY));
    m_subtitleText.setCharacterSize(subCharSize > 10 ? subCharSize : 10);
    sf::FloatRect subBounds = m_subtitleText.getLocalBounds();
    m_subtitleText.setPosition(viewCenter.x - subBounds.width / 2.0f, panelPos.y + 82.0f * scaleY);

    // Baseline card parameters at 1920x1080: 610 x 155 px (frame1_c2) stacked vertically
    float cardWidth = 610.0f * scaleX;
    float cardHeight = 155.0f * scaleY;
    float cardSpacing = 8.0f * scaleY;
    float currentY = panelPos.y + 125.0f * scaleY;

    m_cardTargets.clear();
    m_cardTargets.resize(m_cards.size());

    for (size_t i = 0; i < m_cards.size(); ++i)
    {
        sf::Vector2f cardPos(viewCenter.x - cardWidth / 2.0f, currentY);
        m_cardTargets[i].targetPosition = cardPos;
        m_cardTargets[i].width = cardWidth;
        m_cardTargets[i].height = cardHeight;
        m_cardTargets[i].scaleX = scaleX;
        m_cardTargets[i].scaleY = scaleY;

        if (m_isAppearing)
        {
            const sf::Vector2f offscreenPosition(
                viewCenter.x + viewSize.x * 0.5f + 100.0f * scaleX,
                cardPos.y);
            m_cards[i].SetLayout(offscreenPosition, cardWidth, cardHeight, scaleX, scaleY);
        }
        else
        {
            m_cards[i].SetLayout(cardPos, cardWidth, cardHeight, scaleX, scaleY);
        }
        currentY += cardHeight + cardSpacing;
    }

    // Luck tip text positioning (displayed if less than 4 choices present)
    if (m_luckTipText.getString() != "")
    {
        unsigned int tipCharSize = static_cast<unsigned int>(std::round(16.0f * scaleY));
        m_luckTipText.setCharacterSize(tipCharSize > 8 ? tipCharSize : 8);
        sf::FloatRect tipBounds = m_luckTipText.getLocalBounds();
        m_luckTipText.setPosition(viewCenter.x - tipBounds.width / 2.0f, panelPos.y + panelHeight - 75.0f * scaleY);
    }

    // Controls text positioning
    unsigned int ctrlCharSize = static_cast<unsigned int>(std::round(18.0f * scaleY));
    m_controlsText.setCharacterSize(ctrlCharSize > 8 ? ctrlCharSize : 8);
    sf::FloatRect ctrlBounds = m_controlsText.getLocalBounds();
    m_controlsText.setPosition(viewCenter.x - ctrlBounds.width / 2.0f, panelPos.y + panelHeight - 45.0f * scaleY);
}

void SimpleTextLevelUpView::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    m_window = &window;
    if (event.type == sf::Event::KeyPressed)
    {
        int index = -1;
        if (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1) index = 0;
        else if (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::Numpad2) index = 1;
        else if (event.key.code == sf::Keyboard::Num3 || event.key.code == sf::Keyboard::Numpad3) index = 2;
        else if (event.key.code == sf::Keyboard::Num4 || event.key.code == sf::Keyboard::Numpad4) index = 3;

        if (index >= 0 && index < static_cast<int>(m_cards.size()))
        {
            if (m_onSelectOption)
            {
                m_onSelectOption(index);
            }
            return;
        }

        if (event.key.code == sf::Keyboard::R)
        {
            if (m_rerollCharges > 0 && m_onReroll)
            {
                m_onReroll();
            }
        }
        else if (event.key.code == sf::Keyboard::S)
        {
            if (m_skipCharges > 0 && m_onSkip)
            {
                m_onSkip();
            }
        }
    }
    else if ((event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) && event.mouseButton.button == sf::Mouse::Left)
    {
        for (auto& card : m_cards)
        {
            if (card.HandleMouseEvent(event, window, m_onSelectOption, m_onBanish))
            {
                return;
            }
        }
    }
}

void SimpleTextLevelUpView::Draw(sf::RenderWindow& window)
{
    m_window = &window;
    sf::Vector2f viewSize = window.getView().getSize();
    sf::Vector2f viewCenter = window.getView().getCenter();

    // Recompute layout only when view dimensions change
    if (viewSize != m_lastViewSize || viewCenter != m_lastViewCenter)
    {
        UpdateLayout(viewSize, viewCenter);
    }

    // Pure Draw Pass: zero state mutations
    // Single draw call for 400 batched falling gem particles under the background panel
    if (m_gemAtlasTexture && m_gemVertices.getVertexCount() > 0)
    {
        window.draw(m_gemVertices, m_gemAtlasTexture);
    }

    window.draw(m_nineSliceBg);
    window.draw(m_titleText);
    window.draw(m_subtitleText);

    for (const auto& card : m_cards)
    {
        window.draw(card);
    }

    // Burst VFX Film Projector projected at middle lower part of view during entry transition
    if ((m_isAppearing || !m_vfxAnimator.IsFinished()) && m_burstSprite.getTexture() != nullptr)
    {
        float scaleX = viewSize.x / 1920.0f;
        float scaleY = viewSize.y / 1080.0f;
        float panelHeight = 860.0f * scaleY;
        sf::Vector2f vfxPos = viewCenter + sf::Vector2f(m_vfxOffsetX * scaleX, panelHeight * m_vfxOffsetYRatio);
        m_burstSprite.setPosition(vfxPos);
        m_burstSprite.setScale(m_vfxScale * scaleY, m_vfxScale * scaleY);
        window.draw(m_burstSprite);
    }

    if (m_luckTipText.getString() != "")
    {
        window.draw(m_luckTipText);
    }

    window.draw(m_controlsText);
}
