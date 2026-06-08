#pragma once

#include <vector>
#include "../Resources/TextureAtlas.h"

struct AnimationData
{
    std::vector<AssetTextureData> frames;
    float frameDuration = 0.1f;
    bool isLooping = true;
};
