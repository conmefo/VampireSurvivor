#include "AudioManager.h"
#include <SFML/Audio/Listener.hpp>
#include <algorithm>
#include <iostream>

AudioManager::AudioManager()
    : m_soundPool(64)
{
}

AudioManager::~AudioManager()
{
    Cleanup();
}

void AudioManager::Initialize(const std::string& sfxAssetsPath, const std::string& bgmAssetsPath)
{
    // Load SFX buffers from standard catalog paths
    m_bufferCache.LoadDefaultCatalog(sfxAssetsPath);

    // Register BGM tracks
    std::string bgmBase = bgmAssetsPath;
    if (!bgmBase.empty() && bgmBase.back() != '/' && bgmBase.back() != '\\')
    {
        bgmBase += "/";
    }

    m_bgmPlayer.RegisterTrack(BgmID::MainMenu, bgmBase + "VS_Music_v04-01.ogg");
    m_bgmPlayer.RegisterTrack(BgmID::StageForest, bgmBase + "bgm_elrond_forest.ogg");
    m_bgmPlayer.RegisterTrack(BgmID::StageLibrary, bgmBase + "bgm_library.ogg");
    m_bgmPlayer.RegisterTrack(BgmID::GameOver, bgmBase + "bgm_TheEndMain.ogg");
    m_bgmPlayer.RegisterTrack(BgmID::Victory, bgmBase + "bgm_TheEndIntro.ogg");

    std::cout << "[AudioManager] Initialized. SFX pool: " << m_soundPool.GetPoolSize()
              << " channels." << std::endl;
}

void AudioManager::PlaySfx(SfxID id, const PlaySoundOptions& options)
{
    if (m_isMuted || id == SfxID::None)
    {
        return;
    }

    // Scale volume by effective SFX volume
    PlaySoundOptions adjusted = options;
    adjusted.volumeScale *= GetEffectiveSfxVolume() / 100.0f;

    // Submit to coalescer for frame-level batching
    m_coalescer.Submit(id, adjusted);
}

void AudioManager::PlaySfxAt(SfxID id, const sf::Vector2f& position, float volumeScale, AudioPriority priority)
{
    PlaySoundOptions options;
    options.position = position;
    options.volumeScale = volumeScale;
    options.priority = priority;
    options.isSpatial = true;
    PlaySfx(id, options);
}

void AudioManager::PlayMusic(BgmID id, bool loop, float fadeTimeSeconds)
{
    if (m_isMuted)
    {
        return;
    }

    m_bgmPlayer.SetVolume(GetEffectiveMusicVolume());
    m_bgmPlayer.Play(id, loop, fadeTimeSeconds);
}

void AudioManager::StopMusic(float fadeTimeSeconds)
{
    m_bgmPlayer.Stop(fadeTimeSeconds);
}

void AudioManager::PauseMusic()
{
    m_bgmPlayer.Pause();
}

void AudioManager::ResumeMusic()
{
    m_bgmPlayer.Resume();
}

void AudioManager::Update(float dt, const sf::FloatRect& viewportBounds, const sf::Vector2f& listenerPosition)
{
    m_elapsedTime += dt;

    // Update SFML 3D listener position (flattened to 2D)
    sf::Listener::setPosition(listenerPosition.x, listenerPosition.y, 0.0f);

    // Update viewport for culling
    m_coalescer.SetViewportBounds(viewportBounds);

    // Flush coalesced SFX requests and play them
    auto requests = m_coalescer.Flush();
    for (const auto& req : requests)
    {
        const sf::SoundBuffer* buffer = m_bufferCache.GetBuffer(req.id);
        if (buffer)
        {
            m_soundPool.Play(*buffer, req.options, m_elapsedTime);
        }
    }

    // Update BGM fades
    m_bgmPlayer.Update(dt);
}

void AudioManager::SetMasterVolume(float volume)
{
    m_masterVolume = std::clamp(volume, 0.0f, 100.0f);
    // Propagate to BGM player immediately
    m_bgmPlayer.SetVolume(GetEffectiveMusicVolume());
}

void AudioManager::SetSfxVolume(float volume)
{
    m_sfxVolume = std::clamp(volume, 0.0f, 100.0f);
}

void AudioManager::SetMusicVolume(float volume)
{
    m_musicVolume = std::clamp(volume, 0.0f, 100.0f);
    m_bgmPlayer.SetVolume(GetEffectiveMusicVolume());
}

float AudioManager::GetMasterVolume() const
{
    return m_masterVolume;
}

float AudioManager::GetSfxVolume() const
{
    return m_sfxVolume;
}

float AudioManager::GetMusicVolume() const
{
    return m_musicVolume;
}

void AudioManager::SetMuted(bool muted)
{
    m_isMuted = muted;
    if (muted)
    {
        m_bgmPlayer.Pause();
        StopAllSfx();
    }
    else
    {
        m_bgmPlayer.Resume();
    }
}

bool AudioManager::IsMuted() const
{
    return m_isMuted;
}

void AudioManager::StopAllSfx()
{
    m_soundPool.StopAll();
}

void AudioManager::Cleanup()
{
    StopAllSfx();
    m_bgmPlayer.Stop(0.0f);
    m_bufferCache.Clear();
}

float AudioManager::GetEffectiveSfxVolume() const
{
    return (m_masterVolume / 100.0f) * m_sfxVolume;
}

float AudioManager::GetEffectiveMusicVolume() const
{
    return (m_masterVolume / 100.0f) * m_musicVolume;
}
