#pragma once

#include "TileMap.h"
#include <unordered_map>
#include <memory>
#include <string>

class TileMapManager
{
public:
    TileMapManager() = default;
    ~TileMapManager() = default;

    // Prevent copying
    TileMapManager(const TileMapManager&) = delete;
    TileMapManager& operator=(const TileMapManager&) = delete;

    TileMap* GetMap(int stageId);
    void SetMap(int stageId, std::unique_ptr<TileMap> map);
    void ClearMap(int stageId);
    void ClearAll();

private:
    std::unordered_map<int, std::unique_ptr<TileMap>> m_maps;
};
