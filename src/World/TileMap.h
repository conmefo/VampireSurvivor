#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <string>
#include <vector>

enum class TileId {
  Forest = 0,
  Forest1 = 1,
  Forest2 = 2,
  Water1 = 3,
  Water2 = 4,
  Water3 = 5,
  Water4 = 6
};

class TileMap {
public:
  static constexpr int TileSize = 50;
  static constexpr int MapWidth = 90;
  static constexpr int MapHeight = 60;

  bool Load(const std::string &texturePath);
  void Draw(sf::RenderTarget &target, const sf::View &view);

  sf::Vector2f GetWorldSize() const;

private:
  void BuildMap();
  bool BuildMapTexture();
  TileId GetTileFromGridValue(int value) const;
  sf::IntRect GetTextureRect(TileId tile) const;
  int WrapIndex(int value, int max) const;
  void AppendTile(sf::VertexArray &vertices, int drawX, int drawY, int mapX,
                  int mapY, TileId tile) const;

  static constexpr std::size_t TileTypeCount = 7;

  std::array<sf::Texture, TileTypeCount> m_textures;
  std::vector<TileId> m_tiles;
  std::array<sf::VertexArray, TileTypeCount> m_visibleVertices;
  sf::RenderTexture m_mapTexture;
  bool m_mapTextureReady = false;
};
