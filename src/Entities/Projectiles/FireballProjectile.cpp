#include "FireballProjectile.h"
#include <cmath>

FireballProjectile::FireballProjectile(ProjectileManager* projManager, TextureAtlas* atlas, const sf::Texture& texture, const sf::IntRect& textureRect, sf::Vector2f startPosition, sf::Vector2f velocity, float duration, float power, float areaMultiplier, const std::string& hitVfxName, int penetration)
    : Projectile(texture, textureRect, startPosition, velocity, duration, power, areaMultiplier, hitVfxName, penetration)
    , m_projManager(projManager)
    , m_atlas(atlas)
    , m_areaMultiplier(areaMultiplier)
{
    // Start small and scale up to full size over 0.2 seconds
    m_sprite.setScale(0.0f, 0.0f);
    SetScaleTween(sf::Vector2f(areaMultiplier, areaMultiplier), 0.2f);

    // We will initialize the emitter in Update to sync with the tuning UI!
    // So we don't need to hardcode the config here anymore.
}

FireballProjectile::~FireballProjectile()
{
    if (m_trailEmitter)
    {
        m_trailEmitter->Stop();
    }
    Explode();
}

extern float g_FireWandScale;

void FireballProjectile::Update(float dt)
{
    Projectile::Update(dt);
    
    // Apply real-time g_FireWandScale slider tuning
    m_sprite.setScale(m_areaMultiplier * g_FireWandScale, m_areaMultiplier * g_FireWandScale);

    if (m_projManager && m_projManager->GetParticleManager() != nullptr && m_trailEmitter == nullptr) {
        auto fireWandConfig = m_projManager->GetParticleManager()->GetConfig("fireWand");
        
        // Adjust for weapon area multiplier directly before spawning
        fireWandConfig.minSize *= m_areaMultiplier;
        fireWandConfig.maxSize *= m_areaMultiplier;
        
        m_trailEmitter = m_projManager->GetParticleManager()->SpawnEmitter(fireWandConfig, GetPosition());
    }

    if (m_trailEmitter)
    {
        // Calculate direction for particles to shoot backwards
        sf::Vector2f dir = m_velocity;
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len > 0.0001f) {
            dir /= len;
        } else {
            dir = sf::Vector2f(1.0f, 0.0f);
        }
        
        m_trailEmitter->GetConfig().emissionDirection = -dir;

        // Use the emitter's config for offset
        sf::Vector2f tailPos = GetPosition() - dir * m_trailEmitter->GetConfig().emitterOffset;
        m_trailEmitter->SetPosition(tailPos);
    }
}

void FireballProjectile::Explode()
{
    if (!m_projManager || !m_atlas || m_hasExploded) return;
    m_hasExploded = true;

    // Trigger the FireballSequence state machine
    // This perfectly matches FireballSpawner$$FireballSequence
    float baseDelay = 0.1f; // The internal delay value (param_1 + 0x40)

    auto spawnExplosion = [pm = m_projManager, atlas = m_atlas, pos = GetPosition(), pwr = GetPower(), area = m_sprite.getScale().x](int isLast) {
        // Find the authentic explosion visual (e.g. from FireExplosionProjectile)
        AssetTextureData data = atlas->GetTextureData("WhiteDot"); 
        if(!data.texture) data = atlas->GetTextureData("ProjectileFireball2"); // fallback

        if (data.texture)
        {
            auto proj = std::make_unique<Projectile>(*data.texture, data.rect, pos, sf::Vector2f(0.f, 0.f), 0.5f, pwr, area, "");
            proj->SetScaleTween(sf::Vector2f(area * 1.5f, area * 1.5f), 0.2f);
            pm->AddProjectile(std::move(proj));
        }
    };

    m_projManager->QueueDelayedAction(0.0f, [=]() { spawnExplosion(0); });
    m_projManager->QueueDelayedAction(baseDelay, [=]() { spawnExplosion(0); });
    m_projManager->QueueDelayedAction(baseDelay * 2.0f, [=]() { spawnExplosion(0); });
    m_projManager->QueueDelayedAction(baseDelay * 3.0f, [=]() { spawnExplosion(1); });
    
    // If Uppercut is unlocked, we would trigger SpawnUppercut with baseDelay * 4.0f here!
}
