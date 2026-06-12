#include "GameState.h"
#include "../StateManager.h"

#include <cmath>
#include <iostream>
#include <vector>

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

const std::vector<const char *> &GetStageEnemies(int stageNumber) {
    return stageNumber == 3 ? WarehouseEnemies : LibraryEnemies;
}

const char *GetStageMapPath(int stageNumber) {
    return stageNumber == 3 ? "assets/Data/maps/plant_map.json"
                            : "assets/Data/maps/library_map.json";
}

const char *GetStageName(int stageNumber) {
    return stageNumber == 3 ? "Dairy Plant" : "Inlaid Library";
}
} // namespace

GameState::GameState(StateContext context)
    : BaseState(context), m_cameraCenter(400.0f, 300.0f), m_enemyPool(m_enemyDatabase) {}

void GameState::Init() {
    std::cout << "GameState Init" << std::endl;

    m_worldView.setSize(ViewWidth, ViewHeight);
    ApplyCameraToView();

    if (!m_enemyDatabase.LoadFromFile("assets/Data/enemies/stage_enemies.json")) {
        std::cerr << "Failed to load stage enemies" << std::endl;
    }

    LoadStage(1);
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
    } else if (event.type == sf::Event::Resized) {
        m_worldView.setSize(ViewWidth, ViewHeight);
        ApplyCameraToView();
    }
}

void GameState::Update(float dt) {
    UpdateCamera(dt);
    m_enemyPool.Update(dt, m_cameraCenter);

    const std::vector<sf::FloatRect> obstacles =
        m_tileMap.GetEnemyCollisionRectsInArea(GetViewBounds());
    m_enemyPool.ResolveObstacleCollisions(obstacles);
    m_enemyPool.ResolveEnemyCollisions();
}

void GameState::Draw(sf::RenderWindow &window) {
    window.clear(sf::Color(12, 28, 12));

    sf::View previousView = window.getView();

    window.setView(m_worldView);
    m_tileMap.Draw(window, m_worldView);
    m_enemyPool.Draw(window);
    if (m_showHitboxes) {
        DrawHitboxes(window);
    }
    window.setView(previousView);
}

void GameState::LoadStage(int stageNumber) {
    m_currentStage = stageNumber;
    m_enemyPool.DeactivateAll();

    if (!m_tileMap.Load(GetStageMapPath(stageNumber))) {
        std::cerr << "Failed to load " << GetStageName(stageNumber) << " map" << std::endl;
    }

    m_cameraCenter = sf::Vector2f(400.0f, 300.0f);
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

void GameState::UpdateCamera(float dt) {
    sf::Vector2f direction(0.0f, 0.0f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        direction.y -= 1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        direction.y += 1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        direction.x -= 1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        direction.x += 1.0f;
    }

    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0.0f) {
        direction.x /= length;
        direction.y /= length;
        m_cameraCenter += direction * CameraSpeed * dt;
        ApplyCameraToView();
    }
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

    for (const sf::FloatRect &rect : m_tileMap.GetEnemyCollisionRectsInArea(GetViewBounds())) {
        obstacleHitbox.setPosition(rect.left, rect.top);
        obstacleHitbox.setSize(sf::Vector2f(rect.width, rect.height));
        target.draw(obstacleHitbox);
    }

    m_enemyPool.DrawDebug(target);
}
