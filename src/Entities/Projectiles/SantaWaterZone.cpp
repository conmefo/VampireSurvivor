#include "SantaWaterZone.h"
#include "../../Core/Resources/TextureAtlas.h"

float g_SantaWaterZoneRadius = 16.0f;
float g_SantaWaterZoneAlpha = 51.0f;
float g_SantaWaterColorR = 0.0f;
float g_SantaWaterColorG = 120.0f;
float g_SantaWaterColorB = 255.0f;
float g_SantaWaterFlameRate = 60.0f;
float g_SantaWaterFlameLife = 0.5f;
float g_SantaWaterFlameSizeStart = 0.2f;
float g_SantaWaterFlameSizeEnd = 0.70f;
float g_SantaWaterFlameBoundaryMultiplier = 1.15f;

SantaWaterZone::SantaWaterZone(TextureAtlas* atlas, const sf::Texture& texture, const sf::IntRect& textureRect, sf::Vector2f targetPosition, float duration, float power, float areaMultiplier, float hitBoxDelay, const std::string& hitVfxName)
    : Projectile(texture, textureRect, targetPosition, sf::Vector2f(0.0f, 0.0f), duration, power, areaMultiplier, hitVfxName, -1)
    , m_areaMultiplier(areaMultiplier)
    , m_hitBoxDelay(hitBoxDelay)
    , m_tickTimer(0.0f)
    , m_atlas(atlas)
{
    float radius = g_SantaWaterZoneRadius * areaMultiplier;

    // Translucent blue ground circle visual indicator (no outline)
    m_blueCircleOverlay.setRadius(radius);
    m_blueCircleOverlay.setOrigin(radius, radius);
    m_blueCircleOverlay.setPosition(targetPosition);
    m_blueCircleOverlay.setFillColor(sf::Color(
        static_cast<sf::Uint8>(g_SantaWaterColorR),
        static_cast<sf::Uint8>(g_SantaWaterColorG),
        static_cast<sf::Uint8>(g_SantaWaterColorB),
        static_cast<sf::Uint8>(g_SantaWaterZoneAlpha)
    ));
    m_blueCircleOverlay.setOutlineThickness(0.0f);

    // Hide default sprite in favor of blue ground pool shape
    m_sprite.setPosition(targetPosition);
    m_sprite.setScale(0.0f, 0.0f);

    // Create flame particle emitter using ProjectileFlameHoly
    if(m_atlas)
    {
        AssetTextureData flameData = m_atlas->GetTextureData("ProjectileFlameHoly");
        if(flameData.texture && flameData.rect.width > 0)
        {
            vs::ParticleEmitterConfig config;
            config.startLifetime = g_SantaWaterFlameLife;
            config.startSpeed = 0.0f; // Stationary flames inside pool
            config.emissionRate = g_SantaWaterFlameRate * areaMultiplier * areaMultiplier; // Scale with area so larger zones stay visually full
            config.minSize = g_SantaWaterFlameSizeStart;
            config.maxSize = g_SantaWaterFlameSizeStart;
            config.endSize = g_SantaWaterFlameSizeEnd; // Slow scale-up over lifetime
            config.randomRotation = false;
            config.startColor = sf::Color(255, 255, 255, 255);
            config.endColor = sf::Color(255, 255, 255, 0); // Fade out 255 -> 0
            config.shapeRadius = radius * g_SantaWaterFlameBoundaryMultiplier; // Boundary relative to blue circle
            config.looping = true;

            m_flameEmitter = std::make_unique<vs::ParticleEmitter>(config, targetPosition, flameData.rect);
        }
    }
}

void SantaWaterZone::Update(float dt)
{
    Projectile::Update(dt);

    float radius = g_SantaWaterZoneRadius * m_areaMultiplier;
    m_blueCircleOverlay.setRadius(radius);
    m_blueCircleOverlay.setOrigin(radius, radius);
    m_blueCircleOverlay.setFillColor(sf::Color(
        static_cast<sf::Uint8>(g_SantaWaterColorR),
        static_cast<sf::Uint8>(g_SantaWaterColorG),
        static_cast<sf::Uint8>(g_SantaWaterColorB),
        static_cast<sf::Uint8>(g_SantaWaterZoneAlpha)
    ));

    if(m_flameEmitter)
    {
        // When the zone has expired, stop emitting new flames but let existing ones finish
        if(m_duration <= 0.0f && !m_zoneExpired)
        {
            m_zoneExpired = true;
            m_flameEmitter->Stop();
        }
        m_flameEmitter->GetConfig().shapeRadius = radius * g_SantaWaterFlameBoundaryMultiplier;
        m_flameEmitter->GetConfig().startLifetime = g_SantaWaterFlameLife;
        m_flameEmitter->GetConfig().emissionRate = g_SantaWaterFlameRate * m_areaMultiplier * m_areaMultiplier;
        m_flameEmitter->GetConfig().minSize = g_SantaWaterFlameSizeStart;
        m_flameEmitter->GetConfig().maxSize = g_SantaWaterFlameSizeStart;
        m_flameEmitter->GetConfig().endSize = g_SantaWaterFlameSizeEnd;
        m_flameEmitter->Update(dt);
    }

    m_tickTimer += dt;
    if(m_tickTimer >= m_hitBoxDelay)
    {
        m_tickTimer -= m_hitBoxDelay;
        m_hitEnemies.clear();
    }
}

void SantaWaterZone::Draw(sf::RenderTarget& target) const
{
    // Only draw the blue circle while the zone is still alive
    if(!m_zoneExpired)
    {
        target.draw(m_blueCircleOverlay);
    }

    // Draw active ProjectileFlameHoly particles on top with 90 degree rotation
    if(m_flameEmitter && m_atlas)
    {
        AssetTextureData flameData = m_atlas->GetTextureData("ProjectileFlameHoly");
        if(flameData.texture)
        {
            sf::Sprite sprite(*flameData.texture, flameData.rect);
            sprite.setOrigin(flameData.rect.width / 2.0f, flameData.rect.height / 2.0f);

            for(const auto& particle : m_flameEmitter->GetParticles())
            {
                sprite.setPosition(particle.position);
                sprite.setScale(particle.size, particle.size);
                sprite.setRotation(90.0f + particle.rotation); // Rotated 90 degrees
                sprite.setColor(particle.color);
                target.draw(sprite);
            }
        }
    }
}

bool SantaWaterZone::IsExpired() const
{
    // Keep alive until both the blue circle is gone AND all flame particles have finished
    if(!m_zoneExpired)
    {
        return false;
    }
    if(m_flameEmitter && !m_flameEmitter->IsFinished())
    {
        return false;
    }
    return true;
}

void SantaWaterZone::OnHitEnemy(void* enemyId)
{
    m_hitEnemies.push_back(enemyId);
}
