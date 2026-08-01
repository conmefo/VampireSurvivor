#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "../Particles/ParticleManager.h"

class Projectile
{
public:
    Projectile(const sf::Texture& texture, const sf::IntRect& textureRect, sf::Vector2f startPosition, sf::Vector2f velocity, float duration, float power, float areaMultiplier, const std::string& hitVfxName, int penetration = -1);
    virtual ~Projectile() = default;

    virtual void Update(float dt);
    virtual void Draw(sf::RenderTarget& target) const;

    virtual bool IsExpired() const;
    sf::FloatRect GetGlobalBounds() const;
    float GetPower() const;
    const std::string& GetHitVfxName() const;

    sf::Vector2f GetPosition() const;
    virtual bool HasHitEnemy(void* enemyId) const;
    virtual void OnHitEnemy(void* enemyId);

    void SetParticleManager(vs::ParticleManager* pm) { m_particleManager = pm; }
    vs::ParticleManager* GetParticleManager() const { return m_particleManager; }

protected:
    sf::Sprite m_sprite;
    sf::Vector2f m_velocity;
    float m_duration;
    float m_power;
    std::string m_hitVfxName;
    int m_penetration;
    vs::ParticleManager* m_particleManager = nullptr;
    std::vector<void*> m_hitEnemies;

    bool m_isScaling = false;
    float m_scaleDuration = 0.0f;
    float m_scaleTimer = 0.0f;
    sf::Vector2f m_initialScale;
    sf::Vector2f m_targetScale;

public:
    void SetColor(sf::Color color)
    {
        m_sprite.setColor(color);
    }

    void SetScaleTween(sf::Vector2f targetScale, float duration)
    {
        m_initialScale = m_sprite.getScale();
        m_targetScale = targetScale;
        m_scaleDuration = duration;
        m_scaleTimer = 0.0f;
        m_isScaling = true;
    }
};
