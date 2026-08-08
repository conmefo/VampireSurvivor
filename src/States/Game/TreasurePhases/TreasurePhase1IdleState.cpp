#include "TreasurePhase1IdleState.h"
#include <cmath>
#include <string>

void TreasurePhase1IdleState::OnEnter(TreasureRewardViewContext& ctx)
{
    m_idleFrames.clear();
    for(int frameIndex = 1; frameIndex <= 8; ++frameIndex)
    {
        const std::string suffix = frameIndex < 10 ? "0" + std::to_string(frameIndex)
                                                   : std::to_string(frameIndex);
        const AssetTextureData idleFrame = ctx.atlas.GetTextureData("TreasureIdle_" + suffix);
        if(idleFrame.texture)
        {
            m_idleFrames.push_back(idleFrame);
        }
    }

    m_titleText.setFont(ctx.font);
    m_titleText.setString("Treasure Found!");
    m_titleText.setCharacterSize(m_config.titleFontSize);
    // m_titleText.setStyle(sf::Text::Bold);
    m_titleText.setFillColor(m_config.titleColor);
    m_titleText.setOutlineColor(m_config.titleOutlineColor);
    m_titleText.setOutlineThickness(m_config.titleOutlineThickness);

    m_openButtonBg.SetTexture(ctx.atlas, "button_c9_mouseover");
    m_openButtonBg.SetMargins(10.0f, 10.0f, 10.0f, 10.0f);
    m_openButtonBg.SetCornerScale(2.0f);

    m_openButtonText.setFont(ctx.font);
    m_openButtonText.setString("OPEN");
    m_openButtonText.setCharacterSize(m_config.buttonFontSize);
    // m_openButtonText.setStyle(sf::Text::Bold);
    m_openButtonText.setFillColor(sf::Color::White);
    // m_openButtonText.setOutlineColor(sf::Color::White);
    // m_openButtonText.setOutlineThickness(0.5f);

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

    m_idleFrameIndex = 0;
    m_idleFrameTimer = 0.0f;
    m_idleDelayTimer = 0.0f;
    m_idleInPause = false;

    m_isExiting = false;
    m_exitTimer = 0.0f;
    m_showArrows = true;

    UpdateLayout(ctx.viewSize, ctx.viewCenter, ctx);
}

void TreasurePhase1IdleState::HandleEvent(const sf::Event& event, const sf::RenderWindow* window, TreasureRewardViewContext& /*ctx*/)
{
    if(m_isExiting)
    {
        return;
    }

    bool triggered = false;
    if(event.type == sf::Event::KeyPressed &&
       (event.key.code == sf::Keyboard::Enter ||
        event.key.code == sf::Keyboard::Space))
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
            if(m_openButtonBounds.contains(worldPos))
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
        m_isExiting = true;
        m_showArrows = false; // Arrows disappear immediately on click/press!
        m_exitTimer = 0.0f;
    }
}

