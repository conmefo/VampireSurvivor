#include "TileMap.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <utility>

namespace {
sf::IntRect ReadRect(const nlohmann::json &rect) {
  return sf::IntRect(rect.value("x", 0), rect.value("y", 0),
                     rect.value("width", 0), rect.value("height", 0));
}
} // namespace

bool TileMap::Load(const std::string &mapJsonPath) {
  std::ifstream file(mapJsonPath);
  if (!file.is_open()) {
    std::cerr << "TileMap: failed to open map JSON at " << mapJsonPath
              << std::endl;
    return false;
  }

  nlohmann::json mapJson;
  try {
    file >> mapJson;
  } catch (const nlohmann::json::parse_error &error) {
    std::cerr << "TileMap: JSON parse error in " << mapJsonPath << ": "
              << error.what() << std::endl;
    return false;
  }

  const std::string atlasPath = mapJson.value("atlas", "");
  if (atlasPath.empty() || !m_atlasTexture.loadFromFile(atlasPath)) {
    std::cerr << "TileMap: failed to load atlas at " << atlasPath << std::endl;
    return false;
  }
  m_atlasTexture.setSmooth(false);

  m_mapWidth = mapJson.value("mapWidth", 0);
  m_mapHeight = mapJson.value("mapHeight", 0);
  m_tileWidth = mapJson.value("tileWidth", 0);
  m_tileHeight = mapJson.value("tileHeight", 0);

  if (m_mapWidth <= 0 || m_mapHeight <= 0 || m_tileWidth <= 0 ||
      m_tileHeight <= 0) {
    std::cerr << "TileMap: invalid map dimensions in " << mapJsonPath
              << std::endl;
    return false;
  }

  m_layers.clear();
  m_enemyCollisionRects.clear();
  for (const nlohmann::json &layerJson : mapJson.value("layers", nlohmann::json::array())) {
    Layer layer;
    layer.name = layerJson.value("name", "");
    layer.visible = layerJson.value("visible", true);

    for (const nlohmann::json &tileJson : layerJson.value("tiles", nlohmann::json::array())) {
      Tile tile;
      tile.x = tileJson.value("x", 0);
      tile.y = tileJson.value("y", 0);
      tile.spriteRect = ReadRect(tileJson.at("spriteRect"));
      const nlohmann::json transform =
          tileJson.value("transform", nlohmann::json::object());
      tile.e00 = transform.value("e00", 1.0f);
      tile.e01 = transform.value("e01", 0.0f);
      tile.e03 = transform.value("e03", 0.0f);
      tile.e10 = transform.value("e10", 0.0f);
      tile.e11 = transform.value("e11", 1.0f);
      tile.e13 = transform.value("e13", 0.0f);

      for (const nlohmann::json &vertexJson :
           tileJson.value("vertices", nlohmann::json::array())) {
        Tile::Vertex vertex;
        vertex.position =
            sf::Vector2f(vertexJson.value("x", 0.0f),
                         vertexJson.value("y", 0.0f));
        vertex.texCoord =
            sf::Vector2f(vertexJson.value("u", 0.0f),
                         vertexJson.value("v", 0.0f));
        tile.vertices.push_back(vertex);
      }
      for (const nlohmann::json &indexJson :
           tileJson.value("indices", nlohmann::json::array())) {
        tile.indices.push_back(indexJson.get<unsigned int>());
      }

      if (tile.spriteRect.width > 0 && tile.spriteRect.height > 0) {
        if (layer.name == "Walls") {
          m_enemyCollisionRects.emplace_back(
              static_cast<float>(tile.x * m_tileWidth),
              static_cast<float>(tile.y * m_tileHeight),
              static_cast<float>(m_tileWidth),
              static_cast<float>(m_tileHeight));
        }

        layer.tiles.push_back(tile);
      }
    }

    if (layer.visible && !layer.tiles.empty()) {
      m_layers.push_back(std::move(layer));
    }
  }

  if (m_layers.empty()) {
    std::cerr << "TileMap: no visible tile layers in " << mapJsonPath
              << std::endl;
    return false;
  }

  m_collisionRects.clear();
  std::vector<sf::FloatRect> tmxEnemyCollisionRects;
  for (const nlohmann::json &rectJson :
       mapJson.value("collisionRects", nlohmann::json::array())) {
    sf::FloatRect collisionRect(
        static_cast<float>(rectJson.value("x", 0) * m_tileWidth),
        static_cast<float>(rectJson.value("y", 0) * m_tileHeight),
        static_cast<float>(rectJson.value("width", 0) * m_tileWidth),
        static_cast<float>(rectJson.value("height", 0) * m_tileHeight));
    m_collisionRects.push_back(collisionRect);

    const std::string source = rectJson.value("source", "");
    if (source.find("_Walls") != std::string::npos) {
      tmxEnemyCollisionRects.push_back(collisionRect);
    }
  }

  if (m_enemyCollisionRects.empty()) {
    m_enemyCollisionRects = std::move(tmxEnemyCollisionRects);
  }

  return BuildMapTexture();
}

