#pragma once

#include <SFML/System/Vector2.hpp>
#include <string>

enum class SfxID
{
    None,
    
    // UI SFX
    ButtonClick,
    ButtonHover,
    MenuBack,
    LevelUpOpen,
    PowerUpBuy,
    ChestOpen,
    ChestReward,
    
    // Weapon Firing SFX
    WhipAttack,
    MagicMissileFire,
    FireballFire,
    KnifeFire,
    AxeFire,
    GarlicPulse,
    SantaWaterThrow,
    SantaWaterZone,
    LightningStrike,
    SongOfManaPulse,
    
    // Gameplay & Combat SFX
    EnemyHit,
    EnemyDeath,
    PlayerHit,
    PlayerDeath,
    GemPickup,
    GoldPickup,
    ItemPickup
};

enum class BgmID
{
    None,
    MainMenu,
    StageForest,
    StageLibrary,
    GameOver,
    Victory
};

enum class AudioPriority
{
    Low = 0,     // High frequency enemy hits, ambient sounds
    Medium = 1,  // Enemy deaths, regular UI clicks, pickup sounds
    High = 2,    // Weapon attacks, player taking damage, boss spawns
    Critical = 3 // Level up, player death, chest reward (Never evicted)
};

struct PlaySoundOptions
{
    sf::Vector2f position = {0.0f, 0.0f};
    float volumeScale = 1.0f;
    float pitchJitter = 0.05f; // ±5% pitch jitter
    AudioPriority priority = AudioPriority::Medium;
    bool isSpatial = false;
    float minDistance = 100.0f;
    float attenuation = 1.0f;
};
