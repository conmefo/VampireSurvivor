#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <cstdint>
#include <vector>

struct GridCell
{
    uint32_t begin = 0;
    uint32_t count = 0;
};

class UniformSpatialGrid
{
public:
    explicit UniformSpatialGrid(float cellSize = 48.0f);
    ~UniformSpatialGrid() = default;

    // 3-Pass Rebuild: Count -> Prefix Sum -> Scatter
    void Rebuild(const float* posX, const float* posY, uint32_t activeCount, sf::FloatRect worldBounds);

    // Queries
    GridCell GetCell(int cellX, int cellY) const;
    void QueryArea(sf::FloatRect bounds, std::vector<uint32_t>& outEntityIndices) const;

    // Grid Dimensions
    float GetCellSize() const { return m_cellSize; }
    int GetGridWidth() const { return m_gridWidth; }
    int GetGridHeight() const { return m_gridHeight; }
    sf::FloatRect GetWorldBounds() const { return m_worldBounds; }

    const std::vector<uint32_t>& GetEntityIndices() const { return m_entityIndices; }
    const std::vector<GridCell>& GetCells() const { return m_cells; }

    // Converts (x, y) to grid cell coordinates
    void GetCellCoords(float x, float y, int& outCellX, int& outCellY) const;
    int GetCellIndex(int cellX, int cellY) const;

private:
    float m_cellSize = 48.0f;
    int m_gridWidth = 0;
    int m_gridHeight = 0;
    sf::FloatRect m_worldBounds;

    std::vector<uint32_t> m_cellCounts;
    std::vector<GridCell> m_cells;
    std::vector<uint32_t> m_entityIndices;
    std::vector<uint32_t> m_cellWriteOffsets;
};
