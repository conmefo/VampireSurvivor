#include "TitleState.h"
#include "../Menu/MainMenuState.h"
#include "../StateManager.h"
#include "../../Core/WindowSettings.h"
#include "../../World/TileMapManager.h"
#include <iostream>

TitleState::TitleState(StateContext context, TileMapManager& mapManager)
    : BaseState(std::move(context))
    , m_mapManager(mapManager)
    , m_currentPixelSize(120.0f)
    , m_shaderActive(true)
    , m_inputLocked(true)
    , m_promptFader()
    , m_promptFadingIn(true)
{
}

void TitleState::Init()
{
    std::cout << "TitleState Init\n";

    // 1. Setup Composite Background
    SetupCompositeBackground();

    // 2. Load Pixelate Shader
    if (!m_pixelateShader.loadFromFile("Assets/Shaders/pixelate.frag", sf::Shader::Fragment))
    {
        std::cerr << "Failed to load pixelate.frag\n";
        m_shaderActive = false; // Fallback
    }

    // 3. Setup Illustrations
    for (int i = 0; i < 3; ++i)
    {
        FadeItem item;
        std::string key = "illustrations_" + std::to_string(i);
        AssetTextureData data = m_context.atlas.GetTextureData(key);
        
        if (data.texture)
        {
            item.sprite.setTexture(*data.texture);
            item.sprite.setTextureRect(data.rect);
            item.sprite.setOrigin(data.rect.width / 2.0f, data.rect.height / 2.0f);
        }
        
        // Position them across the screen
        // float spacing = Core::VIRTUAL_WIDTH / 4.0f;
        // item.sprite.setPosition(spacing * (i + 1), Core::VIRTUAL_HEIGHT / 2.0f + 100.0f);
        
        item.sprite.setColor(sf::Color(255, 255, 255, 0)); // Start fully transparent
        item.fader = FaderComponent();
        item.fader.SetDuration(2.0f); // 1 second fade in
        item.delayTimer = i * STAGGER_INTERVAL;
        item.started = false;

        m_illustrations.push_back(item);
    }

    float textureGap = Core::VIRTUAL_WIDTH / 3.0f;
    m_illustrations[0].sprite.setPosition(Core::VIRTUAL_WIDTH / 2.0f, Core::VIRTUAL_HEIGHT / 2.0f + 280.0f);
    m_illustrations[0].sprite.setScale(2.5f, 2.5f);
    
    m_illustrations[1].sprite.setPosition(Core::VIRTUAL_WIDTH / 2.0f - textureGap, Core::VIRTUAL_HEIGHT / 2.0f + 230.0f);
    m_illustrations[1].sprite.setScale(4.9f, 4.9f);
    
    m_illustrations[2].sprite.setPosition(Core::VIRTUAL_WIDTH / 2.0f + textureGap, Core::VIRTUAL_HEIGHT / 2.0f);
    m_illustrations[2].sprite.setScale(-3.0f, 3.0f);
    


    // 4. Setup Press to Start Text
    const sf::Font* fontPtr = m_context.fonts.GetPtr(FontID::Main);
    if (!fontPtr)
    {
        try
        {
            m_context.fonts.Load(FontID::Main, "Assets/Fonts/Courier_HintedSmooth.ttf");
            fontPtr = m_context.fonts.GetPtr(FontID::Main);
        }
        catch (...) {}
    }

    if (fontPtr) m_font = *fontPtr;

    m_promptText.setFont(m_font);
    m_promptText.setString("PRESS TO START");
    m_promptText.setCharacterSize(49);
    m_promptText.setFillColor(sf::Color(255, 255, 255, 0));
    
    sf::FloatRect bounds = m_promptText.getLocalBounds();
    m_promptText.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
    m_promptText.setPosition(Core::VIRTUAL_WIDTH / 2.0f, Core::VIRTUAL_HEIGHT - 240.0f);

    m_promptFader.SetDuration(1.0f);
    m_promptFader.FadeIn();
}

