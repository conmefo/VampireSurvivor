#pragma once
#include "../../Entities/Enemy/EnemyDatabase.h"
#include "../../Entities/Enemy/EnemyPool.h"
#include "../../World/TileMap.h"
#include "../../World/TileMapManager.h"
#include "../../Entities/Player.h"
#include "../BaseState.h"
#include <memory>

class GameState : public BaseState {
  public:
    GameState(StateContext context, TileMapManager& mapManager, const std::string& selectedCharacterId);
    ~GameState() override = default;

    void Init() override;
    void HandleInput(sf::Event &event, sf::RenderWindow &window) override;
    void Update(float dt) override;
    void Draw(sf::RenderWindow &window) override;

  private:
    void LoadStage(int stageNumber);
    void ApplyCameraToView();
    sf::FloatRect GetViewBounds() const;
    void DrawHitboxes(sf::RenderTarget &target);

    static constexpr float CameraSpeed = 200.0f;
    static constexpr float ViewWidth = 1920.0f;
    static constexpr float ViewHeight = 1080.0f;

    TileMapManager& m_mapManager;
    TileMap* m_tileMap = nullptr;
    sf::View m_worldView;
    sf::Vector2f m_cameraCenter;
    EnemyDatabase m_enemyDatabase;
    EnemyPool m_enemyPool;
    std::unique_ptr<Player> m_player;
    std::string m_selectedCharacterId;
    int m_currentStage = 1;
    bool m_showHitboxes = false;
};
