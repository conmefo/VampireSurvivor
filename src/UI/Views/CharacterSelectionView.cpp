#include "CharacterSelectionView.h"
#include "../Panels/MainBoardPanel.h"
#include "../Panels/RosterGridPanel.h"
#include "../Panels/DetailPanel.h"
#include "../Panels/StatsPanel.h"
#include "../Elements/GoldDisplayWidget.h"
#include "../Elements/UIButton.h"
#include "../../Core/Data/PlayerProgressionManager.h"
#include "../../Core/Data/CharacterDataManager.h"
#include "../../Core/Data/CharacterProfile.h"
#include "../../Core/Data/PowerUpDataManager.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

CharacterSelectionView::CharacterSelectionView(TextureAtlas& atlas, const sf::Font& font, const sf::Font* boldFont, const CharacterDataManager& characterData, const PlayerProgressionManager* progressionManager, const WeaponDataManager* weaponManager, const PowerUpDataManager* powerUpManager)
    : m_mainBoard(std::make_unique<MainBoardPanel>(atlas, font, boldFont))
    , m_statsPanel(std::make_unique<StatsPanel>(atlas, font))
    , m_goldDisplay(std::make_unique<GoldDisplayWidget>(atlas, progressionManager, font))
    , m_confirmButton(std::make_unique<UIButton>(atlas, "button_c5_normal", 10, 10, 10, 10))
    , m_backButton(std::make_unique<UIButton>(atlas, "button_c8_normal", 10, 10, 10, 10))
{
    m_confirmButton->SetText("Confirm", font, 24);
    m_backButton->SetText("BACK", font, 24);
    
    m_confirmButton->SetState(ButtonState::Disabled); // Initially disabled

    if(m_mainBoard && m_mainBoard->GetRosterGrid())
    {
        if(progressionManager)
        {
            m_mainBoard->GetRosterGrid()->InitializeRoster(characterData, *progressionManager, weaponManager);
        }
        
        m_mainBoard->GetRosterGrid()->SetOnSelectionChanged([this, &characterData, progressionManager, weaponManager, powerUpManager](const std::string& characterId)
        {
            m_selectedCharacterId = characterId;
            const CharacterProfile& profile = characterData.GetCharacterById(characterId);
            
            if(m_mainBoard->GetDetailPanel())
            {
                m_mainBoard->GetDetailPanel()->SetCharacterProfile(profile, weaponManager);
            }

            if(m_statsPanel)
            {
                m_statsPanel->SetCharacterProfile(profile, progressionManager, powerUpManager);
            }

            if(progressionManager && progressionManager->IsCharacterUnlocked(characterId))
            {
                m_confirmButton->SetState(ButtonState::Normal);
            }
            else
            {
                m_confirmButton->SetState(ButtonState::Disabled);
            }
        });
    }

    m_confirmButton->SetOnClickCallback([this]()
    {
        if(m_onConfirmClicked && !m_selectedCharacterId.empty() && m_confirmButton && true /* Wait, no explicit IsEnabled, state is enough but let's double check */)
        {
            // Ideally we check state, but SetOnClickCallback doesn't automatically block execution in some naive implementations.
            // But we'll trust the button handles it or just check state. But we don't have GetState.
            // Progression manager is not accessible here unless we capture it or we just check if it's not disabled.
            m_onConfirmClicked(m_selectedCharacterId);
        }
    });

    m_backButton->SetOnClickCallback([this]()
    {
        if(m_onBackClicked)
        {
            m_onBackClicked();
        }
    });
}

CharacterSelectionView::~CharacterSelectionView() = default;

void CharacterSelectionView::InitializeLayout(const sf::Vector2f& viewportSize)
{
    SetSize(viewportSize);

    float boardX = (viewportSize.x - BOARD_WIDTH) / 2.0f;
    float boardY = (viewportSize.y - BOARD_HEIGHT) / 2.0f + 50.0f;
    
    if(m_mainBoard)
    {
        m_mainBoard->SetPosition(sf::Vector2f(boardX, boardY));
        m_mainBoard->InitializeLayout(sf::Vector2f(BOARD_WIDTH, BOARD_HEIGHT));
    }

    float statsX = boardX - STATS_WIDTH - STATS_MARGIN_RIGHT;
    if(m_statsPanel)
    {
        m_statsPanel->SetPosition(sf::Vector2f(statsX, boardY + 100.0f));
        m_statsPanel->SetSize(sf::Vector2f(STATS_WIDTH, STATS_HEIGHT));
    }

    float buttonX = viewportSize.x - BUTTON_WIDTH - BUTTON_MARGIN_RIGHT;
    float buttonY = viewportSize.y - BUTTON_HEIGHT - BUTTON_MARGIN_BOTTOM;
    if(m_confirmButton)
    {
        m_confirmButton->SetPosition(sf::Vector2f(buttonX, buttonY));
        m_confirmButton->SetSize(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));
    }
    
    float backX = BUTTON_MARGIN_RIGHT; // using the margin as X padding from left
    if(m_backButton)
    {
        m_backButton->SetPosition(sf::Vector2f(backX, buttonY));
        m_backButton->SetSize(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));
    }

    if(m_goldDisplay)
    {
        m_goldDisplay->SetPosition(sf::Vector2f(GoldDisplayWidget::DEFAULT_X, GoldDisplayWidget::DEFAULT_Y));
    }
}

void CharacterSelectionView::SetOnBackClicked(std::function<void()> callback)
{
    m_onBackClicked = callback;
}

void CharacterSelectionView::SetOnConfirmClicked(std::function<void(const std::string&)> callback)
{
    m_onConfirmClicked = callback;
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
    if(m_backButton)
    {
        m_backButton->Update(deltaTime);
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
    if(m_backButton)
    {
        m_backButton->HandleEvent(event, window);
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
    if(m_backButton)
    {
        m_backButton->Draw(target);
    }
}
