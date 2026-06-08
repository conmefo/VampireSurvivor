#include "AnimationLibrary.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

AnimationLibrary::AnimationLibrary(const TextureAtlas& atlas)
    : m_atlas(atlas)
{
}

void AnimationLibrary::LoadFromJson(const std::string& filepath)
{
    std::ifstream file(filepath);
    if(!file.is_open())
    {
        std::cerr << "AnimationLibrary: Failed to open " << filepath << "\n";
        return;
    }

    json j;
    try
    {
        file >> j;
    }
    catch(const json::parse_error& e)
    {
        std::cerr << "AnimationLibrary: Parse error in " << filepath << ": " << e.what() << "\n";
        return;
    }

    for(auto it = j.begin(); it != j.end(); ++it)
    {
        std::string species = it.key();
        auto statesJson = it.value();

        for(auto stateIt = statesJson.begin(); stateIt != statesJson.end(); ++stateIt)
        {
            std::string state = stateIt.key();
            auto dataJson = stateIt.value();

            AnimationData animData;
            
            if(dataJson.contains("frameDuration"))
            {
                animData.frameDuration = dataJson["frameDuration"].get<float>();
            }
            
            if(dataJson.contains("isLooping"))
            {
                animData.isLooping = dataJson["isLooping"].get<bool>();
            }

            if(dataJson.contains("frames"))
            {
                for(const auto& frameKey : dataJson["frames"])
                {
                    std::string key = frameKey.get<std::string>();
                    AssetTextureData textureData = m_atlas.GetTextureData(key);
                    
                    if(!textureData.texture)
                    {
                        std::cerr << "AnimationLibrary: Missing frame key [" << key << "] in atlas.\n";
                        AssetTextureData emptyData;
                        emptyData.texture = nullptr;
                        emptyData.rect = sf::IntRect(0, 0, 0, 0);
                        animData.frames.push_back(emptyData);
                    }
                    else
                    {
                        animData.frames.push_back(textureData);
                    }
                }
            }

            m_library[species][state] = animData;
        }
    }
}

const AnimationData* AnimationLibrary::GetAnimation(const std::string& species, const std::string& state) const
{
    auto speciesIt = m_library.find(species);
    if(speciesIt != m_library.end())
    {
        auto stateIt = speciesIt->second.find(state);
        if(stateIt != speciesIt->second.end())
        {
            return &stateIt->second;
        }
    }
    
    std::cerr << "AnimationLibrary: Animation not found [" << species << "][" << state << "]\n";
    return nullptr;
}
