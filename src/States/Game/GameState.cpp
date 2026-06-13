#include "GameState.h"
#include "../StateManager.h"

#include <cmath>
#include <iostream>
#include <vector>
#include "../../Core/Data/CharacterDataManager.h"

namespace {
const std::vector<const char *> LibraryEnemies = {
    "GHOST",      "MUD",          "BUER",
    "SHADERED",   "BOSS_COUNT1",  "BOSS_COUNT2",
    "MUMMY",      "MEDUSA1",      "ECTO1",
    "DULL0",      "WITCH1",       "WITCH2",
    "XLMEDUSA",   "XLMUMMY",      "SKULLNOAURA",
    "MASK_GOLD",  "MASK_SILVER",  "MASK_LEFT",
    "MASK_RIGHT", "BOSS_XLDEATH", "BOSS_TRICKSTER_NORMAL"};

const std::vector<const char *> WarehouseEnemies = {
    "SKELETON",     "MILK",           "ECTO1",
    "FISHMAN_1",    "LIZARD1_2",      "PILE1",
    "LIZARD2_3",    "PILE2",          "JELLYFISH",
    "SKELENIN1",    "PILE3",          "GOLEM1",
    "MIGNO1_5",     "MIGNO_3_5SWARM", "ARMORSPEAR_6",
    "ARMOR_6",      "SKELEWING",      "XLTRITON",
    "XLCOCKATRICE", "XLGOLEM1",       "XLARMOR_SWORD",
    "MASK_GOLD",    "MASK_SILVER",    "MASK_LEFT",
    "MASK_RIGHT",   "BOSS_XLDEATH",   "BOSS_STALKER_NORMAL"};

const std::vector<const char *> ForestEnemies = {
    "BAT1", "SKELETON", "ZOMBIE", "MUDMAN1", "GHOST", "XLBAT"};

const std::vector<const char *> &GetStageEnemies(int stageNumber) {
    if (stageNumber == 1) {
        return ForestEnemies;
    }

    return stageNumber == 3 ? WarehouseEnemies : LibraryEnemies;
}

const char *GetStageEnemyPath(int stageNumber) {
    return stageNumber == 1 ? "assets/Data/enemies/forest_enemies.json"
                            : "assets/Data/enemies/stage_enemies.json";
}

const char *GetStageMapPath(int stageNumber) {
    if (stageNumber == 1) {
        return "assets/Data/maps/forest_map.json";
    }

    return stageNumber == 3 ? "assets/Data/maps/plant_map.json"
                            : "assets/Data/maps/library_map.json";
}

const char *GetStageName(int stageNumber) {
    if (stageNumber == 1) {
        return "Mad Forest";
    }

    return stageNumber == 3 ? "Dairy Plant" : "Inlaid Library";
}
} // namespace

GameState::GameState(StateContext context, TileMapManager& mapManager, const std::string& selectedCharacterId)
    : BaseState(std::move(context)), m_mapManager(mapManager), m_enemyPool(m_enemyDatabase), m_selectedCharacterId(selectedCharacterId) {}

void GameState::Init() {
    std::cout << "GameState Init" << std::endl;

    m_worldView.setSize(ViewWidth, ViewHeight);

    LoadStage(1);

    const CharacterProfile& profile = m_context.characterData.GetCharacterById(m_selectedCharacterId);
    
    std::string originalSpriteName = profile.GetSpriteName();
    std::string prefix = originalSpriteName;
    size_t dotPos = prefix.find_last_of('.');
    if(dotPos != std::string::npos) { prefix = prefix.substr(0, dotPos); }
    size_t underscorePos = prefix.find_last_of('_');
    if(underscorePos != std::string::npos) { prefix = prefix.substr(0, underscorePos); }

    std::vector<sf::IntRect> frames;
    const sf::Texture* texture = nullptr;

    for(int i = 1; i <= profile.GetWalkingFrames(); ++i)
    {
        std::string frameName = prefix + (i < 10 ? "_0" : "_") + std::to_string(i);
        AssetTextureData data = m_context.atlas.GetTextureData(frameName);
        if(data.texture)
        {
            texture = data.texture;
            frames.push_back(data.rect);
        }
    }

    if(texture && !frames.empty())
    {
        m_player = std::make_unique<Player>(profile, *texture, frames);
        m_player->SetPosition(m_cameraCenter);
    }
    else
    {
        std::cerr << "Failed to find texture data for player sprite: " << profile.GetSpriteName() << std::endl;
    }

    ApplyCameraToView();
}

void GameState::HandleInput(sf::Event &event, sf::RenderWindow &window) {
    (void)window;

    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        m_context.stateManager.PopState();
    } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::H) {
        m_showHitboxes = !m_showHitboxes;
    } else if (event.type == sf::Event::KeyPressed &&
               (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::Numpad2)) {
        LoadStage(2);
    } else if (event.type == sf::Event::KeyPressed &&
               (event.key.code == sf::Keyboard::Num3 || event.key.code == sf::Keyboard::Numpad3)) {
        LoadStage(3);
    } else if (event.type == sf::Event::KeyPressed &&
               (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1)) {
        LoadStage(1);
    } else if (event.type == sf::Event::Resized) {
        m_worldView.setSize(ViewWidth, ViewHeight);
        ApplyCameraToView();
    }
}

