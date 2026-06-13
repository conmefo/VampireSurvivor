#include "TileMapManager.h"

TileMap* TileMapManager::GetMap(int stageId)
{
    auto it = m_maps.find(stageId);
    if(it != m_maps.end())
    {
        return it->second.get();
    }
    return nullptr;
}

void TileMapManager::SetMap(int stageId, std::unique_ptr<TileMap> map)
{
    m_maps[stageId] = std::move(map);
}

void TileMapManager::ClearMap(int stageId)
{
    m_maps.erase(stageId);
}

void TileMapManager::ClearAll()
{
    m_maps.clear();
}
