#pragma once

#include "Weapon.h"
#include <SFML/Graphics.hpp>

class GarlicWeapon : public Weapon
{
public:
    explicit GarlicWeapon(const WeaponProfile& profile);
    ~GarlicWeapon() override = default;

    void Update(float dt, ProjectileManager& projManager, TextureAtlas& atlas, Player& player, EnemyPool& enemyPool) override;
    void Draw(sf::RenderTarget& target) const override;

protected:
    void FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex) override;

private:
    float m_accumulatorMs = 0.0f;
    float m_rotationAngle = 0.0f;
    float m_alphaTimer = 0.0f;
    
    // Persistent visual sprite drawn directly by the weapon
    sf::Sprite m_visualSprite;
    bool m_visualInitialized = false;
};
