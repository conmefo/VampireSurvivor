#pragma once

#include "../../World/TileMap.h"
#include "../BaseState.h"


class GameState : public BaseState {
public:
  explicit GameState(StateContext context);
  ~GameState() override = default;

  void Init() override;
  void HandleInput(sf::Event &event, sf::RenderWindow &window) override;
  void Update(float dt) override;
  void Draw(sf::RenderWindow &window) override;

private:
  void UpdateCamera(float dt);
  void WrapCameraToMap();

  static constexpr float CameraSpeed = 100.0f;
  static constexpr float ViewWidth = 640.0f;
  static constexpr float ViewHeight = 480.0f;

  TileMap m_tileMap;
  sf::View m_worldView;
  sf::Vector2f m_cameraCenter;
};
