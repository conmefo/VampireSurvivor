#pragma once

#include "AudioIdentifiers.h"
#include <SFML/Audio/SoundBuffer.hpp>
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <random>

class SoundBufferCache
{
public:
    SoundBufferCache();
    ~SoundBufferCache() = default;

    // Load sound file(s) for a given SfxID.
    bool Load(SfxID id, const std::string& filepath);
    bool LoadVariations(SfxID id, const std::vector<std::string>& filepaths);

    // Default auto-loader for standard asset locations.
    void LoadDefaultCatalog(const std::string& assetsPath = "assets/audio/sfx/");

    // Get a sound buffer (random variation if multiple samples registered for SfxID).
    const sf::SoundBuffer* GetBuffer(SfxID id);

    bool HasBuffer(SfxID id) const;
    void Clear();

private:
    std::unordered_map<SfxID, std::vector<std::unique_ptr<sf::SoundBuffer>>> m_bufferMap;
    std::mt19937 m_rng;
};
