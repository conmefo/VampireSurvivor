#include "LightningRingProjectile.h"
#include "../../Core/Resources/TextureAtlas.h"
#include <cmath>
#include <random>

// Global Tuning Parameter Defaults
float g_LightningRingSpeed = 3090.0f;
float g_LightningRingScaleX = 1.50f;
float g_LightningRingScaleY = 0.50f;
float g_LightningRingRotation = 90.0f;
float g_LightningRingStrikeRadius = 40.0f;
float g_LightningRingDuration = 0.4f;

LightningRingProjectile::LightningRingProjectile(
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
)
    : Projectile(texture, textureRect, targetPosition, sf::Vector2f(0.0f, 0.0f), duration, power, areaMultiplier, hitVfxName, -1)
    , m_projManager(projManager)
    , m_atlas(atlas)
    , m_enemyPool(enemyPool)
    , m_targetPos(targetPosition)
    , m_areaMultiplier(areaMultiplier)
    , m_scrollOffset(0.0f)
    , m_speed(g_LightningRingSpeed)
    , m_hasStruck(false)
{
    // Apply rotation angle from tuning variable
    m_sprite.setRotation(g_LightningRingRotation);
    
    // Origin at top-left of rotated sprite (0, height/2)
    m_sprite.setOrigin(0.0f, textureRect.height * 0.5f);

    // Apply scale settings from tuning variables
    float scaleX = g_LightningRingScaleX * m_areaMultiplier;
    float scaleY = g_LightningRingScaleY * m_areaMultiplier;
    m_sprite.setScale(scaleX, scaleY);
}

void LightningRingProjectile::Update(float dt)
{
    m_age += dt;
    if (m_duration > 0.0f)
    {
        m_duration -= dt;
    }

    // Dynamic scale and rotation update for live tuning feedback
    float scaleX = g_LightningRingScaleX * m_areaMultiplier;
    float scaleY = g_LightningRingScaleY * m_areaMultiplier;
    m_sprite.setScale(scaleX, scaleY);
    m_sprite.setRotation(g_LightningRingRotation);

    // Scroll downward endlessly using global speed slider value
    m_scrollOffset += g_LightningRingSpeed * dt;

    // Check if leading tip has reached target ground level
    float screenTopY = m_targetPos.y - 1000.0f;
    float leadingTipY = screenTopY + m_age * g_LightningRingSpeed;

    if (!m_hasStruck && leadingTipY >= m_targetPos.y)
    {
        ExecuteStrike();
    }
}

void LightningRingProjectile::ExecuteStrike()
{
    m_hasStruck = true;

    // Apply circular Area of Effect damage at target location using global radius slider value
    if (m_enemyPool)
    {
        float strikeRadius = g_LightningRingStrikeRadius * m_areaMultiplier;
        float radiusSq = strikeRadius * strikeRadius;

        const auto& activeEnemies = m_enemyPool->GetActiveEnemies();
        for (auto* enemy : activeEnemies)
        {
            if (enemy && enemy->IsAlive())
            {
                sf::Vector2f diff = enemy->GetPosition() - m_targetPos;
                if (diff.x * diff.x + diff.y * diff.y <= radiusSq)
                {
                    enemy->TakeDamage(GetPower());
                }
            }
        }
    }

    // Trigger ground impact hit VFX if atlas is available
    if (m_projManager && m_atlas && !m_hitVfxName.empty())
    {
        AssetTextureData vfxData = m_atlas->GetTextureData(m_hitVfxName);
        if (vfxData.texture && vfxData.rect.width > 0)
        {
            auto hitEffect = std::make_unique<Projectile>(
                *vfxData.texture,
                vfxData.rect,
                m_targetPos,
                sf::Vector2f(0.0f, 0.0f),
                0.15f,
                0.0f,
                m_areaMultiplier,
                ""
            );
            hitEffect->SetScaleTween(sf::Vector2f(m_areaMultiplier, m_areaMultiplier), 0.1f);
            m_projManager->AddProjectile(std::move(hitEffect));
        }
    }
}

void LightningRingProjectile::Draw(sf::RenderTarget& target) const
{
    if (IsExpired())
        return;

    sf::IntRect texRect = m_sprite.getTextureRect();
    // After 90 deg rotation, vertical segment height = textureRect.width * scale.x
    const float spriteSegmentHeight = texRect.width * m_sprite.getScale().x;
    if (spriteSegmentHeight <= 0.0f)
        return;

    float screenTopY = m_targetPos.y - 1000.0f; // Above screen top boundary
    
    // Leading lower tip travels down from screenTopY towards targetY at exact speed (g_LightningRingSpeed)
    float calculatedTipY = screenTopY + m_age * g_LightningRingSpeed;
    float currentTipY = std::min(m_targetPos.y, calculatedTipY);

    // Modulo scroll offset for seamless looping segment stack
    float modOffset = std::fmod(m_scrollOffset, spriteSegmentHeight);

    float startY = screenTopY + modOffset;
    if (startY > screenTopY)
    {
        startY -= spriteSegmentHeight;
    }

    sf::RenderStates states;
    states.blendMode = sf::BlendAdd;

    // Seamless vertical segment loop: stack from screenTopY down to currentTipY
    for (float currentY = startY; currentY < currentTipY; currentY += spriteSegmentHeight - 0.5f)
    {
        float segmentTop = currentY;
        float segmentBottom = currentY + spriteSegmentHeight;

        if (segmentBottom <= screenTopY)
            continue;

        if (segmentTop >= currentTipY)
            break;

        // Clip bottom-most segment if it crosses currentTipY
        if (segmentBottom > currentTipY)
        {
            float visibleHeight = currentTipY - segmentTop;
            float visibleRatio = visibleHeight / spriteSegmentHeight;

            sf::IntRect clippedRect = texRect;
            clippedRect.width = static_cast<int>(texRect.width * visibleRatio);

            if (clippedRect.width > 0)
            {
                sf::Sprite clippedSprite(*m_sprite.getTexture(), clippedRect);
                clippedSprite.setOrigin(0.0f, texRect.height * 0.5f);
                clippedSprite.setRotation(90.0f);
                clippedSprite.setScale(m_sprite.getScale());
                clippedSprite.setPosition(m_targetPos.x, segmentTop);
                target.draw(clippedSprite, states);
            }
        }
        else
        {
            // Full segment drawing starting exactly at currentY
            sf::Sprite segmentSprite = m_sprite;
            segmentSprite.setPosition(m_targetPos.x, currentY);
            target.draw(segmentSprite, states);
        }
    }
}
