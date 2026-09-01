#pragma once

#include "IAudioService.h"
#include "SoundBufferCache.h"
#include "SoundPool.h"
#include "BgmPlayer.h"
#include "SfxCoalescer.h"

class AudioManager : public IAudioService
{
public:
    AudioManager();
    ~AudioManager() override;

    // Initialize audio resources. Call after construction.
    void Initialize(const std::string& sfxAssetsPath = "assets/audio/sfx/",
                    const std::string& bgmAssetsPath = "assets/audio/bgm/");

    // --- IAudioService interface ---
    void PlaySfx(SfxID id, const PlaySoundOptions& options = PlaySoundOptions()) override;
    void PlaySfxAt(SfxID id, const sf::Vector2f& position, float volumeScale = 1.0f, AudioPriority priority = AudioPriority::Medium) override;

    void PlayMusic(BgmID id, bool loop = true, float fadeTimeSeconds = 0.5f) override;
    void StopMusic(float fadeTimeSeconds = 0.5f) override;
    void PauseMusic() override;
    void ResumeMusic() override;

    void Update(float dt, const sf::FloatRect& viewportBounds = sf::FloatRect(), const sf::Vector2f& listenerPosition = sf::Vector2f(0.f, 0.f)) override;

    void SetMasterVolume(float volume) override;
    void SetSfxVolume(float volume) override;
    void SetMusicVolume(float volume) override;

    float GetMasterVolume() const override;
    float GetSfxVolume() const override;
    float GetMusicVolume() const override;

    void SetMuted(bool muted) override;
    bool IsMuted() const override;

    bool LoadSettings(const std::string& filepath) override;
    bool SaveSettings(const std::string& filepath) const override;

    void StopAllSfx() override;
    void Cleanup() override;

    // Direct access to subsystems for advanced configuration
    SoundBufferCache& GetBufferCache() { return m_bufferCache; }
    BgmPlayer& GetBgmPlayer() { return m_bgmPlayer; }

private:
    float GetEffectiveSfxVolume() const;
    float GetEffectiveMusicVolume() const;

    SoundBufferCache m_bufferCache;
    SoundPool m_soundPool;
    BgmPlayer m_bgmPlayer;
    SfxCoalescer m_coalescer;

    float m_masterVolume = 100.0f;
    float m_sfxVolume = 100.0f;
    float m_musicVolume = 100.0f;
    bool m_isMuted = false;

    float m_elapsedTime = 0.0f; // Monotonic time for SoundPool LRU tracking
};
