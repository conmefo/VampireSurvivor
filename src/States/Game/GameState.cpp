#include "GameState.h"
#include "../StateManager.h"

#include <cmath>
#include <iostream>

GameState::GameState(StateContext context)
    : BaseState(context),
      m_cameraCenter(400.0f, 300.0f),
      m_enemyPool(m_enemyDatabase) {}

void GameState::Init() {
  std::cout << "GameState Init" << std::endl;

  if (!m_tileMap.Load("assets/Data/maps/forest_map.json")) {
    std::cerr << "Failed to load forest map" << std::endl;
  }

  m_worldView.setSize(ViewWidth, ViewHeight);
  ApplyCameraToView();

  if (!m_enemyDatabase.LoadFromFile("assets/Data/enemies/forest_enemies.json")) {
    std::cerr << "Failed to load forest enemies" << std::endl;
  }

  m_enemyPool.Prewarm("BAT1", 1);
  m_enemyPool.Prewarm("SKELETON", 1);
  m_enemyPool.Acquire("BAT1", sf::Vector2f(500.0f, 300.0f));
  m_enemyPool.Acquire("SKELETON", sf::Vector2f(560.0f, 340.0f));
}

void GameState::HandleInput(sf::Event &event, sf::RenderWindow &window) {
  (void)window;

  if (event.type == sf::Event::KeyPressed &&
      event.key.code == sf::Keyboard::Escape) {
    m_context.stateManager.PopState();
  } else if (event.type == sf::Event::KeyPressed &&
             event.key.code == sf::Keyboard::H) {
    m_showHitboxes = !m_showHitboxes;
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

  float length =
      std::sqrt(direction.x * direction.x + direction.y * direction.y);
  if (length > 0.0f) {
    direction.x /= length;
    direction.y /= length;
    m_cameraCenter += direction * CameraSpeed * dt;
    ApplyCameraToView();
  }
}

void GameState::ApplyCameraToView() {
  m_worldView.setCenter(std::round(m_cameraCenter.x),
                        std::round(m_cameraCenter.y));
}

sf::FloatRect GameState::GetViewBounds() const {
  const sf::Vector2f viewSize = m_worldView.getSize();
  const sf::Vector2f viewCenter = m_worldView.getCenter();
  return sf::FloatRect(viewCenter.x - viewSize.x / 2.0f,
                       viewCenter.y - viewSize.y / 2.0f, viewSize.x,
                       viewSize.y);
}

void GameState::DrawHitboxes(sf::RenderTarget &target) {
  sf::RectangleShape obstacleHitbox;
  obstacleHitbox.setFillColor(sf::Color(255, 40, 40, 35));
  obstacleHitbox.setOutlineColor(sf::Color(255, 40, 40, 220));
  obstacleHitbox.setOutlineThickness(1.0f);

  for (const sf::FloatRect &rect :
       m_tileMap.GetEnemyCollisionRectsInArea(GetViewBounds())) {
    obstacleHitbox.setPosition(rect.left, rect.top);
    obstacleHitbox.setSize(sf::Vector2f(rect.width, rect.height));
    target.draw(obstacleHitbox);
  }

  m_enemyPool.DrawDebug(target);
}
