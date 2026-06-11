#include "CharacterSelectionScreen.h"
#include "../StateManager.h"
#include <iostream>

CharacterSelectionScreen::CharacterSelectionScreen(StateContext context)
    : BaseState(context)
{
}

void CharacterSelectionScreen::Init()
{
    const sf::Font* font = m_context.fonts.GetPtr(FontID::Main);
    if(font)
    {
        m_view = std::make_unique<CharacterSelectionView>(
            m_context.atlas, 
            *font, 
            m_context.characterData, 
            &m_context.progressionData
        );

        // We assume VIRTUAL_WIDTH and VIRTUAL_HEIGHT from Core/WindowSettings.h, 
        // but for now let's just initialize with 1920x1080 or rely on window size later.
        m_view->InitializeLayout(sf::Vector2f(1920.0f, 1080.0f));

        m_view->SetOnBackClicked([this]()
        {
            m_context.stateManager.PopState();
        });

        m_view->SetOnConfirmClicked([this](const std::string& characterId)
        {
            std::cout << "Starting game with character: " << characterId << std::endl;
            // m_context.stateManager.PushState(std::make_unique<GameState>(m_context)); // Uncomment when GameState exists
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
    bg.setFillColor(sf::Color(0, 0, 0, 200));
    window.draw(bg);

    if(m_view)
    {
        m_view->Draw(window);
    }
}
