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

    // Vampire Survivors clips extracted from the supplied game asset bundle.
    Load(SfxID::ButtonClick, base + "sfx_bell2.ogg");
    Load(SfxID::ButtonHover, base + "sfx_littlepickup.wav");
    Load(SfxID::MenuBack, base + "sfx_sounds_pause7_out.wav");
    Load(SfxID::LevelUpOpen, base + "sfx_levelup.wav");
    Load(SfxID::ChestOpen, base + "sfx_treasure_found.wav");
    Load(SfxID::ChestReward, base + "sfx_treasure_foundB.wav");

    Load(SfxID::WhipAttack, base + "sfx_strike.wav");
    LoadVariations(SfxID::MagicMissileFire, { base + "sfx_projectile_magic.wav", base + "sfx_projectile_magic2.wav", base + "sfx_projectile_magic4.wav" });
    Load(SfxID::FireballFire, base + "sfx_explo_soft.wav");
    Load(SfxID::KnifeFire, base + "sfx_javelin.wav");
    Load(SfxID::AxeFire, base + "sfx_javelin2.wav");
    Load(SfxID::GarlicPulse, base + "sfx_mwind1.wav");
    Load(SfxID::SantaWaterThrow, base + "sfx_bubble.wav");
    Load(SfxID::LightningStrike, base + "sfx_mlightning1.wav");
    Load(SfxID::SongOfManaPulse, base + "sfx_magic_chanting.wav");

    // Multi-sample variations for high frequency events
    LoadVariations(SfxID::EnemyHit, { base + "sfx_enemyHit.wav", base + "sfx_sounds_impact9.wav", base + "sfx_sounds_impact15_2.wav" });
    LoadVariations(SfxID::EnemyDeath, { base + "sfx_death_4.wav", base + "sfx_potBreak.wav" });

    Load(SfxID::PlayerHit, base + "sfx_impact.wav");
    Load(SfxID::PlayerDeath, base + "sfx_deathscream.wav");
    Load(SfxID::GemPickup, base + "sfx_gem.wav");
    Load(SfxID::GoldPickup, base + "sfx_coin_double4.wav");
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
