#include "GameState.h"
#include "../StateManager.h"
#include <iostream>

GameState::GameState(StateContext context)
    : BaseState(context)
{
}

void GameState::Init()
{
    std::cout << "GameState Init" << std::endl;

    if(!m_font.loadFromFile("assets/fonts/arial.ttf"))
    {
        std::cerr << "Failed to load assets/fonts/arial.ttf" << std::endl;
    }

    m_titleText.setFont(m_font);
    m_titleText.setString("GAME STATE");
    m_titleText.setCharacterSize(36);
    m_titleText.setStyle(sf::Text::Bold);
    m_titleText.setFillColor(sf::Color::White);
}

void GameState::HandleInput(sf::Event& event)
{
    if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
    {
        m_context.stateManager.PopState();
    }
}

void GameState::Update(float dt)
{
}

void GameState::Draw(sf::RenderWindow& window)
{
    window.clear(sf::Color(18, 18, 24));

    sf::FloatRect bounds = m_titleText.getLocalBounds();
    m_titleText.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
    m_titleText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

    window.draw(m_titleText);
}

