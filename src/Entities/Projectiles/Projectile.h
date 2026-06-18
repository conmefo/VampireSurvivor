#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class Projectile
{
public:
    Projectile(const sf::Texture& texture, const sf::IntRect& textureRect, sf::Vector2f startPosition, sf::Vector2f velocity, float duration, float power, float areaMultiplier, const std::string& hitVfxName, int penetration = -1);
    virtual ~Projectile() = default;

    virtual void Update(float dt);
    virtual void Draw(sf::RenderTarget& target) const;

    bool IsExpired() const;
    sf::FloatRect GetGlobalBounds() const;
    float GetPower() const;
    const std::string& GetHitVfxName() const;

    sf::Vector2f GetPosition() const;
    bool HasHitEnemy(void* enemyId) const;
    void OnHitEnemy(void* enemyId);

protected:
    sf::Sprite m_sprite;
    sf::Vector2f m_velocity;
    float m_duration;
    float m_power;
    std::string m_hitVfxName;
    int m_penetration;
    std::vector<void*> m_hitEnemies;
};
