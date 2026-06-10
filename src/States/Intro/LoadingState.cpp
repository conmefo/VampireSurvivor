#include "LoadingState.h"
#include "../Menu/MainMenuState.h"
#include "WarningState.h"
#include "../StateManager.h"
#include "Core/WindowSettings.h"
#include <iostream>
#include <iomanip>
#include <sstream>

LoadingState::LoadingState(StateContext context)
    : BaseState(std::move(context))
    , m_currentDlcIndex(0)
    , m_currentPercent(0)
    , m_stepTimer(0.0f)
    , m_phase(0)
    , m_finalWaitTimer(0.0f)
{
}

void LoadingState::Init()
{
    // Load font
    try
    {
        m_context.fonts.Load(FontID::Main, "Assets/Fonts/Courier_HintedSmooth.ttf");
    }
    catch (...)
    {
        std::cerr << "Failed to load Courier_HintedSmooth.ttf, falling back to what is available.\n";
    }

    const sf::Font* fontPtr = m_context.fonts.GetPtr(FontID::Main);
    if(fontPtr)
    {
        m_font = *fontPtr;
    }

    // Initialize texts
    m_textTop.setFont(m_font);
    m_textTop.setCharacterSize(38);
    m_textTop.setFillColor(sf::Color::White);
    m_textTop.setString("Loading DLC");

    m_textMiddle.setFont(m_font);
    m_textMiddle.setCharacterSize(24);
    m_textMiddle.setFillColor(sf::Color::White);

    m_textBottom.setFont(m_font);
    m_textBottom.setCharacterSize(17);
    m_textBottom.setFillColor(sf::Color(150, 150, 150));
    m_textBottom.setString("v1.14.112 (63587693R)");

    // Assemble AnimationData manually
    m_treasureAnimData.frameDuration = 0.4f; // 2.5 frames per second
    m_treasureAnimData.isLooping = true;
    
    // TreasureIdle_01 to 04
    for(int i = 1; i <= 4; ++i)
    {
        std::stringstream ss;
        ss << "TreasureIdle_0" << i;
        AssetTextureData frameData = m_context.atlas.GetTextureData(ss.str());
        if(frameData.texture != nullptr)
        {
            m_treasureAnimData.frames.push_back(frameData);
        }
    }

    m_treasureAnimator.Play(&m_treasureAnimData);

    // Positioning: Treasure in bottom-right
    // Assume Virtual Bounds
    float vx = Core::VIRTUAL_WIDTH;
    float vy = Core::VIRTUAL_HEIGHT;

    m_treasureSprite.setPosition(vx - 175.0f, vy - 120.0f);
    m_treasureSprite.setScale(4.3f, 3.2f);

    m_textBottom.setPosition(vx - 600.0f, vy - 65.0f);
    m_textMiddle.setPosition(vx - 600.0f, vy - 98.0f);
    m_textTop.setPosition(vx - 600.0f, vy - 140.0f);
}

void LoadingState::HandleInput(sf::Event& event, sf::RenderWindow& window)
{
}

void LoadingState::Update(float dt)
{
    m_treasureAnimator.Update(dt, m_treasureSprite);

    if(m_phase == 0)
    {
        m_stepTimer += dt;
        if(m_stepTimer >= 0.15f) // step fast
        {
            m_stepTimer = 0.0f;
            m_currentPercent += 10 + (rand() % 70); // random jump

            if(m_currentPercent >= 100)
            {
                m_currentPercent = 100;
                
                // Keep it at 100% for one frame then advance
                m_currentDlcIndex++;
                if(m_currentDlcIndex >= static_cast<int>(m_dlcNames.size()))
                {
                    m_phase = 1;
                }
                else
                {
                    m_currentPercent = 0;
                }
            }
        }

        if(m_phase == 0 && m_currentDlcIndex < static_cast<int>(m_dlcNames.size()))
        {
            std::stringstream ss;
            ss << m_dlcNames[m_currentDlcIndex] << " (" << m_currentPercent << "%)";
            m_textMiddle.setString(ss.str());
        }
    }
    else if(m_phase == 1)
    {
        // Phase 1: Hide middle text, move top text
        m_textMiddle.setString("");
        
        m_textTop.setString("Loading");
        m_textTop.setPosition(Core::VIRTUAL_WIDTH - 600.0f, Core::VIRTUAL_HEIGHT - 110.0f); // Move to middle line Y

        m_finalWaitTimer += dt;
        if(m_finalWaitTimer >= 1.5f)
        {
            m_context.stateManager.PopState();
            m_context.stateManager.AddState(std::make_unique<WarningState>(m_context));
        }
    }
}

void LoadingState::Draw(sf::RenderWindow& window)
{
    // Pure Black background
    window.clear(sf::Color::Black);

    window.draw(m_textTop);
    if(m_phase == 0)
    {
        window.draw(m_textMiddle);
    }
    window.draw(m_textBottom);

    window.draw(m_treasureSprite);
}
