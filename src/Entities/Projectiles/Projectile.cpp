#include "Projectile.h"

Projectile::Projectile(const sf::Texture& texture, const sf::IntRect& textureRect, sf::Vector2f startPosition, sf::Vector2f velocity, float duration, float power, float areaMultiplier, const std::string& hitVfxName, int penetration)
    : m_velocity(velocity)
    , m_duration(duration)
    , m_power(power)
    , m_hitVfxName(hitVfxName)
    , m_penetration(penetration)
{
    m_sprite.setTexture(texture);
    m_sprite.setTextureRect(textureRect);

    // Center origin
    sf::FloatRect bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);

    m_sprite.setPosition(startPosition);
    m_sprite.setScale(areaMultiplier, areaMultiplier);
}

void Projectile::Update(float dt)
{
    if(m_duration > 0.0f)
    {
        m_duration -= dt;
    }

    m_sprite.move(m_velocity * dt);
}

void Projectile::Draw(sf::RenderTarget& target) const
{
    target.draw(m_sprite);
}

bool Projectile::IsExpired() const
{
    if(m_penetration == 0)
    {
        return true;
    }
    return m_duration <= 0.0f;
}

sf::FloatRect Projectile::GetGlobalBounds() const
{
    return m_sprite.getGlobalBounds();
}

float Projectile::GetPower() const
{
    return m_power;
}

const std::string& Projectile::GetHitVfxName() const
{
    return m_hitVfxName;
}

sf::Vector2f Projectile::GetPosition() const
{
    return m_sprite.getPosition();
}

bool Projectile::HasHitEnemy(void* enemyId) const
{
    for(void* id : m_hitEnemies)
    {
        if(id == enemyId)
        {
            return true;
        }
    }
    return false;
}

void Projectile::OnHitEnemy(void* enemyId)
{
    m_hitEnemies.push_back(enemyId);
    if(m_penetration > 0)
    {
        m_penetration--;
    }
}
