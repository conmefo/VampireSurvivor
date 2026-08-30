#include "PulsePet.h"
#include "../DamageNumberManager.h"
#include "../Pickups/ExperienceGemManager.h"
#include <cmath>
#include <algorithm>
#include <iostream>

PulsePet::PulsePet(TextureAtlas& atlas, int level, PulsePetConfig config)
    : m_atlas(atlas), m_level(level), m_config(config)
{
    m_frameKeys = {
        "tinyC01Crewmate_01",
        "tinyC01Crewmate_02",
        "tinyC01Crewmate_03",
        "tinyC01Crewmate_04"
    };

    // Initialize Shockwave Ring Texture from Atlas
    m_ringTextureData = m_atlas.GetTextureData("Rings3");
    if (m_ringTextureData.texture)
    {
        m_ringSprite.setTexture(*m_ringTextureData.texture);
        m_ringSprite.setTextureRect(m_ringTextureData.rect);
        m_ringSprite.setOrigin(
            static_cast<float>(m_ringTextureData.rect.width) / 2.0f,
            static_cast<float>(m_ringTextureData.rect.height) / 2.0f
        );
    }
}

void PulsePet::Update(float dt, const Player& player, EnemyPool& enemyPool, DamageNumberManager* damageNumbers)
{
    // --- 1. Film Projector Frame Animation ---
    m_frameTimer += dt;
    if (m_frameTimer >= m_config.frameDuration)
    {
        m_frameTimer -= m_config.frameDuration;
        m_currentFrameIndex = (m_currentFrameIndex + 1) % m_frameKeys.size();
    }

    AssetTextureData frameData = m_atlas.GetTextureData(m_frameKeys[m_currentFrameIndex]);
    if (frameData.texture)
    {
        m_petSprite.setTexture(*frameData.texture);
        m_petSprite.setTextureRect(frameData.rect);
        // Set origin to bottom center of sprite as requested
        m_petSprite.setOrigin(
            static_cast<float>(frameData.rect.width) / 2.0f,
            static_cast<float>(frameData.rect.height)
        );
    }

    // --- 2. Orbit Movement around Player ---
    m_orbitAngle += m_config.orbitSpeed * dt;
    constexpr float TWO_PI = 6.28318530718f;
    if (m_orbitAngle >= TWO_PI)
    {
        m_orbitAngle -= TWO_PI;
    }

    sf::Vector2f playerCenter = player.GetCenterPosition();
    m_position = playerCenter + sf::Vector2f(
        std::cos(m_orbitAngle) * m_config.orbitRadius,
        std::sin(m_orbitAngle) * m_config.orbitRadius
    );
    m_petSprite.setPosition(m_position);

    // --- 3. Direction Flipping based on Player Facing ---
    sf::Vector2f facing = player.GetFacingDirection();
    if (facing.x < -0.01f)
    {
        m_facingLeft = true;
    }
    else if (facing.x > 0.01f)
    {
        m_facingLeft = false;
    }

    float scaleX = m_facingLeft ? -m_config.petScale : m_config.petScale;
    float scaleY = m_config.petScale;
    m_petSprite.setScale(scaleX, scaleY);

    // --- 4. Shockwave Ability Cooldown & Lifecycle State Machine ---
    float effectiveCooldown = std::max(0.5f, m_config.baseCooldown - (m_level - 1) * m_config.cooldownPerLevel);

    if (m_shockwaveState == ShockwaveState::Ready)
    {
        m_cooldownTimer += dt;
        if (m_cooldownTimer >= effectiveCooldown)
        {
            m_cooldownTimer = 0.0f;
            m_shockwaveState = ShockwaveState::Expanding;
            m_ringTimer = 0.0f;
        }
    }

    if (m_shockwaveState == ShockwaveState::Expanding)
    {
        m_ringTimer += dt;
        float progress = std::min(1.0f, m_ringTimer / m_config.scaleUpDuration);
        float currentScale = progress * m_config.targetRingScale;

        // Keep ring attached to pet's current position
        m_ringSprite.setPosition(m_position);
        m_ringSprite.setScale(currentScale, currentScale);
        m_ringSprite.setColor(sf::Color(255, 255, 255, m_config.ringMaxOpacity));

        if (m_ringTimer >= m_config.scaleUpDuration)
        {
            // Reached peak scale - start fading down AND trigger knockback/damage at this exact moment!
            m_shockwaveState = ShockwaveState::Fading;
            m_ringTimer = 0.0f;
            TriggerShockwave(enemyPool, damageNumbers);
        }
    }
    else if (m_shockwaveState == ShockwaveState::Fading)
    {
        m_ringTimer += dt;
        float fadeProgress = std::min(1.0f, m_ringTimer / m_config.fadeOutDuration);
        uint8_t currentAlpha = static_cast<uint8_t>(m_config.ringMaxOpacity * (1.0f - fadeProgress));

        // Continue scaling up during fade-out phase (purely visual expansion)
        float currentScale = m_config.targetRingScale + fadeProgress * (m_config.fadeEndRingScale - m_config.targetRingScale);

        m_ringSprite.setPosition(m_position);
        m_ringSprite.setScale(currentScale, currentScale);
        m_ringSprite.setColor(sf::Color(255, 255, 255, currentAlpha));

        if (m_ringTimer >= m_config.fadeOutDuration)
        {
            m_shockwaveState = ShockwaveState::Ready;
            m_cooldownTimer = 0.0f;
        }
    }
}

void PulsePet::TriggerShockwave(EnemyPool& enemyPool, DamageNumberManager* damageNumbers)
{
    float baseWidth = (m_ringTextureData.rect.width > 0) ? static_cast<float>(m_ringTextureData.rect.width) : 32.0f;
    float ringRadius = (baseWidth / 2.0f) * m_config.targetRingScale;
    sf::Vector2f currentRingCenter = m_position;

    std::vector<EnemyBase*> activeEnemies = enemyPool.GetActiveEnemies();
    for (EnemyBase* enemy : activeEnemies)
    {
        if (!enemy || !enemy->IsActive() || !enemy->IsAlive()) continue;

        sf::Vector2f enemyPos = enemy->GetPosition();
        sf::Vector2f diff = enemyPos - currentRingCenter;
        float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        // Only enemies inside the radius of the ring at peak moment get damaged and knocked back
        if (distance <= ringRadius)
        {
            sf::Vector2f knockbackDir = (distance > 0.001f) ? (diff / distance) : sf::Vector2f(1.0f, 0.0f);

            // Deal shockwave damage
            if (m_config.damage > 0.0f)
            {
                enemyPool.ApplyDamageByPointer(enemy, m_config.damage, knockbackDir, m_config.knockbackForce);

                if (damageNumbers)
                {
                    damageNumbers->Spawn(m_config.damage, enemyPos - sf::Vector2f(0.0f, enemy->GetCollisionRadius()));
                }
            }
        }
    }
}

void PulsePet::Draw(sf::RenderTarget& target)
{
    // Draw shockwave ring first so pet draws on top
    if (m_shockwaveState == ShockwaveState::Expanding || m_shockwaveState == ShockwaveState::Fading)
    {
        target.draw(m_ringSprite);
    }

    target.draw(m_petSprite);
}
