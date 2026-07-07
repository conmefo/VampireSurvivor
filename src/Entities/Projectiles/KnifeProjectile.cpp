#include "KnifeProjectile.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

KnifeProjectile::KnifeProjectile(const sf::Texture& texture, const sf::IntRect& rect, sf::Vector2f startPosition, sf::Vector2f velocity, float duration, float power, float areaMultiplier, const std::string& hitVfxName, int penetration)
    : Projectile(texture, rect, startPosition, velocity, duration, power, areaMultiplier, hitVfxName, penetration)
{
    m_sprite.setScale(areaMultiplier, areaMultiplier);

    // Calculate rotation based on velocity vector
    if(velocity.x != 0.0f || velocity.y != 0.0f)
    {
        float angle = std::atan2(velocity.y, velocity.x) * 180.0f / static_cast<float>(M_PI);
        m_sprite.setRotation(angle);
    }
}

void KnifeProjectile::Update(float dt)
{
    Projectile::Update(dt);
}
