#pragma once

#include "../BaseState.h"
#include "../StateContext.h"
#include "Core/Animation/SpriteAnimator.h"
#include "../../World/TileMapManager.h"
#include <SFML/Graphics.hpp>
#include <atomic>
#include <thread>
#include <string>

class StageLoadingState : public BaseState
{
public:
    StageLoadingState(StateContext context, TileMapManager& mapManager, const std::string& characterId, int stageId);
    ~StageLoadingState() override;

    void Init() override;
    void HandleInput(sf::Event& event, sf::RenderWindow& window) override;
    void Update(float dt) override;
    void Draw(sf::RenderWindow& window) override;

private:
    void LoadMapWorker();

    TileMapManager& m_mapManager;
    std::string m_characterId;
    int m_stageId;
    
    sf::Text m_loadingText;
    
    std::thread m_workerThread;
    std::atomic<bool> m_isLoadComplete;
    std::atomic<bool> m_hasError;

    sf::Sprite m_treasureSprite;
    SpriteAnimator m_treasureAnimator;
    AnimationData m_treasureAnimData;
};
