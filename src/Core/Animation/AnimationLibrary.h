#pragma once

#include "AnimationData.h"
#include <string>
#include <unordered_map>

class TextureAtlas;

class AnimationLibrary
{
public:
    AnimationLibrary(const TextureAtlas& atlas);
    ~AnimationLibrary() = default;

    void LoadFromJson(const std::string& filepath);
    const AnimationData* GetAnimation(const std::string& species, const std::string& state) const;

private:
    const TextureAtlas& m_atlas;
    std::unordered_map<std::string, std::unordered_map<std::string, AnimationData>> m_library;
};
