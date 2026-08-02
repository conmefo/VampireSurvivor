#pragma once

#include "Projectile.h"
#include "ProjectileManager.h"
#include "../Enemy/EnemyPool.h"

// Global Tuning Parameters (Controllable via Sliders / Tuning UI)
extern float g_LightningRingSpeed;        // Downward scroll speed (px/sec)
extern float g_LightningRingScaleX;       // Vertical beam length multiplier
extern float g_LightningRingScaleY;       // Horizontal squeezed beam thickness
extern float g_LightningRingRotation;     // Beam rotation angle (degrees)
extern float g_LightningRingStrikeRadius; // Impact AoE damage radius (px)
extern float g_LightningRingDuration;     // Visual stay duration on screen (sec)

class LightningRingProjectile : public Projectile
{
public:
    LightningRingProjectile(
        ProjectileManager* projManager,
        TextureAtlas* atlas,
        EnemyPool* enemyPool,
        const sf::Texture& texture,
        const sf::IntRect& textureRect,
        sf::Vector2f targetPosition,
        float duration,
        float power,
        float areaMultiplier,
        const std::string& hitVfxName
    );

    void Update(float dt) override;
    void Draw(sf::RenderTarget& target) const override;

private:
    void ExecuteStrike();

    ProjectileManager* m_projManager = nullptr;
    TextureAtlas* m_atlas = nullptr;
    EnemyPool* m_enemyPool = nullptr;

    sf::Vector2f m_targetPos;
    float m_areaMultiplier;
    
    float m_scrollOffset = 0.0f;
    float m_age = 0.0f;
    float m_speed = 3200.0f;

    bool m_hasStruck = false;
};