void TileMap::Draw(sf::RenderTarget &target, const sf::View &view) {
  if (!m_mapTextureReady) {
    return;
  }

  const sf::Vector2f center = view.getCenter();
  const sf::Vector2f size = view.getSize();
  const sf::Vector2f worldSize = GetWorldSize();
  sf::Sprite mapSprite(m_mapTexture.getTexture());

  const float left = center.x - size.x / 2.0f;
  const float top = center.y - size.y / 2.0f;
  const float right = center.x + size.x / 2.0f;
  const float bottom = center.y + size.y / 2.0f;

  const int startX = static_cast<int>(std::floor(left / worldSize.x)) - 1;
  const int startY = static_cast<int>(std::floor(top / worldSize.y)) - 1;
  const int endX = static_cast<int>(std::ceil(right / worldSize.x)) + 1;
  const int endY = static_cast<int>(std::ceil(bottom / worldSize.y)) + 1;

  for (int y = startY; y <= endY; ++y) {
    for (int x = startX; x <= endX; ++x) {
      mapSprite.setPosition(static_cast<float>(x) * worldSize.x,
                            static_cast<float>(y) * worldSize.y);
      target.draw(mapSprite);
    }
  }
}

sf::Vector2f TileMap::GetWorldSize() const {
  return sf::Vector2f(static_cast<float>(m_mapWidth * m_tileWidth),
                      static_cast<float>(m_mapHeight * m_tileHeight));
}

const std::vector<sf::FloatRect> &TileMap::GetCollisionRects() const {
  return m_collisionRects;
}

std::vector<sf::FloatRect>
TileMap::GetCollisionRectsInArea(const sf::FloatRect &area) const {
  std::vector<sf::FloatRect> rects;
  const sf::Vector2f worldSize = GetWorldSize();
  if (worldSize.x <= 0.0f || worldSize.y <= 0.0f) {
    return rects;
  }

  const int startX = static_cast<int>(std::floor(area.left / worldSize.x)) - 1;
  const int startY = static_cast<int>(std::floor(area.top / worldSize.y)) - 1;
  const int endX =
      static_cast<int>(std::ceil((area.left + area.width) / worldSize.x)) + 1;
  const int endY =
      static_cast<int>(std::ceil((area.top + area.height) / worldSize.y)) + 1;

  for (int y = startY; y <= endY; ++y) {
    for (int x = startX; x <= endX; ++x) {
      const sf::Vector2f offset(static_cast<float>(x) * worldSize.x,
                                static_cast<float>(y) * worldSize.y);
      for (const sf::FloatRect &baseRect : m_collisionRects) {
        sf::FloatRect repeatedRect(baseRect.left + offset.x,
                                   baseRect.top + offset.y, baseRect.width,
                                   baseRect.height);
        if (repeatedRect.intersects(area)) {
          rects.push_back(repeatedRect);
        }
      }
    }
  }

  return rects;
}

