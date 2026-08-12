#pragma once

#include "AudioIdentifiers.h"
#include <SFML/Audio/Music.hpp>
#include <string>
#include <unordered_map>

class BgmPlayer
{
public:
    BgmPlayer();
    ~BgmPlayer();

    // Register BGM file path for a given BgmID.
    void RegisterTrack(BgmID id, const std::string& filepath);

    // Play a BGM track with optional crossfade.
    void Play(BgmID id, bool loop = true, float fadeInSeconds = 0.5f);

    // Stop current music with optional fade-out.
    void Stop(float fadeOutSeconds = 0.5f);

    void Pause();
    void Resume();

    // Must be called every frame for fade transitions.
    void Update(float dt);

    // Volume control (0.0 - 100.0)
    void SetVolume(float volume);
    float GetVolume() const;

    BgmID GetCurrentTrackId() const { return m_currentTrackId; }
    bool IsPlaying() const;

private:
    enum class FadeState
    {
        None,
        FadingIn,
        FadingOut,
        FadingOutThenIn // Crossfade: fade out current, then fade in pending
    };

    sf::Music m_music;
    std::unordered_map<BgmID, std::string> m_trackPaths;

    BgmID m_currentTrackId = BgmID::None;
    BgmID m_pendingTrackId = BgmID::None;
    bool m_pendingLoop = true;

    float m_baseVolume = 100.0f;
    float m_fadeProgress = 0.0f;
    float m_fadeDuration = 0.0f;
    FadeState m_fadeState = FadeState::None;
};