void TitleState::SetupCompositeBackground()
{
    m_compositeTexture.create(Core::VIRTUAL_WIDTH, Core::VIRTUAL_HEIGHT);
    m_compositeTexture.clear(sf::Color::Black);

    const sf::Texture* bgTex = m_context.textures.GetPtr("Background");
    if (bgTex)
    {
        sf::Sprite bgSprite(*bgTex);
        bgSprite.setOrigin(bgTex->getSize().x / 2.0f, bgTex->getSize().y / 2.0f);
        bgSprite.setPosition(Core::VIRTUAL_WIDTH / 2.0f, Core::VIRTUAL_HEIGHT / 2.0f);
        
        // Scale to cover the entire virtual screen, maintaining aspect ratio
        float scaleX = Core::VIRTUAL_WIDTH / static_cast<float>(bgTex->getSize().x);
        float scaleY = Core::VIRTUAL_HEIGHT / static_cast<float>(bgTex->getSize().y);
        float scale = std::max(scaleX, scaleY);
        bgSprite.setScale(scale, scale);
        
        m_compositeTexture.draw(bgSprite);
    }

    const sf::Texture* titleTex = m_context.textures.GetPtr("Title");
    if (titleTex)
    {
        sf::Sprite titleSprite(*titleTex);
        titleSprite.setOrigin(titleTex->getSize().x / 2.0f, titleTex->getSize().y / 2.0f);
        titleSprite.setPosition(Core::VIRTUAL_WIDTH / 2.0f,
                                Core::VIRTUAL_HEIGHT / 2.0f - 216.f);
        titleSprite.setScale(1.8f, 1.8f);
        m_compositeTexture.draw(titleSprite);
    }

    m_compositeTexture.display();
    m_compositeSprite.setTexture(m_compositeTexture.getTexture());
}

void TitleState::HandleInput(sf::Event& event, sf::RenderWindow& window)
{
    if (m_inputLocked) return;

    if (event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonPressed)
    {
        m_inputLocked = true;
        // Trigger transition to MainMenu
        m_context.stateManager.PopState();
        m_context.stateManager.AddState(std::make_unique<MainMenuState>(m_context, m_mapManager));
    }
}

void TitleState::Update(float dt)
{
    // 1. Pixelation Phase
    if (m_shaderActive)
    {
        m_currentPixelSize -= dt * 80.0f; // Shrink pixels rapidly
        if (m_currentPixelSize <= PIXEL_SNAP_THRESHOLD)
        {
            m_currentPixelSize = 1.0f;
            m_shaderActive = false;
        }
    }
    else
    {
        // 2. Reveal Illustrations after Pixelation Finishes
        UpdateFaders(dt);

        // Check if last illustration has started fading in, then start blinking prompt
        if (!m_illustrations.empty() && m_illustrations.back().started)
        {
            m_inputLocked = false;

            m_promptFader.Update(dt);
            float alpha = m_promptFader.GetFadeRatio();
            
            sf::Color c = m_promptText.getFillColor();
            c.a = static_cast<sf::Uint8>(alpha * 255.0f);
            m_promptText.setFillColor(c);

            if (m_promptFadingIn && alpha >= 0.99f)
            {
                m_promptFadingIn = false;
                m_promptFader.FadeOut();
            }
            else if (!m_promptFadingIn && alpha <= 0.01f)
            {
                m_promptFadingIn = true;
                m_promptFader.FadeIn();
            }
        }
    }
}

void TitleState::UpdateFaders(float dt)
{
    for (auto& item : m_illustrations)
    {
        if (!item.started)
        {
            item.delayTimer -= dt;
            if (item.delayTimer <= 0.0f)
            {
                item.started = true;
                item.fader.FadeIn();
            }
        }
        else
        {
            item.fader.Update(dt);
            sf::Color c = item.sprite.getColor();
            
            float targetAlpha = 255.0f;
            if (&item == &m_illustrations[1])
            {
                targetAlpha = 90.0f;
            }
            if(&item == &m_illustrations[2])
            {
                targetAlpha = 150.0f;
            }
            
            c.a = static_cast<sf::Uint8>(item.fader.GetFadeRatio() * targetAlpha);
            item.sprite.setColor(c);
        }
    }
}

void TitleState::Draw(sf::RenderWindow& window)
{
    window.clear(sf::Color::Black);

    if (m_shaderActive)
    {
        m_pixelateShader.setUniform("texture", sf::Shader::CurrentTexture);
        m_pixelateShader.setUniform("pixelSize", m_currentPixelSize);
        m_pixelateShader.setUniform("resolution", sf::Vector2f(Core::VIRTUAL_WIDTH, Core::VIRTUAL_HEIGHT));
        window.draw(m_compositeSprite, &m_pixelateShader);
    }
    else
    {
        window.draw(m_compositeSprite);
    }

    for (const auto& item : m_illustrations)
    {
        if (item.started)
        {
            window.draw(item.sprite);
        }
    }

    if (!m_inputLocked)
    {
        window.draw(m_promptText);
    }
}
