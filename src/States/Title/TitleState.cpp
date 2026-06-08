#include "TitleState.h"
#include "../StateManager.h"
#include <iostream>
#include <algorithm>

TitleState::TitleState(StateContext context)
    : BaseState(context)
{
}

void TitleState::Init()
{
    if(!m_pixelateShader.loadFromFile("assets/Shaders/pixelate.frag", sf::Shader::Fragment))
    {
        std::cerr << "Failed to load pixelate.frag\n";
    }

    m_bgTexture = &m_context.textures.Get(TextureID::Background);
    m_bgSprite.setTexture(*m_bgTexture);

    m_titleTexture = &m_context.textures.Get(TextureID::Title);
    m_titleSprite.setTexture(*m_titleTexture);
    m_titleSprite.setPosition(340.0f, 100.0f);

    m_compositeTexture.create(1280, 720);
    m_compositeTexture.clear();
    m_compositeTexture.draw(m_bgSprite);
    m_compositeTexture.draw(m_titleSprite);
    m_compositeTexture.display();
    
    m_compositeSprite.setTexture(m_compositeTexture.getTexture());

    m_charTextures.resize(UI_ITEM_COUNT);
    m_charSprites.resize(UI_ITEM_COUNT);
    m_charFaders.resize(UI_ITEM_COUNT);

    for(int i = 0; i < UI_ITEM_COUNT; i++)
    {
        TextureID id = TextureID::CharacterDecoration_0;
        if(i == 1) id = TextureID::CharacterDecoration_1;
        if(i == 2) id = TextureID::CharacterDecoration_2;
        
        m_charTextures[i] = &m_context.textures.Get(id);
        m_charSprites[i].setTexture(*m_charTextures[i]);
        m_charSprites[i].setPosition(300.0f + i * 250.0f, 350.0f);
        
        sf::Color c = m_charSprites[i].getColor();
        c.a = 0;
        m_charSprites[i].setColor(c);
        
        m_charFaders[i].SetDuration(0.5f);
    }

    m_promptTexture = &m_context.textures.Get(TextureID::Prompt);
    m_promptSprite.setTexture(*m_promptTexture);
    m_promptSprite.setPosition(490.0f, 600.0f);
    
    sf::Color c = m_promptSprite.getColor();
    c.a = 0;
    m_promptSprite.setColor(c);
    
    m_promptFader.SetDuration(0.8f);
    m_promptFader.FadeIn();
}

void TitleState::HandleInput(sf::Event& event)
{
    if(m_inputLocked || m_isTransitioningOut)
    {
        return;
    }

    if(event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonPressed)
    {
        m_inputLocked = true;
        m_isTransitioningOut = true;
        m_promptFadingIn = false;
        m_promptFader.FadeOut();
        
        m_context.stateManager.PopState();
    }
}

void TitleState::Update(float dt)
{
    m_stateTime += dt;

    if(!m_isPromptActive && m_currentPixelSize > 1.0f)
    {
        m_currentPixelSize -= (INITIAL_PIXEL_SIZE / PIXEL_ANIM_DURATION) * dt;
        if(m_currentPixelSize < 1.0f)
        {
            m_currentPixelSize = 1.0f;
        }

        if(m_currentPixelSize <= CLARITY_THRESHOLD)
        {
            m_isPromptActive = true;
            m_inputLocked = false;
        }
    }

    if(m_isPromptActive)
    {
        m_clarityTime += dt;

        for(int i = 0; i < UI_ITEM_COUNT; i++)
        {
            float triggerTime = i * UI_STAGGER_INTERVAL;
            if(m_clarityTime >= triggerTime && m_charFaders[i].GetFadeRatio() == 0.0f)
            {
                m_charFaders[i].FadeIn();
            }
            m_charFaders[i].Update(dt);
            
            sf::Color c = m_charSprites[i].getColor();
            c.a = static_cast<sf::Uint8>(std::clamp(m_charFaders[i].GetFadeRatio() * MAX_ALPHA, 0.0f, MAX_ALPHA));
            m_charSprites[i].setColor(c);
        }

        if(!m_isTransitioningOut)
        {
            m_promptFader.Update(dt);
            
            if(m_promptFadingIn && m_promptFader.GetFadeRatio() >= 1.0f)
            {
                m_promptFadingIn = false;
                m_promptFader.FadeOut();
            }
            else if(!m_promptFadingIn && m_promptFader.GetFadeRatio() <= 0.0f)
            {
                m_promptFadingIn = true;
                m_promptFader.FadeIn();
            }
        }
        else
        {
            m_promptFader.Update(dt);
        }

        sf::Color pc = m_promptSprite.getColor();
        pc.a = static_cast<sf::Uint8>(std::clamp(m_promptFader.GetFadeRatio() * MAX_ALPHA, 0.0f, MAX_ALPHA));
        m_promptSprite.setColor(pc);
    }
}

void TitleState::Draw(sf::RenderWindow& window)
{
    if(m_currentPixelSize > PIXEL_SNAP_THRESHOLD)
    {
        m_pixelateShader.setUniform("texture", sf::Shader::CurrentTexture);
        m_pixelateShader.setUniform("pixelSize", m_currentPixelSize);
        sf::Vector2f resolution(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y));
        m_pixelateShader.setUniform("resolution", resolution);

        window.draw(m_compositeSprite, &m_pixelateShader);
    }
    else
    {
        window.draw(m_compositeSprite);
    }

    for(int i = 0; i < UI_ITEM_COUNT; i++)
    {
        window.draw(m_charSprites[i]);
    }

    if(m_isPromptActive)
    {
        window.draw(m_promptSprite);
    }
}
