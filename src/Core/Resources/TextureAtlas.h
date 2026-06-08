#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>

#include "ResourceManager.h"

struct AssetTextureData
{
    const sf::Texture* texture;
    sf::IntRect rect;
};

struct AtlasRegion
{
    std::string textureId;
    sf::IntRect rect;
};

class TextureAtlas
{
public:
    TextureAtlas(ResourceManager<sf::Texture, std::string>& textureManager);
    ~TextureAtlas() = default;

    // Loads the JSON mapping and performs origin transformation
    void LoadFromFile(const std::string& jsonPath, const std::string& textureId, unsigned int textureHeight);

    // Retrieves the unified asset data (Texture + Viewport Rect)
    AssetTextureData GetTextureData(const std::string& assetId) const;

    // Factory method for standalone sprites
    sf::Sprite CreateSprite(const std::string& assetId) const;

private:
    ResourceManager<sf::Texture, std::string>& m_textureManager;
    std::unordered_map<std::string, AtlasRegion> m_regions;
};
