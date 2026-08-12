#include "SoundBufferCache.h"
#include <iostream>

SoundBufferCache::SoundBufferCache()
    : m_rng(std::random_device{}())
{
}

bool SoundBufferCache::Load(SfxID id, const std::string& filepath)
{
    auto buffer = std::make_unique<sf::SoundBuffer>();
    if (!buffer->loadFromFile(filepath))
    {
        // Gracefully log warning without throwing exception so the game runs even if sound files are missing.
        std::cerr << "[SoundBufferCache] Notice: Could not load sound file: " << filepath << std::endl;
        return false;
    }

    m_bufferMap[id].push_back(std::move(buffer));
    return true;
}

bool SoundBufferCache::LoadVariations(SfxID id, const std::vector<std::string>& filepaths)
{
    bool loadedAny = false;
    for (const auto& path : filepaths)
    {
        if (Load(id, path))
        {
            loadedAny = true;
        }
    }
    return loadedAny;
}

void SoundBufferCache::LoadDefaultCatalog(const std::string& assetsPath)
{
    std::string base = assetsPath;
    if (!base.empty() && base.back() != '/' && base.back() != '\\')
    {
        base += "/";
    }

    // Attempt loading standard SFX paths (missing ones will be logged safely)
    Load(SfxID::ButtonClick, base + "ui_click.wav");
    Load(SfxID::ButtonHover, base + "ui_hover.wav");
    Load(SfxID::MenuBack, base + "ui_back.wav");
    Load(SfxID::LevelUpOpen, base + "sfx_levelup.wav");
    Load(SfxID::ChestOpen, base + "sfx_chest_open.wav");
    Load(SfxID::ChestReward, base + "sfx_chest_reward.wav");

    Load(SfxID::WhipAttack, base + "sfx_whip.wav");
    Load(SfxID::MagicMissileFire, base + "sfx_magic_missile.wav");
    Load(SfxID::FireballFire, base + "sfx_fireball.wav");
    Load(SfxID::KnifeFire, base + "sfx_knife.wav");
    Load(SfxID::AxeFire, base + "sfx_axe.wav");
    Load(SfxID::GarlicPulse, base + "sfx_garlic.wav");
    Load(SfxID::SantaWaterThrow, base + "sfx_santa_water.wav");
    Load(SfxID::LightningStrike, base + "sfx_lightning.wav");
    Load(SfxID::SongOfManaPulse, base + "sfx_song_of_mana.wav");

    // Multi-sample variations for high frequency events
    LoadVariations(SfxID::EnemyHit, { base + "sfx_hit_1.wav", base + "sfx_hit_2.wav", base + "sfx_hit_3.wav" });
    LoadVariations(SfxID::EnemyDeath, { base + "sfx_enemy_death_1.wav", base + "sfx_enemy_death_2.wav" });

    Load(SfxID::PlayerHit, base + "sfx_player_hit.wav");
    Load(SfxID::PlayerDeath, base + "sfx_player_death.wav");
    Load(SfxID::GemPickup, base + "sfx_gem_pickup.wav");
    Load(SfxID::GoldPickup, base + "sfx_coin_pickup.wav");
}

const sf::SoundBuffer* SoundBufferCache::GetBuffer(SfxID id)
{
    auto it = m_bufferMap.find(id);
    if (it == m_bufferMap.end() || it->second.empty())
    {
        return nullptr;
    }

    if (it->second.size() == 1)
    {
        return it->second[0].get();
    }

    // Pick random sample variation
    std::uniform_int_distribution<std::size_t> dist(0, it->second.size() - 1);
    std::size_t idx = dist(m_rng);
    return it->second[idx].get();
}

bool SoundBufferCache::HasBuffer(SfxID id) const
{
    auto it = m_bufferMap.find(id);
    return it != m_bufferMap.end() && !it->second.empty();
}

void SoundBufferCache::Clear()
{
    m_bufferMap.clear();
}
