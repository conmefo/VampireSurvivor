#include "CharacterSelectionView.h"
#include "../Panels/MainBoardPanel.h"
#include "../Panels/StatsPanel.h"
#include "../Elements/GoldDisplayWidget.h"
#include "../Elements/UIButton.h"
#include "../../Core/Data/PlayerProgressionManager.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

CharacterSelectionView::CharacterSelectionView(TextureAtlas& atlas, const sf::Font& font, const PlayerProgressionManager* progressionManager)
    : m_mainBoard(std::make_unique<MainBoardPanel>(atlas, font))
    , m_statsPanel(std::make_unique<StatsPanel>(atlas, font))
    , m_goldDisplay(std::make_unique<GoldDisplayWidget>(atlas, progressionManager, font))
    , m_confirmButton(std::make_unique<UIButton>(atlas, "button_c9_normal", 10, 10, 10, 10))
{
    m_confirmButton->SetText("Confirm", font, 24);
}

CharacterSelectionView::~CharacterSelectionView() = default;

void CharacterSelectionView::InitializeLayout(const sf::Vector2f& viewportSize)
{
    SetSize(viewportSize);

    float boardX = (viewportSize.x - BOARD_WIDTH) / 2.0f;
    float boardY = (viewportSize.y - BOARD_HEIGHT) / 2.0f;
    
    if(m_mainBoard)
    {
        m_mainBoard->SetPosition(sf::Vector2f(boardX, boardY));
        m_mainBoard->InitializeLayout(sf::Vector2f(BOARD_WIDTH, BOARD_HEIGHT));
    }

    float statsX = boardX - STATS_WIDTH - STATS_MARGIN_RIGHT;
    if(m_statsPanel)
    {
        m_statsPanel->SetPosition(sf::Vector2f(statsX, boardY));
        m_statsPanel->SetSize(sf::Vector2f(STATS_WIDTH, BOARD_HEIGHT));
    }

    float buttonX = viewportSize.x - BUTTON_WIDTH - BUTTON_MARGIN_RIGHT;
    float buttonY = viewportSize.y - BUTTON_HEIGHT - BUTTON_MARGIN_BOTTOM;
    if(m_confirmButton)
    {
        m_confirmButton->SetPosition(sf::Vector2f(buttonX, buttonY));
        m_confirmButton->SetSize(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));
    }

    if(m_goldDisplay)
    {
        m_goldDisplay->SetPosition(sf::Vector2f(GoldDisplayWidget::DEFAULT_X, GoldDisplayWidget::DEFAULT_Y));
    }
}

void CharacterSelectionView::Update(float deltaTime)
{
    if(m_mainBoard)
    {
        m_mainBoard->Update(deltaTime);
    }
    if(m_statsPanel)
    {
        m_statsPanel->Update(deltaTime);
    }
    if(m_goldDisplay)
    {
        m_goldDisplay->Update(deltaTime);
    }
    if(m_confirmButton)
    {
        m_confirmButton->Update(deltaTime);
    }
}

void CharacterSelectionView::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    if(m_mainBoard)
    {
        m_mainBoard->HandleEvent(event, window);
    }
    if(m_statsPanel)
    {
        m_statsPanel->HandleEvent(event, window);
    }
    if(m_goldDisplay)
    {
        m_goldDisplay->HandleEvent(event, window);
    }
    if(m_confirmButton)
    {
        m_confirmButton->HandleEvent(event, window);
    }
}

void CharacterSelectionView::Draw(sf::RenderTarget& target)
{
    if(m_mainBoard)
    {
        m_mainBoard->Draw(target);
    }
    if(m_statsPanel)
    {
        m_statsPanel->Draw(target);
    }
    if(m_goldDisplay)
    {
        m_goldDisplay->Draw(target);
    }
    if(m_confirmButton)
    {
        m_confirmButton->Draw(target);
    }
}
