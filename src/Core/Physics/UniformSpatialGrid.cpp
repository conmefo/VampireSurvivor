#include "UniformSpatialGrid.h"

#include <algorithm>
#include <cmath>

UniformSpatialGrid::UniformSpatialGrid(float cellSize)
    : m_cellSize(cellSize > 8.0f ? cellSize : 48.0f)
{
}

void UniformSpatialGrid::GetCellCoords(float x, float y, int& outCellX, int& outCellY) const
{
    outCellX = std::clamp(static_cast<int>(std::floor((x - m_worldBounds.left) / m_cellSize)), 0, m_gridWidth - 1);
    outCellY = std::clamp(static_cast<int>(std::floor((y - m_worldBounds.top) / m_cellSize)), 0, m_gridHeight - 1);
}

int UniformSpatialGrid::GetCellIndex(int cellX, int cellY) const
{
    if (cellX < 0 || cellX >= m_gridWidth || cellY < 0 || cellY >= m_gridHeight)
    {
        return -1;
    }
    return cellY * m_gridWidth + cellX;
}

GridCell UniformSpatialGrid::GetCell(int cellX, int cellY) const
{
    const int idx = GetCellIndex(cellX, cellY);
    if (idx < 0 || idx >= static_cast<int>(m_cells.size()))
    {
        return {0, 0};
    }
    return m_cells[idx];
}

void UniformSpatialGrid::Rebuild(const float* posX, const float* posY, uint32_t activeCount, sf::FloatRect worldBounds)
{
    m_worldBounds = worldBounds;
    m_gridWidth = std::max(1, static_cast<int>(std::ceil(m_worldBounds.width / m_cellSize)));
    m_gridHeight = std::max(1, static_cast<int>(std::ceil(m_worldBounds.height / m_cellSize)));

    const std::size_t totalCells = static_cast<std::size_t>(m_gridWidth * m_gridHeight);

    m_cellCounts.assign(totalCells, 0);
    m_cells.resize(totalCells);
    m_cellWriteOffsets.resize(totalCells);
    m_entityIndices.resize(activeCount);

    if (activeCount == 0)
    {
        for (std::size_t i = 0; i < totalCells; ++i)
        {
            m_cells[i] = {0, 0};
        }
        return;
    }

    // Pass 1: Count entities in each cell
    for (uint32_t i = 0; i < activeCount; ++i)
    {
        int cx, cy;
        GetCellCoords(posX[i], posY[i], cx, cy);
        const int cellIdx = GetCellIndex(cx, cy);
        if (cellIdx >= 0)
        {
            ++m_cellCounts[cellIdx];
        }
    }

    // Pass 2: Prefix sum to compute contiguous range for each cell
    uint32_t runningSum = 0;
    for (std::size_t i = 0; i < totalCells; ++i)
    {
        const uint32_t count = m_cellCounts[i];
        m_cells[i] = {runningSum, count};
        m_cellWriteOffsets[i] = runningSum;
        runningSum += count;
    }

    // Pass 3: Scatter entity indices into pre-allocated contiguous ranges
    for (uint32_t i = 0; i < activeCount; ++i)
    {
        int cx, cy;
        GetCellCoords(posX[i], posY[i], cx, cy);
        const int cellIdx = GetCellIndex(cx, cy);
        if (cellIdx >= 0)
        {
            const uint32_t targetIndex = m_cellWriteOffsets[cellIdx]++;
            m_entityIndices[targetIndex] = i;
        }
    }
}

void UniformSpatialGrid::QueryArea(sf::FloatRect bounds, std::vector<uint32_t>& outEntityIndices) const
{
    outEntityIndices.clear();
    if (m_gridWidth <= 0 || m_gridHeight <= 0 || m_cells.empty())
    {
        return;
    }

    int minCx, minCy, maxCx, maxCy;
    GetCellCoords(bounds.left, bounds.top, minCx, minCy);
    GetCellCoords(bounds.left + bounds.width, bounds.top + bounds.height, maxCx, maxCy);

    for (int cy = minCy; cy <= maxCy; ++cy)
    {
        for (int cx = minCx; cx <= maxCx; ++cx)
        {
            const GridCell cell = GetCell(cx, cy);
            const uint32_t end = cell.begin + cell.count;
            for (uint32_t i = cell.begin; i < end; ++i)
            {
                outEntityIndices.push_back(m_entityIndices[i]);
            }
        }
    }
}