std::vector<sf::FloatRect>
TileMap::GetEnemyCollisionRectsInArea(const sf::FloatRect &area) const {
  std::vector<sf::FloatRect> rects;
  const sf::Vector2f worldSize = GetWorldSize();
  if (worldSize.x <= 0.0f || worldSize.y <= 0.0f) {
    return rects;
  }

  const int startX = static_cast<int>(std::floor(area.left / worldSize.x)) - 1;
  const int startY = static_cast<int>(std::floor(area.top / worldSize.y)) - 1;
  const int endX =
      static_cast<int>(std::ceil((area.left + area.width) / worldSize.x)) + 1;
  const int endY =
      static_cast<int>(std::ceil((area.top + area.height) / worldSize.y)) + 1;

  for (int y = startY; y <= endY; ++y) {
    for (int x = startX; x <= endX; ++x) {
      const sf::Vector2f offset(static_cast<float>(x) * worldSize.x,
                                static_cast<float>(y) * worldSize.y);
      for (const sf::FloatRect &baseRect : m_enemyCollisionRects) {
        sf::FloatRect repeatedRect(baseRect.left + offset.x,
                                   baseRect.top + offset.y, baseRect.width,
                                   baseRect.height);
        if (repeatedRect.intersects(area)) {
          rects.push_back(repeatedRect);
        }
      }
    }
  }

  return rects;
}

bool TileMap::BuildMapTexture() {
  const sf::Vector2f worldSize = GetWorldSize();
  if (!m_mapTexture.create(static_cast<unsigned int>(worldSize.x),
                           static_cast<unsigned int>(worldSize.y))) {
    m_mapTextureReady = false;
    return false;
  }

  m_mapTexture.clear(sf::Color(12, 28, 12, 255));

  sf::RenderStates states;
  states.texture = &m_atlasTexture;

  for (const Layer &layer : m_layers) {
    sf::VertexArray vertices(sf::Triangles);
    for (const Tile &tile : layer.tiles) {
      AppendTile(vertices, tile);
    }
    m_mapTexture.draw(vertices, states);
  }

  m_mapTexture.display();
  m_mapTextureReady = true;
  return true;
}

void TileMap::AppendTile(sf::VertexArray &vertices, const Tile &tile) const {
  const float worldX = static_cast<float>(tile.x * m_tileWidth);
  const float worldY = static_cast<float>(tile.y * m_tileHeight);
  if (!tile.vertices.empty() && !tile.indices.empty()) {
    for (const unsigned int index : tile.indices) {
      if (index >= tile.vertices.size()) {
        continue;
      }

      const Tile::Vertex &vertex = tile.vertices[index];
      vertices.append(sf::Vertex(
          sf::Vector2f(worldX + vertex.position.x, worldY + vertex.position.y),
          vertex.texCoord));
    }
    return;
  }

  const float width = static_cast<float>(tile.spriteRect.width);
  const float height = static_cast<float>(tile.spriteRect.height);
  const float unitToPixels = static_cast<float>(m_tileWidth) / 0.32f;
  const float translateX = tile.e03 * unitToPixels;
  const float translateY = tile.e13 * unitToPixels;

  const float texLeft = static_cast<float>(tile.spriteRect.left);
  const float texTop = static_cast<float>(tile.spriteRect.top);
  const float texRight =
      static_cast<float>(tile.spriteRect.left + tile.spriteRect.width);
  const float texBottom =
      static_cast<float>(tile.spriteRect.top + tile.spriteRect.height);

  const auto transformPoint = [&](float localX, float localY) {
    return sf::Vector2f(worldX + tile.e00 * localX + tile.e01 * localY +
                            translateX,
                        worldY + tile.e10 * localX + tile.e11 * localY +
                            translateY);
  };

  const sf::Vertex topLeft(transformPoint(0.0f, 0.0f),
                           sf::Vector2f(texLeft, texTop));
  const sf::Vertex topRight(transformPoint(width, 0.0f),
                            sf::Vector2f(texRight, texTop));
  const sf::Vertex bottomRight(transformPoint(width, height),
                               sf::Vector2f(texRight, texBottom));
  const sf::Vertex bottomLeft(transformPoint(0.0f, height),
                              sf::Vector2f(texLeft, texBottom));

  vertices.append(topLeft);
  vertices.append(topRight);
  vertices.append(bottomRight);
  vertices.append(topLeft);
  vertices.append(bottomRight);
  vertices.append(bottomLeft);
}
