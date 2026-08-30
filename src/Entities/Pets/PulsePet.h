#pragma once

#include "../../Core/Resources/TextureAtlas.h"
#include "../Player.h"
#include "../Enemy/EnemyPool.h"

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

struct PulsePetConfig
{
    // --- Pet Film Projector Animation & Visuals ---
    float frameDuration = 0.10f;           // Delay of film projector (seconds per frame)
    float petScale = 1.10f;                 // Base scale of the pet sprite

    // --- Orbit Movement ---
    float orbitRadius = 40.0f;             // Orbit distance from player center (pixels)
    float orbitSpeed = 0.8f;               // Speed of orbit rotation (radians per second)

    // --- Ability Timing ---
    float baseCooldown = 6.0f;             // Base cooldown between shockwaves at Level 1 (seconds)
    float cooldownPerLevel = 1.2f;          // Cooldown reduction per additional level rank

    // --- Shockwave Ring Visuals ---
    float targetRingScale = 6.5f;          // Scale at which knockback triggers when scaleUpDuration ends
    float fadeEndRingScale = 9.0f;         // Visual scale at end of fade-out phase (continues scaling up)
    float scaleUpDuration = 0.23f;         // Fast scale-up duration from 0 to targetRingScale (seconds)
    float fadeOutDuration = 0.08f;         // Fade-out duration from peak opacity down to 0 (seconds)
    uint8_t ringMaxOpacity = 53;          // Max alpha/opacity of rings3 sprite (0-255)

    // --- Shockwave Mechanics ---
    float damage = 10.0f;                  // Damage dealt to enemies inside ring at peak moment (8-12 HP)
    float knockbackForce = 400.0f;         // Push force applied to enemies inside ring at peak moment
};

class DamageNumberManager;

class PulsePet
{
public:
    PulsePet(TextureAtlas& atlas, int level = 1, PulsePetConfig config = PulsePetConfig());
    ~PulsePet() = default;

    void Update(float dt, const Player& player, EnemyPool& enemyPool, DamageNumberManager* damageNumbers = nullptr);
    void Draw(sf::RenderTarget& target);

    // Getters / Setters for live tweaking
    PulsePetConfig& GetConfig() { return m_config; }
    const PulsePetConfig& GetConfig() const { return m_config; }
    void SetConfig(const PulsePetConfig& config) { m_config = config; }
    void SetLevel(int level) { m_level = level; }

private:
    enum class ShockwaveState
    {
        Ready,
        Expanding,
        Fading
    };

    TextureAtlas& m_atlas;
    int m_level = 1;
    PulsePetConfig m_config;

    // Orbit state
    float m_orbitAngle = 0.0f;
    sf::Vector2f m_position{0.0f, 0.0f};
    bool m_facingLeft = false;

    // Film projector animation
    std::vector<std::string> m_frameKeys;
    std::size_t m_currentFrameIndex = 0;
    float m_frameTimer = 0.0f;
    sf::Sprite m_petSprite;

    // Shockwave effect state
    ShockwaveState m_shockwaveState = ShockwaveState::Ready;
    float m_cooldownTimer = 0.0f;
    float m_ringTimer = 0.0f;
    sf::Vector2f m_shockwaveCenter{0.0f, 0.0f};
    sf::Sprite m_ringSprite;
    AssetTextureData m_ringTextureData{nullptr, sf::IntRect()};

    void TriggerShockwave(EnemyPool& enemyPool, DamageNumberManager* damageNumbers);
};
