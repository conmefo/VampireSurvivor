#include "BgmPlayer.h"
#include <algorithm>
#include <iostream>

BgmPlayer::BgmPlayer() = default;

BgmPlayer::~BgmPlayer()
{
    m_music.stop();
}

void BgmPlayer::RegisterTrack(BgmID id, const std::string& filepath)
{
    m_trackPaths[id] = filepath;
}

void BgmPlayer::Play(BgmID id, bool loop, float fadeInSeconds)
{
    if (id == BgmID::None)
    {
        Stop(fadeInSeconds);
        return;
    }

    // Already playing this track
    if (id == m_currentTrackId && IsPlaying())
    {
        return;
    }

    auto it = m_trackPaths.find(id);
    if (it == m_trackPaths.end())
    {
        std::cerr << "[BgmPlayer] No track registered for BgmID " << static_cast<int>(id) << std::endl;
        return;
    }

    // If something is currently playing, crossfade
    if (IsPlaying() && fadeInSeconds > 0.0f)
    {
        m_pendingTrackId = id;
        m_pendingLoop = loop;
        m_fadeState = FadeState::FadingOutThenIn;
        m_fadeDuration = fadeInSeconds;
        m_fadeProgress = 0.0f;
        return;
    }

    // Direct play (nothing playing or no fade requested)
    if (!m_music.openFromFile(it->second))
    {
        std::cerr << "[BgmPlayer] Failed to open music file: " << it->second << std::endl;
        return;
    }

    m_currentTrackId = id;
    m_music.setLoop(loop);

    if (fadeInSeconds > 0.0f)
    {
        m_music.setVolume(0.0f);
        m_fadeState = FadeState::FadingIn;
        m_fadeDuration = fadeInSeconds;
        m_fadeProgress = 0.0f;
    }
    else
    {
        m_music.setVolume(m_baseVolume);
        m_fadeState = FadeState::None;
    }

    m_music.play();
}

void BgmPlayer::Stop(float fadeOutSeconds)
{
    if (!IsPlaying())
    {
        m_currentTrackId = BgmID::None;
        return;
    }

    if (fadeOutSeconds > 0.0f)
    {
        m_fadeState = FadeState::FadingOut;
        m_fadeDuration = fadeOutSeconds;
        m_fadeProgress = 0.0f;
        m_pendingTrackId = BgmID::None;
    }
    else
    {
        m_music.stop();
        m_currentTrackId = BgmID::None;
        m_fadeState = FadeState::None;
    }
}

void BgmPlayer::Pause()
{
    if (m_music.getStatus() == sf::Music::Playing)
    {
        m_music.pause();
    }
}

void BgmPlayer::Resume()
{
    if (m_music.getStatus() == sf::Music::Paused)
    {
        m_music.play();
    }
}

void BgmPlayer::Update(float dt)
{
    if (m_fadeState == FadeState::None)
    {
        return;
    }

    m_fadeProgress += dt;
    float t = std::clamp(m_fadeProgress / m_fadeDuration, 0.0f, 1.0f);

    switch (m_fadeState)
    {
        case FadeState::FadingIn:
        {
            m_music.setVolume(t * m_baseVolume);
            if (t >= 1.0f)
            {
                m_music.setVolume(m_baseVolume);
                m_fadeState = FadeState::None;
            }
            break;
        }
        case FadeState::FadingOut:
        {
            m_music.setVolume((1.0f - t) * m_baseVolume);
            if (t >= 1.0f)
            {
                m_music.stop();
                m_currentTrackId = BgmID::None;
                m_fadeState = FadeState::None;
            }
            break;
        }
        case FadeState::FadingOutThenIn:
        {
            m_music.setVolume((1.0f - t) * m_baseVolume);
            if (t >= 1.0f)
            {
                m_music.stop();
                // Now start the pending track with a fresh fade-in
                m_currentTrackId = BgmID::None;
                m_fadeState = FadeState::None;
                Play(m_pendingTrackId, m_pendingLoop, m_fadeDuration);
                m_pendingTrackId = BgmID::None;
            }
            break;
        }
        default:
            break;
    }
}

void BgmPlayer::SetVolume(float volume)
{
    m_baseVolume = std::clamp(volume, 0.0f, 100.0f);
    if (m_fadeState == FadeState::None && IsPlaying())
    {
        m_music.setVolume(m_baseVolume);
    }
}

float BgmPlayer::GetVolume() const
{
    return m_baseVolume;
}

bool BgmPlayer::IsPlaying() const
{
    return m_music.getStatus() == sf::Music::Playing ||
           m_music.getStatus() == sf::Music::Paused;
}
