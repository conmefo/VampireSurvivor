#pragma once

#include "IAudioService.h"

class NullAudioService : public IAudioService
{
public:
    void PlaySfx(SfxID /*id*/, const PlaySoundOptions& /*options*/ = PlaySoundOptions()) override {}
    void PlaySfxAt(SfxID /*id*/, const sf::Vector2f& /*position*/, float /*volumeScale*/ = 1.0f, AudioPriority /*priority*/ = AudioPriority::Medium) override {}

    void PlayMusic(BgmID /*id*/, bool /*loop*/ = true, float /*fadeTimeSeconds*/ = 0.5f) override {}
    void StopMusic(float /*fadeTimeSeconds*/ = 0.5f) override {}
    void PauseMusic() override {}
    void ResumeMusic() override {}

    void Update(float /*dt*/, const sf::FloatRect& /*viewportBounds*/ = sf::FloatRect(), const sf::Vector2f& /*listenerPosition*/ = sf::Vector2f(0.f, 0.f)) override {}

    void SetMasterVolume(float /*volume*/) override {}
    void SetSfxVolume(float /*volume*/) override {}
    void SetMusicVolume(float /*volume*/) override {}

    float GetMasterVolume() const override { return 100.0f; }
    float GetSfxVolume() const override { return 100.0f; }
    float GetMusicVolume() const override { return 100.0f; }

    void SetMuted(bool /*muted*/) override {}
    bool IsMuted() const override { return false; }

    bool LoadSettings(const std::string& /*filepath*/) override { return false; }
    bool SaveSettings(const std::string& /*filepath*/) const override { return false; }

    void StopAllSfx() override {}
    void Cleanup() override {}
};
