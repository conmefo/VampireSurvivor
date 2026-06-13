#include "StageLoadingState.h"
#include "GameState.h"
#include "../StateManager.h"
#include "../../Core/WindowSettings.h"
#include "../../Core/WindowSettings.h"
#include <iostream>
#include <sstream>

StageLoadingState::StageLoadingState(StateContext context, TileMapManager& mapManager, const std::string& characterId, int stageId)
    : BaseState(context), m_mapManager(mapManager), m_characterId(characterId), m_stageId(stageId), m_isLoadComplete(false), m_hasError(false)
{
}

StageLoadingState::~StageLoadingState()
{
    if(m_workerThread.joinable())
    {
        m_workerThread.join();
    }
}

void StageLoadingState::Init()
{
    const sf::Font* font = m_context.fonts.GetPtr(FontID::Main);
    if (font)
    {
        m_loadingText.setFont(*font);
        m_loadingText.setString("Loading");
        m_loadingText.setCharacterSize(38);
        m_loadingText.setFillColor(sf::Color::White);
    }
    
    m_treasureAnimData.frameDuration = 0.3f; // 2.5 frames per second
    m_treasureAnimData.isLooping = true;
    
    for(int i = 2; i <= 4; ++i)
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

    float vx = Core::VIRTUAL_WIDTH;
    float vy = Core::VIRTUAL_HEIGHT;

    m_treasureSprite.setPosition(vx - 175.0f, vy - 140.0f);
    m_treasureSprite.setScale(4.3f, 3.2f);
    
    sf::FloatRect bounds = m_loadingText.getLocalBounds();
    m_loadingText.setOrigin(bounds.left + bounds.width, 0.0f);
    m_loadingText.setPosition(vx - 200.0f, vy - 110.0f);
    
    // Spawn background thread to load the map JSON and build the RAM image
    m_workerThread = std::thread(&StageLoadingState::LoadMapWorker, this);
}

void StageLoadingState::LoadMapWorker()
{
    std::string mapPath;
    if (m_stageId == 1) mapPath = "assets/Data/maps/forest_map.json";
    else if (m_stageId == 3) mapPath = "assets/Data/maps/plant_map.json";
    else mapPath = "assets/Data/maps/library_map.json";
    
    auto tileMap = std::make_unique<TileMap>();
    if (!tileMap->LoadAsync(mapPath))
    {
        m_hasError = true;
    }
    else
    {
        m_mapManager.SetMap(m_stageId, std::move(tileMap));
    }
    
    m_isLoadComplete = true;
}

void StageLoadingState::HandleInput(sf::Event& event, sf::RenderWindow& window)
{
    (void)event;
    (void)window;
}

void StageLoadingState::Update(float dt)
{
    m_treasureAnimator.Update(dt, m_treasureSprite);
    
    if (m_isLoadComplete)
    {
        if(m_workerThread.joinable())
        {
            m_workerThread.join();
        }
        
        if (!m_hasError)
        {
            TileMap* map = m_mapManager.GetMap(m_stageId);
            if (map)
            {
                // Finalize on main thread (upload sf::Image to GPU sf::Texture)
                if (!map->FinalizeMapTexture())
                {
                    std::cerr << "StageLoadingState: Failed to finalize map texture!" << std::endl;
                }
            }
        }
        else
        {
            std::cerr << "StageLoadingState: Error during background map load!" << std::endl;
        }
        
        // Transition to GameState
        m_context.stateManager.PopState();
        m_context.stateManager.AddState(std::make_unique<GameState>(m_context, m_mapManager, m_characterId));
    }
}

void StageLoadingState::Draw(sf::RenderWindow& window)
{
    window.clear(sf::Color::Black);
    window.draw(m_loadingText);
    window.draw(m_treasureSprite);
}
