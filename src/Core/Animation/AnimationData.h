#pragma once

#include <vector>
#include <string>
#include "../Resources/TextureAtlas.h"

struct AnimationData
{
    std::string name;
    std::vector<AssetTextureData> frames;
    float frameDuration = 0.1f;
    bool isLooping = true;

    size_t GetFrameCount() const { return frames.size(); }
    float GetTotalDuration() const { return static_cast<float>(frames.size()) * frameDuration; }
    bool IsEmpty() const { return frames.empty(); }
};
