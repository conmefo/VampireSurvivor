#pragma once

#include "Weapon.h"
#include <vector>

class SantaWaterWeapon : public Weapon
{
public:
    explicit SantaWaterWeapon(const WeaponProfile& profile);
    ~SantaWaterWeapon() override = default;

    void ReshuffleTargets();

protected:
    void OnLevelUp() override;
    sf::Vector2f GetTargetPosition(EnemyPool& enemyPool, Player& player) override;
    void FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex) override;

private:
    std::vector<float> m_shuffledAngles;
    std::vector<float> m_shuffledRadii;
    int m_angleIndex;
    int m_radiusIndex;

    static constexpr int SEQUENCE_COUNT = 12;
    static constexpr float VIEWPORT_MIN_DIM = 1080.0f / 2.2f; // 490.9f (1080p with 2.2x zoom)
    static constexpr float RADIUS_MIN_RATIO = 0.25f;          // 25% of viewport
    static constexpr float RADIUS_MAX_RATIO = 0.35f;          // 35% of viewport
};
