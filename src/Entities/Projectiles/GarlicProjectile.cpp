#include "GarlicProjectile.h"
#include "../Enemy/EnemyBase.h"

GarlicProjectile::GarlicProjectile(const sf::Texture& texture, const sf::IntRect& textureRect, sf::Vector2f startPosition, float duration, float power, float areaMultiplier, const std::string& hitVfxName, int penetration)
    : Projectile(texture, textureRect, startPosition, sf::Vector2f(0.0f, 0.0f), duration, power, areaMultiplier, hitVfxName, penetration)
{
    // Garlic is a circular static pulse, so center the origin exactly
    m_sprite.setOrigin(textureRect.width / 2.0f, textureRect.height / 2.0f);
}

void GarlicProjectile::Update(float dt)
{
    // Garlic moves strictly locked to the player's position, updated by setPosition
    // Bypasses standard velocity vector updates
    if (m_duration > 0.0f)
    {
        m_duration -= dt;
    }
}

void GarlicProjectile::Draw(sf::RenderTarget& target) const
{
    // Override base Draw to do nothing: GarlicProjectile acts purely as a collision checker
    (void)target;
}

void GarlicProjectile::OnHitEnemy(void* enemyId)
{
    Projectile::OnHitEnemy(enemyId);

    // Apply the original game's garlic hit debuffs to the enemy
    EnemyBase* enemy = static_cast<EnemyBase*>(enemyId);
    if (enemy)
    {
        // Reduce knockback resistance by 0.3
        enemy->ReduceKnockbackResistance(0.3f);
        // Reduce freeze/status resistance by 0.1
        enemy->ReduceStatusResistance(0.1f);
    }
}

void GarlicProjectile::SetCenterPosition(const sf::Vector2f& pos)
{
    m_sprite.setPosition(pos);
}
