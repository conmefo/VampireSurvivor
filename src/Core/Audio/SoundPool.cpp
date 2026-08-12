#include "SoundPool.h"
#include <algorithm>

SoundPool::SoundPool(std::size_t poolSize)
    : m_rng(std::random_device{}())
{
    m_channels.resize(poolSize);
}

bool SoundPool::Play(const sf::SoundBuffer& buffer, const PlaySoundOptions& options, float currentTime)
{
    VoiceChannel* channel = AcquireChannel(options.priority, currentTime);
    if (!channel)
    {
        return false; // All channels occupied by higher-priority sounds
    }

    channel->sound.stop();
    channel->sound.setBuffer(buffer);
    channel->priority = options.priority;
    channel->startTime = currentTime;

    // Apply volume scale (0.0 - 100.0 range for SFML)
    float volume = std::clamp(options.volumeScale * 100.0f, 0.0f, 100.0f);
    channel->sound.setVolume(volume);

    // Apply pitch jitter
    if (options.pitchJitter > 0.0f)
    {
        std::uniform_real_distribution<float> dist(1.0f - options.pitchJitter, 1.0f + options.pitchJitter);
        channel->sound.setPitch(dist(m_rng));
    }
    else
    {
        channel->sound.setPitch(1.0f);
    }

    // Apply 2D spatial positioning (SFML uses 3D; we flatten Z to 0)
    if (options.isSpatial)
    {
        channel->sound.setRelativeToListener(false);
        channel->sound.setPosition(options.position.x, options.position.y, 0.0f);
        channel->sound.setMinDistance(options.minDistance);
        channel->sound.setAttenuation(options.attenuation);
    }
    else
    {
        // Non-spatial: play relative to listener at origin (centered, full volume)
        channel->sound.setRelativeToListener(true);
        channel->sound.setPosition(0.0f, 0.0f, 0.0f);
        channel->sound.setMinDistance(1.0f);
        channel->sound.setAttenuation(0.0f);
    }

    channel->sound.play();
    return true;
}

void SoundPool::StopAll()
{
    for (auto& ch : m_channels)
    {
        ch.sound.stop();
        ch.priority = AudioPriority::Low;
        ch.startTime = 0.0f;
    }
}

std::size_t SoundPool::GetActiveChannelCount() const
{
    std::size_t count = 0;
    for (const auto& ch : m_channels)
    {
        if (ch.sound.getStatus() == sf::Sound::Playing)
        {
            ++count;
        }
    }
    return count;
}

VoiceChannel* SoundPool::AcquireChannel(AudioPriority incomingPriority, float currentTime)
{
    // Pass 1: Find a free (stopped) channel
    for (auto& ch : m_channels)
    {
        if (ch.sound.getStatus() == sf::Sound::Stopped)
        {
            return &ch;
        }
    }

    // Pass 2: Voice stealing - find the oldest channel with priority <= incoming
    // Critical sounds are never stolen.
    VoiceChannel* candidate = nullptr;
    float oldestTime = currentTime + 1.0f; // Start higher than any possible startTime

    for (auto& ch : m_channels)
    {
        // Don't steal Critical priority channels
        if (ch.priority == AudioPriority::Critical)
        {
            continue;
        }

        // Only steal channels with priority <= incoming request
        if (static_cast<int>(ch.priority) <= static_cast<int>(incomingPriority))
        {
            if (ch.startTime < oldestTime)
            {
                oldestTime = ch.startTime;
                candidate = &ch;
            }
        }
    }

    return candidate; // May be nullptr if all channels are Critical or higher priority
}
