#pragma once

#include "AudioIdentifiers.h"
#include <SFML/Graphics/Rect.hpp>
#include <string>

class IAudioService
{
public:
    virtual ~IAudioService() = default;

    // SFX Playback
    virtual void PlaySfx(SfxID id, const PlaySoundOptions& options = PlaySoundOptions()) = 0;
    virtual void PlaySfxAt(SfxID id, const sf::Vector2f& position, float volumeScale = 1.0f, AudioPriority priority = AudioPriority::Medium) = 0;

    // Music Playback
    virtual void PlayMusic(BgmID id, bool loop = true, float fadeTimeSeconds = 0.5f) = 0;
    virtual void StopMusic(float fadeTimeSeconds = 0.5f) = 0;
    virtual void PauseMusic() = 0;
    virtual void ResumeMusic() = 0;

    // Audio Frame Update & Listener / Viewport Update
    virtual void Update(float dt, const sf::FloatRect& viewportBounds = sf::FloatRect(), const sf::Vector2f& listenerPosition = sf::Vector2f(0.f, 0.f)) = 0;

    // Volume & Settings Control (0.0 to 100.0)
    virtual void SetMasterVolume(float volume) = 0;
    virtual void SetSfxVolume(float volume) = 0;
    virtual void SetMusicVolume(float volume) = 0;

    virtual float GetMasterVolume() const = 0;
    virtual float GetSfxVolume() const = 0;
    virtual float GetMusicVolume() const = 0;

    virtual void SetMuted(bool muted) = 0;
    virtual bool IsMuted() const = 0;

    // Persist user-facing mixer settings between sessions.
    virtual bool LoadSettings(const std::string& filepath) = 0;
    virtual bool SaveSettings(const std::string& filepath) const = 0;

    // Lifecycle & Clean Shutdown
    virtual void StopAllSfx() = 0;
    virtual void Cleanup() = 0;
};
