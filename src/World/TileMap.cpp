#include "TileMap.h"
#include <algorithm>
#include <cmath>

bool TileMap::Load(const std::string& texturePath)
{
    if(!m_tileset.loadFromFile(texturePath))
    {
        return false;
    }

    m_tileset.setSmooth(false);
    BuildMap();
    return true;
}

void TileMap::Draw(sf::RenderTarget& target, const sf::View& view)
{
    if(m_tiles.empty())
    {
        return;
    }

    sf::Vector2f center = view.getCenter();
    sf::Vector2f size = view.getSize();

    float left = center.x - size.x / 2.0f;
    float top = center.y - size.y / 2.0f;
    float right = center.x + size.x / 2.0f;
    float bottom = center.y + size.y / 2.0f;

    int startX = std::max(0, static_cast<int>(std::floor(left / TileSize)));
    int startY = std::max(0, static_cast<int>(std::floor(top / TileSize)));
    int endX = std::min(MapWidth - 1, static_cast<int>(std::ceil(right / TileSize)));
    int endY = std::min(MapHeight - 1, static_cast<int>(std::ceil(bottom / TileSize)));

    m_visibleVertices.clear();
    m_visibleVertices.setPrimitiveType(sf::Quads);

    for(int y = startY; y <= endY; ++y)
    {
        for(int x = startX; x <= endX; ++x)
        {
            AppendTile(m_visibleVertices, x, y, m_tiles[y * MapWidth + x]);
        }
    }

    sf::RenderStates states;
    states.texture = &m_tileset;
    target.draw(m_visibleVertices, states);
}

sf::Vector2f TileMap::GetWorldSize() const
{
    return sf::Vector2f(
        static_cast<float>(MapWidth * TileSize),
        static_cast<float>(MapHeight * TileSize)
    );
}

void TileMap::BuildMap()
{
    m_tiles.assign(MapWidth * MapHeight, TileId::Grass);
}

sf::IntRect TileMap::GetTextureRect(TileId tile) const
{
    switch(tile)
    {
        case TileId::Grass:
            return sf::IntRect(0, 0, 32, 32);
    }

    return sf::IntRect(0, 0, 32, 32);
}

void TileMap::AppendTile(sf::VertexArray& vertices, int x, int y, TileId tile) const
{
    sf::IntRect texRect = GetTextureRect(tile);

    float worldX = static_cast<float>(x * TileSize);
    float worldY = static_cast<float>(y * TileSize);
    float worldRight = worldX + TileSize;
    float worldBottom = worldY + TileSize;

    float texLeft = static_cast<float>(texRect.left);
    float texTop = static_cast<float>(texRect.top);
    float texRight = static_cast<float>(texRect.left + texRect.width);
    float texBottom = static_cast<float>(texRect.top + texRect.height);

    vertices.append(sf::Vertex(sf::Vector2f(worldX, worldY), sf::Vector2f(texLeft, texTop)));
    vertices.append(sf::Vertex(sf::Vector2f(worldRight, worldY), sf::Vector2f(texRight, texTop)));
    vertices.append(sf::Vertex(sf::Vector2f(worldRight, worldBottom), sf::Vector2f(texRight, texBottom)));
    vertices.append(sf::Vertex(sf::Vector2f(worldX, worldBottom), sf::Vector2f(texLeft, texBottom)));
}
