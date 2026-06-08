#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

enum class TileId
{
    Grass
};

class TileMap
{
public:
    static constexpr int TileSize = 32;
    static constexpr int MapWidth = 80;
    static constexpr int MapHeight = 60;

    bool Load(const std::string& texturePath);
    void Draw(sf::RenderTarget& target, const sf::View& view);

    sf::Vector2f GetWorldSize() const;

private:
    void BuildMap();
    sf::IntRect GetTextureRect(TileId tile) const;
    void AppendTile(sf::VertexArray& vertices, int x, int y, TileId tile) const;

    sf::Texture m_tileset;
    std::vector<TileId> m_tiles;
    sf::VertexArray m_visibleVertices;
};