void GameState::Update(float dt) {
    if(m_player)
    {
        m_player->Update(dt);
        m_cameraCenter = m_player->GetPosition();
        ApplyCameraToView();
    }
    m_enemyPool.Update(dt, m_cameraCenter);

    if (m_tileMap) {
        const std::vector<sf::FloatRect> obstacles =
            m_tileMap->GetEnemyCollisionRectsInArea(GetViewBounds());
        m_enemyPool.ResolveObstacleCollisions(obstacles);
    }
    m_enemyPool.ResolveEnemyCollisions();
}

void GameState::Draw(sf::RenderWindow &window) {
    window.clear(sf::Color(12, 28, 12));

    sf::View previousView = window.getView();

    window.setView(m_worldView);
    if (m_tileMap) {
        m_tileMap->Draw(window, m_worldView);
    }
    m_enemyPool.Draw(window);
    
    if(m_player)
    {
        m_player->Draw(window);
    }

    if (m_showHitboxes) {
        DrawHitboxes(window);
    }

    // Draw 16:10 Dim Bars Overlay (1920x1080 view -> 1728x1080 play area = 96px bars)
    sf::View uiView(sf::FloatRect(0.0f, 0.0f, ViewWidth, ViewHeight));
    window.setView(uiView);

    sf::RectangleShape leftDimBar(sf::Vector2f(96.0f, ViewHeight));
    leftDimBar.setFillColor(sf::Color(0, 0, 0, 180));
    leftDimBar.setPosition(0.0f, 0.0f);
    window.draw(leftDimBar);

    sf::RectangleShape rightDimBar(sf::Vector2f(96.0f, ViewHeight));
    rightDimBar.setFillColor(sf::Color(0, 0, 0, 180));
    rightDimBar.setPosition(ViewWidth - 96.0f, 0.0f);
    window.draw(rightDimBar);

    window.setView(previousView);
}

void GameState::LoadStage(int stageNumber) {
    m_currentStage = stageNumber;
    m_enemyPool.Clear();

    m_tileMap = m_mapManager.GetMap(stageNumber);
    if (!m_tileMap) {
        std::cerr << "GameState: Map for stage " << stageNumber << " was not preloaded!" << std::endl;
    }

    if (!m_enemyDatabase.LoadFromFile(GetStageEnemyPath(stageNumber))) {
        std::cerr << "Failed to load " << GetStageName(stageNumber) << " enemies" << std::endl;
    }

    if (m_tileMap) {
        sf::Vector2f worldSize = m_tileMap->GetWorldSize();
        m_cameraCenter = sf::Vector2f(worldSize.x / 2.0f, worldSize.y / 2.0f);
    } else {
        m_cameraCenter = sf::Vector2f(400.0f, 300.0f);
    }
    
    if (m_player) {
        m_player->SetPosition(m_cameraCenter);
    }

    ApplyCameraToView();

    const std::vector<const char *> &enemyIds = GetStageEnemies(stageNumber);
    for (const char *enemyId : enemyIds) {
        m_enemyPool.Prewarm(enemyId, 1);
    }

    const int columns = 7;
    const float spacing = 58.0f;
    const sf::Vector2f spawnStart(m_cameraCenter.x - 180.0f, m_cameraCenter.y - 120.0f);
    for (std::size_t i = 0; i < enemyIds.size(); ++i) {
        const float x = spawnStart.x + static_cast<float>(i % columns) * spacing;
        const float y = spawnStart.y + static_cast<float>(i / columns) * spacing;
        m_enemyPool.Acquire(enemyIds[i], sf::Vector2f(x, y));
    }

    std::cout << "Loaded stage " << stageNumber << ": " << GetStageName(stageNumber) << " with "
              << enemyIds.size() << " enemies" << std::endl;
}

void GameState::ApplyCameraToView() {
    m_worldView.setCenter(std::round(m_cameraCenter.x), std::round(m_cameraCenter.y));
}

sf::FloatRect GameState::GetViewBounds() const {
    const sf::Vector2f viewSize = m_worldView.getSize();
    const sf::Vector2f viewCenter = m_worldView.getCenter();
    return sf::FloatRect(viewCenter.x - viewSize.x / 2.0f, viewCenter.y - viewSize.y / 2.0f,
                         viewSize.x, viewSize.y);
}

void GameState::DrawHitboxes(sf::RenderTarget &target) {
    sf::RectangleShape obstacleHitbox;
    obstacleHitbox.setFillColor(sf::Color(255, 40, 40, 35));
    obstacleHitbox.setOutlineColor(sf::Color(255, 40, 40, 220));
    obstacleHitbox.setOutlineThickness(1.0f);

    if (m_tileMap) {
        for (const sf::FloatRect &rect : m_tileMap->GetEnemyCollisionRectsInArea(GetViewBounds())) {
            obstacleHitbox.setPosition(rect.left, rect.top);
            obstacleHitbox.setSize(sf::Vector2f(rect.width, rect.height));
            target.draw(obstacleHitbox);
        }
    }

    m_enemyPool.DrawDebug(target);
}
