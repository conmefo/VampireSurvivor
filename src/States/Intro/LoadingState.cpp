#include "LoadingState.h"
#include "WarningState.h"
#include "../StateManager.h"
#include <iostream>
#include <cmath>

LoadingState::LoadingState(StateContext context)
    : BaseState(context)
    , m_config(Config())
    , m_assetPaths(AssetPaths())
    , m_useTextures(true)
    , m_elapsedTime(0.0f)
    , m_taskTimer(0.0f)
    , m_animTimer(0.0f)
    , m_progress(0.0f)
    , m_currentTaskIndex(0)
    , m_currentFrame(0)
{
}

LoadingState::LoadingState(StateContext context, const Config& config, const AssetPaths& assetPaths)
    : BaseState(context)
    , m_config(config)
    , m_assetPaths(assetPaths)
    , m_useTextures(true)
    , m_elapsedTime(0.0f)
    , m_taskTimer(0.0f)
    , m_animTimer(0.0f)
    , m_progress(0.0f)
    , m_currentTaskIndex(0)
    , m_currentFrame(0)
{
}

void LoadingState::Init()
{
    if(!m_font.loadFromFile(m_assetPaths.fontPath))
    {
        std::cerr << "Failed to load font: " << m_assetPaths.fontPath << std::endl;
    }

    m_text.setFont(m_font);
    m_text.setCharacterSize(24);
    m_text.setFillColor(sf::Color::White);

    UpdateText();

    sf::FloatRect textRect = m_text.getLocalBounds();
    m_text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);

    // Load multiple texture frames
    m_chestTextures.resize(m_assetPaths.chestFramePaths.size());
    for(size_t i = 0; i < m_assetPaths.chestFramePaths.size(); ++i)
    {
        if(!m_chestTextures[i].loadFromFile(m_assetPaths.chestFramePaths[i]))
        {
            std::cerr << "Failed to load frame: " << m_assetPaths.chestFramePaths[i] << std::endl;
            m_useTextures = false;
        }
    }

    if(m_useTextures && !m_chestTextures.empty())
    {
        m_chestSprite.setTexture(m_chestTextures[0]);
        sf::Vector2u texSize = m_chestTextures[0].getSize();
        m_chestSprite.setOrigin(texSize.x / 2.0f, texSize.y / 2.0f);
    }
    else
    {
        m_chestPlaceholder.setSize(sf::Vector2f(80.0f, 80.0f));
        m_chestPlaceholder.setOrigin(40.0f, 40.0f);
        m_chestPlaceholder.setFillColor(sf::Color(139, 69, 19));
        m_chestPlaceholder.setOutlineThickness(2.0f);
        m_chestPlaceholder.setOutlineColor(sf::Color::Yellow);
    }

    m_progressBarOutline.setSize(sf::Vector2f(400.0f, 20.0f));
    m_progressBarOutline.setOrigin(200.0f, 10.0f);
    m_progressBarOutline.setFillColor(sf::Color::Transparent);
    m_progressBarOutline.setOutlineThickness(2.0f);
    m_progressBarOutline.setOutlineColor(sf::Color::White);

    m_progressBarFill.setSize(sf::Vector2f(0.0f, 16.0f));
    m_progressBarFill.setOrigin(0.0f, 8.0f);
    m_progressBarFill.setFillColor(sf::Color::Green);
}

void LoadingState::HandleInput(sf::Event& event)
{
}

void LoadingState::Update(float dt)
{
    if(m_elapsedTime < m_config.totalLoadTime)
    {
        m_elapsedTime += dt;
        m_progress = (m_elapsedTime / m_config.totalLoadTime) * 100.0f;
        if(m_progress >= 100.0f)
        {
            m_progress = 100.0f;
        }
    }

    m_taskTimer += dt;
    if(m_taskTimer >= m_config.taskSwitchInterval)
    {
        m_taskTimer = 0.0f;
        if(!m_config.loadingTasks.empty())
        {
            m_currentTaskIndex = (m_currentTaskIndex + 1) % m_config.loadingTasks.size();
        }
    }

    UpdateText();
    sf::FloatRect textRect = m_text.getLocalBounds();
    m_text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);

    float currentFillWidth = (m_progress / 100.0f) * 396.0f;
    m_progressBarFill.setSize(sf::Vector2f(currentFillWidth, 16.0f));

    m_animTimer += dt;
    if(m_animTimer >= m_config.frameDuration)
    {
        m_animTimer = 0.0f;
        size_t totalFrames = m_useTextures ? m_chestTextures.size() : 4;
        if(totalFrames > 0)
        {
            m_currentFrame = (m_currentFrame + 1) % totalFrames;
        }
    }

    UpdateChestVisuals();

    if(m_progress >= 100.0f)
    {
        m_context.stateManager.PopState();
        m_context.stateManager.AddState(std::make_unique<WarningState>(m_context));
    }
}

void LoadingState::Draw(sf::RenderWindow& window)
{
    sf::Vector2f center(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

    if(m_useTextures)
    {
        m_chestSprite.setPosition(center.x, center.y - 100.0f);
        window.draw(m_chestSprite);
    }
    else
    {
        m_chestPlaceholder.setPosition(center.x, center.y - 100.0f);
        window.draw(m_chestPlaceholder);
    }

    m_text.setPosition(center.x, center.y + 20.0f);
    m_progressBarOutline.setPosition(center.x, center.y + 100.0f);

    m_progressBarFill.setPosition(center.x - 198.0f, center.y + 100.0f);

    window.draw(m_text);
    window.draw(m_progressBarOutline);
    window.draw(m_progressBarFill);
}

void LoadingState::UpdateText()
{
    std::string task = "Loading";
    if(!m_config.loadingTasks.empty())
    {
        task = m_config.loadingTasks[m_currentTaskIndex];
    }
    m_text.setString(task + "... (" + std::to_string(static_cast<int>(m_progress)) + "%)");
}

void LoadingState::UpdateChestVisuals()
{
    if(m_useTextures && !m_chestTextures.empty())
    {
        if(m_currentFrame < static_cast<int>(m_chestTextures.size()))
        {
            m_chestSprite.setTexture(m_chestTextures[m_currentFrame]);
            sf::Vector2u texSize = m_chestTextures[m_currentFrame].getSize();
            m_chestSprite.setOrigin(texSize.x / 2.0f, texSize.y / 2.0f);
        }
    }
    else
    {
        float scaleFactor = 1.0f + 0.1f * std::sin(m_currentFrame * 3.14159f / 2.0f);
        m_chestPlaceholder.setScale(scaleFactor, scaleFactor);
    }
}
