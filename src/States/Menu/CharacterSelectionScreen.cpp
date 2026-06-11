#include "CharacterSelectionScreen.h"
#include "../Game/GameState.h"
#include "../StateManager.h"
#include "../../Core/WindowSettings.h"
#include <iostream>

CharacterSelectionScreen::CharacterSelectionScreen(StateContext context)
    : BaseState(context)
{
}

void CharacterSelectionScreen::Init()
{
    const sf::Font* font = m_context.fonts.GetPtr(FontID::Main);
    const sf::Font* boldFont = m_context.fonts.GetPtr(FontID::Bold);
    if(font)
    {
        m_view = std::make_unique<CharacterSelectionView>(
            m_context.atlas, 
            *font, 
            boldFont,
            m_context.characterData, 
            &m_context.progressionData
        );

        m_view->InitializeLayout(sf::Vector2f(Core::VIRTUAL_WIDTH, Core::VIRTUAL_HEIGHT));

        m_view->SetOnBackClicked([this]()
        {
            m_context.stateManager.PopState();
        });

        m_view->SetOnConfirmClicked([this](const std::string& characterId)
        {
            std::cout << "Starting game with character: " << characterId << std::endl;
            m_context.stateManager.PopState(); // Pop CharacterSelectionScreen
            m_context.stateManager.PopState(); // Pop MainMenuState
            m_context.stateManager.AddState(std::make_unique<GameState>(m_context));
        });
    }
}

void CharacterSelectionScreen::HandleInput(sf::Event& event, sf::RenderWindow& window)
{
    if(m_view)
    {
        m_view->HandleEvent(event, window);
    }
}

void CharacterSelectionScreen::Update(float dt)
{
    if(m_view)
    {
        m_view->Update(dt);
    }
}

void CharacterSelectionScreen::Draw(sf::RenderWindow& window)
{
    // Semi-transparent overlay background
    sf::RectangleShape bg(sf::Vector2f(window.getSize().x, window.getSize().y));
    bg.setFillColor(sf::Color(0, 0, 0, 120));
    window.draw(bg);

    if(m_view)
    {
        m_view->Draw(window);
    }
}
