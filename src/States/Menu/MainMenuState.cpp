#include "MainMenuState.h"
#include "../Game/GameState.h"
#include "../StateManager.h"
#include <iostream>

namespace
{
    bool LoadMenuFont(sf::Font& font)
    {
        const char* fontPaths[] = {
            "assets/fonts/menu.ttf",
            "C:/Windows/Fonts/georgiab.ttf",
            "assets/fonts/arial.ttf"
        };

        for(const char* path : fontPaths)
        {
            if(font.loadFromFile(path))
            {
                return true;
            }
        }

        return false;
    }
}

void MainMenuState::Init()
{
    std::cout << "MainMenuState Init" << std::endl;

    if(!m_buttonTexture.loadFromFile("assets/images/buttons/blue_button.png"))
    {
        std::cerr << "Failed to load assets/images/buttons/blue_button.png" << std::endl;
        return;
    }

    m_startButton = std::make_unique<UIButton>(m_buttonTexture, 8.0f, 8.0f, 8.0f, 8.0f);
    m_startButton->SetPosition(sf::Vector2f(285.0f, 330.0f));
    m_startButton->SetSize(sf::Vector2f(230.0f, 72.0f));
    m_startButton->SetStateColors(
        sf::Color::White,
        sf::Color(230, 235, 255),
        sf::Color(170, 180, 230),
        sf::Color(80, 80, 100, 180)
    );

    if(!LoadMenuFont(m_font))
    {
        std::cerr << "Failed to load menu font" << std::endl;
    }

    m_font.setSmooth(false);
    m_startText.setFont(m_font);
    m_startText.setString("START");
    m_startText.setCharacterSize(32);
    m_startText.setStyle(sf::Text::Bold);
    m_startText.setLetterSpacing(0.9f);
    m_startText.setFillColor(sf::Color::White);
    m_startText.setOutlineThickness(2.0f);
    m_startText.setOutlineColor(sf::Color(78, 84, 170));
    CenterStartText();
}

void MainMenuState::HandleInput(sf::Event& event)
{
    if(!m_startButton)
    {
        return;
    }

    if(event.type == sf::Event::MouseMoved)
    {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
        m_startButton->SetFocus(m_startButton->Contains(mousePos));
    }
    else if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        if(m_startButton->Contains(mousePos))
        {
            m_startButton->SetState(ButtonState::Pressed);
            m_startPressed = true;
        }
    }
    else if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        bool shouldStart = m_startPressed && m_startButton->Contains(mousePos);
        m_startPressed = false;
        m_startButton->SetState(shouldStart ? ButtonState::Hovered : ButtonState::Normal);

        if(shouldStart)
        {
            StartGame();
        }
    }
}

void MainMenuState::Update(float dt)
{
    if(m_startButton)
    {
        m_startButton->Update(dt);
    }
}

void MainMenuState::Draw(sf::RenderWindow& window)
{
    window.clear(sf::Color(10, 50, 80));

    if(m_startButton)
    {
        m_startButton->Draw(window);
        window.draw(m_startText);
    }
}

void MainMenuState::CenterStartText()
{
    if(!m_startButton)
    {
        return;
    }

    sf::FloatRect textBounds = m_startText.getLocalBounds();
    sf::Vector2f buttonPos = m_startButton->GetPosition();
    sf::Vector2f buttonSize = m_startButton->GetSize();

    m_startText.setOrigin(
        textBounds.left + textBounds.width / 2.0f,
        textBounds.top + textBounds.height / 2.0f
    );
    m_startText.setPosition(
        buttonPos.x + buttonSize.x / 2.0f,
        buttonPos.y + buttonSize.y / 2.0f - 2.0f
    );
}

void MainMenuState::StartGame()
{
    m_context.stateManager.PopState();
    m_context.stateManager.AddState(std::make_unique<GameState>(m_context));
}
