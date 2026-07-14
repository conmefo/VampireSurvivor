#include "GameOverView.h"

#include "../../Core/Resources/TextureAtlas.h"
#include "../../UI/Elements/UIButton.h"

#include <algorithm>
#include <memory>
#include <utility>

namespace
{
constexpr float ViewWidth = 1920.0f;
constexpr float ViewHeight = 1080.0f;
constexpr float EntranceDuration = 1.0f;
constexpr float TitleFinalWidth = 650.0f;
constexpr float TitleStartScaleMultiplier = 1.52f;
constexpr sf::Uint8 OverlayFinalAlpha = 158;

float SmoothStep(float value)
{
    value = std::clamp(value, 0.0f, 1.0f);
    return value * value * (3.0f - 2.0f * value);
}
}

GameOverView::GameOverView(
    TextureAtlas& atlas,
    const sf::Texture& titleTexture,
    const sf::Font& boldFont)
{
    m_crimsonOverlay.setSize(sf::Vector2f(ViewWidth, ViewHeight));

    m_title.setTexture(titleTexture);
    const sf::Vector2u titleSize = titleTexture.getSize();
    m_title.setOrigin(
        static_cast<float>(titleSize.x) / 2.0f,
        static_cast<float>(titleSize.y) / 2.0f);
    m_title.setPosition(ViewWidth / 2.0f, 415.0f);
    if(titleSize.x > 0)
    {
        m_finalTitleScale = TitleFinalWidth / static_cast<float>(titleSize.x);
    }

    auto quitButton = std::make_unique<UIButton>(
        atlas, "button_c9_normal", 10.0f, 10.0f, 10.0f, 10.0f);
    quitButton->SetHoverTexture("button_c9_mouseover");
    quitButton->SetPressTexture("button_c9_pressed");
    quitButton->SetPosition(sf::Vector2f(790.0f, 650.0f));
    quitButton->SetSize(sf::Vector2f(340.0f, 94.0f));
    quitButton->SetText("QUIT", boldFont, 36);
    quitButton->SetCornerScale(2.0f);
    quitButton->SetOnClickCallback([this]() { RequestQuit(); });

    m_quitButton = quitButton.get();
    m_uiManager.AddElement(std::move(quitButton));

    Show();
}

void GameOverView::Show()
{
    m_entranceElapsed = 0.0f;
    m_quitRequested = false;
    if(m_quitButton)
    {
        m_quitButton->SetFocus(true);
    }
    UpdateEntranceAnimation();
}

void GameOverView::SetOnQuit(std::function<void()> callback)
{
    m_onQuit = std::move(callback);
}

void GameOverView::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    m_uiManager.HandleEvent(event, window);

    if(event.type == sf::Event::KeyPressed &&
       (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Escape))
    {
        RequestQuit();
    }
}

void GameOverView::Update(float dt)
{
    m_entranceElapsed = std::min(EntranceDuration, m_entranceElapsed + std::max(0.0f, dt));
    UpdateEntranceAnimation();
    m_uiManager.Update(dt);
}

void GameOverView::Draw(sf::RenderTarget& target)
{
    target.draw(m_crimsonOverlay);
    target.draw(m_title);
    m_uiManager.Draw(target);
}

void GameOverView::RequestQuit()
{
    if(m_quitRequested)
    {
        return;
    }

    m_quitRequested = true;
    if(m_onQuit)
    {
        m_onQuit();
    }
}

void GameOverView::UpdateEntranceAnimation()
{
    const float progress = SmoothStep(m_entranceElapsed / EntranceDuration);
    const sf::Uint8 overlayAlpha = static_cast<sf::Uint8>(OverlayFinalAlpha * progress);
    m_crimsonOverlay.setFillColor(sf::Color(112, 5, 28, overlayAlpha));

    const float scaleMultiplier =
        TitleStartScaleMultiplier + (1.0f - TitleStartScaleMultiplier) * progress;
    const float titleScale = m_finalTitleScale * scaleMultiplier;
    m_title.setScale(titleScale, titleScale);
    m_title.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(255.0f * progress)));
}
