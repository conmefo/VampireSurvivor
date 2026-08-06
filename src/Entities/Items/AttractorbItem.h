#pragma once

#include "../Weapons/Weapon.h"

// Passive Item class for Attractorb ("MAGNET").
// Encapsulates passive stat bonuses (magnet radius) applied to Player on equip and level-up.
class AttractorbItem : public Weapon
{
public:
    explicit AttractorbItem(const WeaponProfile& profile);
    ~AttractorbItem() override = default;

    void Update(float dt, ProjectileManager& projManager, TextureAtlas& atlas, Player& player, EnemyPool& enemyPool) override;

protected:
    void OnLevelUp() override;
    void FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex) override;

private:
    bool m_isEquipped = false;
    int m_lastAppliedLevel = 0;
};
