#include "GameState.h"
#include "../StateManager.h"

#include <cmath>
#include <iostream>

GameState::GameState(StateContext context)
    : BaseState(context), m_cameraCenter(400.0f, 300.0f) {}

void GameState::Init() {
  std::cout << "GameState Init" << std::endl;

  if (!m_tileMap.Load("assets/images/maps/forest_tiles.png")) {
    std::cerr << "Failed to load forest tile assets" << std::endl;
  }

  m_worldView.setSize(ViewWidth, ViewHeight);
  ApplyCameraToView();

  m_enemyPool.Prewarm(EnemyType::Bat1, 1);
  m_enemyPool.Acquire(EnemyType::Bat1, sf::Vector2f(500.0f, 300.0f));
}

void GameState::HandleInput(sf::Event &event, sf::RenderWindow &window) {
  (void)window;

  if (event.type == sf::Event::KeyPressed &&
      event.key.code == sf::Keyboard::Escape) {
    m_context.stateManager.PopState();
  } else if (event.type == sf::Event::Resized) {
    m_worldView.setSize(ViewWidth, ViewHeight);
    ApplyCameraToView();
  }
}

void GameState::Update(float dt) {
  UpdateCamera(dt);
  m_enemyPool.Update(dt, m_cameraCenter);
}

void GameState::Draw(sf::RenderWindow &window) {
  window.clear(sf::Color(12, 28, 12));

  sf::View previousView = window.getView();

  window.setView(m_worldView);
  m_tileMap.Draw(window, m_worldView);
  m_enemyPool.Draw(window);
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
