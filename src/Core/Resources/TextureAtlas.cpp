#include "TextureAtlas.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

TextureAtlas::TextureAtlas(ResourceManager<sf::Texture, std::string>& textureManager)
    : m_textureManager(textureManager)
{
}

void TextureAtlas::LoadFromFile(const std::string& jsonPath, const std::string& textureId, unsigned int textureHeight)
{
    std::ifstream file(jsonPath);
    if (!file.is_open())
    {
        std::cerr << "TextureAtlas: Failed to open atlas JSON at " << jsonPath << "\n";
        return;
    }

    nlohmann::json atlasJson;
    try
    {
        file >> atlasJson;
    }
    catch (const nlohmann::json::parse_error& e)
    {
        std::cerr << "TextureAtlas: JSON parse error in " << jsonPath << ": " << e.what() << "\n";
        return;
    }

    for (auto it = atlasJson.begin(); it != atlasJson.end(); ++it)
    {
        const std::string& spriteName = it.key();
        const auto& rectData = it.value();

        if (rectData.contains("x") && rectData.contains("y") && 
            rectData.contains("width") && rectData.contains("height"))
        {
            int x = rectData["x"].get<int>();
            int y_source = rectData["y"].get<int>();
            int width = rectData["width"].get<int>();
            int height = rectData["height"].get<int>();

            // Perform geometric inversion: Y_SFML = H_texture - Y_source - H_sprite
            // This corrects Unity's bottom-left origin to SFML's top-left origin.
            int y_sfml = static_cast<int>(textureHeight) - y_source - height;

            m_regions[spriteName] = { textureId, sf::IntRect(x, y_sfml, width, height) };
        }
    }
}

AssetTextureData TextureAtlas::GetTextureData(const std::string& assetId) const
{
    // Modality A: Sliced Texture Atlas Resolution
    auto it = m_regions.find(assetId);
    if (it != m_regions.end())
    {
        const sf::Texture* atlasTex = m_textureManager.GetPtr(it->second.textureId);
        if (atlasTex)
        {
            return { atlasTex, it->second.rect };
        }
    }
    
    // Modality B: Monolithic Full-Texture Fallback
    const sf::Texture* standaloneTex = m_textureManager.GetPtr(assetId);
    if (standaloneTex)
    {
        sf::Vector2u size = standaloneTex->getSize();
        return { standaloneTex, sf::IntRect(0, 0, static_cast<int>(size.x), static_cast<int>(size.y)) };
    }
    
    std::cerr << "TextureAtlas: Asset '" << assetId << "' not found in slices or standalone pool!\n";
    return { nullptr, sf::IntRect() };
}

sf::Sprite TextureAtlas::CreateSprite(const std::string& assetId) const
{
    sf::Sprite sprite;
    AssetTextureData data = GetTextureData(assetId);
    
    if (data.texture)
    {
        sprite.setTexture(*data.texture);
        sprite.setTextureRect(data.rect);
    }
    
    return sprite;
}
