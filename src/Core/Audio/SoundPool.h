#pragma once

#include "AudioIdentifiers.h"
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <vector>
#include <random>

// A single managed voice channel in the pool.
struct VoiceChannel
{
    sf::Sound sound;
    AudioPriority priority = AudioPriority::Low;
    float startTime = 0.0f; // Monotonic timestamp for LRU eviction ordering
};

class SoundPool
{
public:
    explicit SoundPool(std::size_t poolSize = 64);
    ~SoundPool() = default;

    // Play a sound buffer with given options. Returns true if a channel was acquired.
    bool Play(const sf::SoundBuffer& buffer, const PlaySoundOptions& options, float currentTime);

    // Stop all active sounds and reset channels.
    void StopAll();

    // Returns number of currently playing channels.
    std::size_t GetActiveChannelCount() const;

    std::size_t GetPoolSize() const { return m_channels.size(); }

private:
    // Find a free (stopped) channel, or steal one via priority-based LRU.
    VoiceChannel* AcquireChannel(AudioPriority incomingPriority, float currentTime);

    std::vector<VoiceChannel> m_channels;
    std::mt19937 m_rng;
};
