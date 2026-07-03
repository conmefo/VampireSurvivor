#include "MagicMissileProjectile.h"
#include <cmath>
#include <algorithm>
#include <cstdlib> // For rand()

MagicMissileProjectile::MagicMissileProjectile(const sf::Texture& texture, const sf::IntRect& rect1, const sf::IntRect& rect2, sf::Vector2f startPosition, sf::Vector2f velocity, float duration, float power, float areaMultiplier, const std::string& hitVfxName, int penetration)
    : Projectile(texture, rect1, startPosition, velocity, duration, power, areaMultiplier, hitVfxName, penetration)
    , m_currentFrameIndex(0)
    , m_animationTimer(0.0f)
{
    m_frames.push_back(rect1);
    m_frames.push_back(rect2);

    // Center origin
    sf::FloatRect bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);

    // Force the weapon sprite to be 27x27 pixels, respecting area multiplier
    m_baseScaleX = (45.0f / bounds.width) * areaMultiplier;
    m_baseScaleY = (29.0f / bounds.height) * areaMultiplier;
    m_sprite.setScale(m_baseScaleX, m_baseScaleY);

    // Compute angle of velocity for rotation
    float angle = std::atan2(velocity.y, velocity.x) * 180.0f / 3.14159265f;
    m_sprite.setRotation(angle);
}

MagicMissileProjectile::~MagicMissileProjectile()
{
    if (m_emitter) {
        m_emitter->Stop();
    }
}

void MagicMissileProjectile::Update(float dt)
{
    Projectile::Update(dt);

    m_animationTimer += dt;
    if(m_animationTimer >= FRAME_TIME)
    {
        m_animationTimer -= FRAME_TIME;
        m_currentFrameIndex = (m_currentFrameIndex + 1) % 2;
        m_sprite.setTextureRect(m_frames[m_currentFrameIndex]);
    }

    if (m_particleManager != nullptr && m_emitter == nullptr) {
        auto magicWandConfig = m_particleManager->GetConfig("magicWand");
        m_emitter = m_particleManager->SpawnEmitter(magicWandConfig, m_sprite.getPosition());
        
        // Apply static scaling based on config
        float scaleX = magicWandConfig.weaponScaleX;
        float scaleY = magicWandConfig.weaponScaleY;
        m_sprite.setScale(m_baseScaleX * scaleX, m_baseScaleY * scaleY);
    }

    if (m_emitter) {
        sf::Vector2f dir = m_velocity;
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len > 0.0001f) {
            dir /= len;
        } else {
            dir = sf::Vector2f(1.0f, 0.0f);
        }
        
        // Don't modify the global config, modify our emitter's config directly
        m_emitter->GetConfig().emissionDirection = -dir;
        
        sf::Vector2f tailPos = m_sprite.getPosition() - dir * m_emitter->GetConfig().emitterOffset;
        m_emitter->SetPosition(tailPos);
    }
}

void MagicMissileProjectile::Draw(sf::RenderTarget& target) const
{
    // Draw main projectile
    Projectile::Draw(target);
}
