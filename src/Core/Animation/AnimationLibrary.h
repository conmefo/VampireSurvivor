#pragma once

#include "AnimationData.h"
#include <string>
#include <vector>
#include <unordered_map>

class TextureAtlas;

class AnimationLibrary
{
public:
    AnimationLibrary(const TextureAtlas& atlas);
    ~AnimationLibrary() = default;

    // Load definitions from JSON configuration
    void LoadFromJson(const std::string& filepath);

    // Register animations programmatically or from code
    void RegisterAnimation(const std::string& species, const std::string& state, const AnimationData& animData);
    
    // Create an AnimationData directly from a list of atlas frame keys
    AnimationData CreateFromAtlasFrames(
        const std::vector<std::string>& frameKeys,
        float frameDuration = 0.1f,
        bool isLooping = true) const;

    // Query animations
    const AnimationData* GetAnimation(const std::string& species, const std::string& state) const;
    bool HasAnimation(const std::string& species, const std::string& state) const;

private:
    const TextureAtlas& m_atlas;
    std::unordered_map<std::string, std::unordered_map<std::string, AnimationData>> m_library;
};