void TreasurePhase1IdleState::Update(float dt, TreasureRewardViewContext& ctx)
{
    if(m_isExiting)
    {
        m_exitTimer += dt;
        float duration = std::max(0.01f, m_config.buttonExitDuration);
        float progress = std::clamp(m_exitTimer / duration, 0.0f, 1.0f);

        // Smooth step curve for rotation and scale
        float smoothProgress = progress * progress * (3.0f - 2.0f * progress);

        float rotation = smoothProgress * -180.0f; // Rotate 0 deg to -180 deg (left)
        float scaleVal = 1.0f - smoothProgress;   // Scale 1.0 down to 0.0

        float scaleX = ctx.viewSize.x / 1920.0f;
        float scaleY = ctx.viewSize.y / 1080.0f;
        float btnW = m_config.buttonWidth * scaleX;
        float btnH = m_config.buttonHeight * scaleY;
        sf::Vector2f btnCenter(ctx.viewCenter.x, ctx.panelPos.y + m_config.buttonYOffset * scaleY);

        m_openButtonBg.setOrigin(btnW * 0.5f, btnH * 0.5f);
        m_openButtonBg.setPosition(btnCenter);
        m_openButtonBg.setRotation(rotation);
        m_openButtonBg.setScale(scaleVal, scaleVal);

        m_openButtonText.setRotation(rotation);
        m_openButtonText.setScale(scaleVal, scaleVal);

        if(m_exitTimer >= duration)
        {
            if(ctx.triggerOpening)
            {
                ctx.triggerOpening();
            }
        }
        return;
    }

    if(!m_idleInPause && !m_idleFrames.empty())
    {
        m_idleFrameTimer += dt;
        if(m_idleFrameTimer >= m_config.chestFrameDuration)
        {
            m_idleFrameTimer -= m_config.chestFrameDuration;
            m_idleFrameIndex++;
            if(m_idleFrameIndex >= m_idleFrames.size())
            {
                m_idleFrameIndex = 0;
                m_idleInPause = true;
                m_idleDelayTimer = 0.0f;
            }
        }
    }
    else if(m_idleInPause)
    {
        m_idleDelayTimer += dt;
        if(m_idleDelayTimer >= m_config.chestLoopDelay)
        {
            m_idleInPause = false;
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
}

void TreasurePhase1IdleState::UpdateLayout(const sf::Vector2f& viewSize, const sf::Vector2f& viewCenter, TreasureRewardViewContext& ctx)
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

    CenterText(m_titleText, viewCenter.x, ctx.panelPos.y + m_config.titleYOffset * scaleY);

    float btnW = m_config.buttonWidth * scaleX;
    float btnH = m_config.buttonHeight * scaleY;
    sf::Vector2f btnCenter(viewCenter.x, ctx.panelPos.y + m_config.buttonYOffset * scaleY);
    sf::Vector2f btnPos = btnCenter - sf::Vector2f(btnW * 0.5f, btnH * 0.5f);

    m_openButtonBg.SetSize(sf::Vector2f(btnW, btnH));
    m_openButtonBg.setOrigin(0.0f, 0.0f);
    m_openButtonBg.setPosition(btnPos);
    m_openButtonBg.setRotation(0.0f);
    m_openButtonBg.setScale(1.0f, 1.0f);
    m_openButtonBg.Update();

    CenterText(m_openButtonText, btnCenter.x, btnCenter.y);
    m_openButtonText.setRotation(0.0f);
    m_openButtonText.setScale(1.0f, 1.0f);
    m_openButtonBounds = sf::FloatRect(btnPos.x, btnPos.y, btnW, btnH);

    float pad = m_config.arrowPadding * scaleX;
    m_leftArrow.setPosition(btnPos.x - pad, btnCenter.y);
    m_leftArrow.setScale(m_config.arrowScale * scaleY, m_config.arrowScale * scaleY);

    m_rightArrow.setPosition(btnPos.x + btnW + pad, btnCenter.y);
    m_rightArrow.setScale(-m_config.arrowScale * scaleY, m_config.arrowScale * scaleY);
}

void TreasurePhase1IdleState::Draw(sf::RenderTarget& target, const TreasureRewardViewContext& ctx) const
{
    target.draw(m_titleText);

    if(!m_idleFrames.empty() && m_idleFrameIndex < m_idleFrames.size())
    {
        float scaleY = ctx.panelSize.y / 860.0f;
        ConfigureSprite(m_chestSprite, m_idleFrames[m_idleFrameIndex]);

        // Recalculate position Y so bottom-center origin chest stays at the exact same screen location
        float chestBottomY = ctx.panelPos.y + (m_config.chestYOffset + 11.0f * m_config.chestScale) * scaleY;
        m_chestSprite.setPosition(ctx.viewCenter.x, chestBottomY);
        m_chestSprite.setScale(m_config.chestScale * scaleY, m_config.chestScale * scaleY);
        target.draw(m_chestSprite);
    }

    // Draw OPEN button while scale > 0
    if(!m_isExiting || m_exitTimer < m_config.buttonExitDuration)
    {
        target.draw(m_openButtonBg);
        target.draw(m_openButtonText);
    }

    // Draw cursor arrows when not exiting
    if(m_showArrows && !m_isExiting && m_leftArrow.getTexture() && m_rightArrow.getTexture())
    {
        target.draw(m_leftArrow);
        target.draw(m_rightArrow);
    }
}

void TreasurePhase1IdleState::OnExit(TreasureRewardViewContext& /*ctx*/)
{
}

void TreasurePhase1IdleState::ConfigureSprite(sf::Sprite& sprite, const AssetTextureData& data) const
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

void TreasurePhase1IdleState::CenterText(sf::Text& text, float x, float y) const
{
    const sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(
        std::floor(bounds.left + bounds.width * 0.5f),
        std::floor(bounds.top + bounds.height * 0.5f));
    text.setPosition(std::floor(x), std::floor(y));
}
