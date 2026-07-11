#include "RunResultView.h"

#include "../../Core/Resources/TextureAtlas.h"
#include "../../UI/Elements/UIButton.h"

#include <algorithm>
#include <iomanip>
#include <memory>
#include <sstream>
#include <utility>

namespace
{
constexpr float ResultViewWidth = 1920.0f;
constexpr float ResultViewHeight = 1080.0f;

void CenterText(sf::Text& text, const sf::Vector2f& position)
{
    const sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
    text.setPosition(position);
}

std::string FormatTime(int totalSeconds)
{
    totalSeconds = std::max(0, totalSeconds);

    std::ostringstream stream;
    stream << totalSeconds / 60 << ':' << std::setw(2) << std::setfill('0') << totalSeconds % 60;
    return stream.str();
}
}

RunResultView::RunResultView(TextureAtlas& atlas, const sf::Font& font, const sf::Font& boldFont)
{
    m_dimmer.setSize(sf::Vector2f(ResultViewWidth, ResultViewHeight));
    m_dimmer.setFillColor(sf::Color(3, 5, 12, 218));

    m_panel.setPosition(620.0f, 340.0f);
    m_panel.setSize(sf::Vector2f(680.0f, 400.0f));
    m_panel.setFillColor(sf::Color(14, 31, 34, 248));
    m_panel.setOutlineColor(sf::Color(207, 181, 93));
    m_panel.setOutlineThickness(4.0f);

    m_title.setFont(boldFont);
    m_title.setCharacterSize(56);
    m_title.setOutlineColor(sf::Color(35, 20, 8));
    m_title.setOutlineThickness(3.0f);

    m_stageName.setFont(boldFont);
    m_stageName.setCharacterSize(28);
    m_stageName.setFillColor(sf::Color::White);

    m_survivalTime.setFont(font);
    m_survivalTime.setCharacterSize(30);
    m_survivalTime.setFillColor(sf::Color(215, 220, 210));

    auto returnButton = std::make_unique<UIButton>(
        atlas, "button_c9_normal", 10.0f, 10.0f, 10.0f, 10.0f);
    returnButton->SetHoverTexture("button_c9_mouseover");
    returnButton->SetPressTexture("button_c9_pressed");
    returnButton->SetPosition(sf::Vector2f(810.0f, 625.0f));
    returnButton->SetSize(sf::Vector2f(300.0f, 76.0f));
    returnButton->SetText("MAIN MENU", boldFont, 28);
    returnButton->SetCornerScale(2.0f);
    returnButton->SetFocus(true);
    returnButton->SetOnClickCallback([this]() { RequestReturnToMenu(); });

    m_returnButton = returnButton.get();
    m_uiManager.AddElement(std::move(returnButton));
}

void RunResultView::SetResult(
    RunResultOutcome outcome,
    const std::string& stageName,
    int survivalTimeSeconds)
{
    const bool completed = outcome == RunResultOutcome::Completed;
    m_title.setString(completed ? "STAGE COMPLETE" : "GAME OVER");
    m_title.setFillColor(completed ? sf::Color(255, 224, 112) : sf::Color(235, 72, 72));
    CenterText(m_title, sf::Vector2f(ResultViewWidth / 2.0f, 425.0f));

    m_stageName.setString(stageName);
    CenterText(m_stageName, sf::Vector2f(ResultViewWidth / 2.0f, 510.0f));

    m_survivalTime.setString("SURVIVAL TIME  " + FormatTime(survivalTimeSeconds));
    CenterText(m_survivalTime, sf::Vector2f(ResultViewWidth / 2.0f, 568.0f));

    m_returnRequested = false;
    if(m_returnButton)
    {
        m_returnButton->SetFocus(true);
    }
}

void RunResultView::SetOnReturnToMenu(std::function<void()> callback)
{
    m_onReturnToMenu = std::move(callback);
}

void RunResultView::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    m_uiManager.HandleEvent(event, window);

    if(event.type == sf::Event::KeyPressed &&
       (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Escape))
    {
        RequestReturnToMenu();
    }
}

void RunResultView::Update(float dt)
{
    m_uiManager.Update(dt);
}

void RunResultView::Draw(sf::RenderTarget& target)
{
    target.draw(m_dimmer);
    target.draw(m_panel);
    target.draw(m_title);
    target.draw(m_stageName);
    target.draw(m_survivalTime);
    m_uiManager.Draw(target);
}

void RunResultView::RequestReturnToMenu()
{
    if(m_returnRequested)
    {
        return;
    }

    m_returnRequested = true;
    if(m_onReturnToMenu)
    {
        m_onReturnToMenu();
    }
}
