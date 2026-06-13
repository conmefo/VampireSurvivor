#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class TileMap {
public:
  bool LoadAsync(const std::string &mapJsonPath);
  bool FinalizeMapTexture();
  void Draw(sf::RenderTarget &target, const sf::View &view);

  sf::Vector2f GetWorldSize() const;
  const std::vector<sf::FloatRect> &GetCollisionRects() const;
  std::vector<sf::FloatRect>
  GetCollisionRectsInArea(const sf::FloatRect &area) const;
  std::vector<sf::FloatRect>
  GetEnemyCollisionRectsInArea(const sf::FloatRect &area) const;

private:
  struct Tile {
    struct Vertex {
      sf::Vector2f position;
      sf::Vector2f texCoord;
    };

    int x = 0;
    int y = 0;
    sf::IntRect spriteRect;
    float e00 = 1.0f;
    float e01 = 0.0f;
    float e03 = 0.0f;
    float e10 = 0.0f;
    float e11 = 1.0f;
    float e13 = 0.0f;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
  };

  struct Layer {
    std::string name;
    bool visible = true;
    std::vector<Tile> tiles;
  };

  bool BuildMapImage();
  void AppendTileToImage(const Tile &tile);

  sf::Image m_atlasImage;
  std::vector<Layer> m_layers;
  std::vector<sf::FloatRect> m_collisionRects;
  std::vector<sf::FloatRect> m_enemyCollisionRects;
  sf::Image m_mapImage;
  sf::Texture m_mapTexture;

  int m_mapWidth = 0;
  int m_mapHeight = 0;
  int m_tileWidth = 0;
  int m_tileHeight = 0;
  bool m_mapTextureReady = false;
};
