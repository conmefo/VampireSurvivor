#include "AttractorbItem.h"
#include "../Player.h"
#include <iostream>

AttractorbItem::AttractorbItem(const WeaponProfile& profile)
    : Weapon(profile)
{
}

void AttractorbItem::Update(float dt, ProjectileManager& projManager, TextureAtlas& atlas, Player& player, EnemyPool& enemyPool)
{
    // On first equip, apply level 1 base magnet bonus (+50%)
    if (!m_isEquipped)
    {
        m_isEquipped = true;
        float baseBonus = 0.50f;
        if (m_profile.GetMagnet() > 0.0f)
        {
            baseBonus = m_profile.GetMagnet();
        }
        player.AddMagnetBonus(baseBonus);
        m_lastAppliedLevel = 1;
        std::cout << "[AttractorbItem] Equipped! Level 1 (+ " << (baseBonus * 100.0f) << "% Magnet). New Radius: " << player.GetMagnetRadius() << "\n";
    }

    // Check for level-ups and apply additive magnet bonuses
    int currentLevel = m_profile.GetCurrentLevel();
    while (m_lastAppliedLevel < currentLevel)
    {
        int nextLevel = m_lastAppliedLevel + 1;
        int deltaIdx = nextLevel - 2; // Delta index for level 2 is 0
        float bonus = 0.0f;

        const auto& deltas = GetLevelDeltas();
        if (deltaIdx >= 0 && deltaIdx < static_cast<int>(deltas.size()) && deltas[deltaIdx].magnet > 0.0f)
        {
            bonus = deltas[deltaIdx].magnet;
        }
        else
        {
            // Standard compounding Attractorb level deltas: L2:+33%, L3:+25%, L4:+20%, L5:+33%
            static const float defaultDeltas[] = { 0.33f, 0.25f, 0.20f, 0.33f };
            if (deltaIdx >= 0 && deltaIdx < 4)
            {
                bonus = defaultDeltas[deltaIdx];
            }
            else
            {
                bonus = 0.20f;
            }
        }

        player.AddMagnetBonus(bonus);
        m_lastAppliedLevel = nextLevel;
        std::cout << "[AttractorbItem] Leveled up to Lv " << nextLevel << "! (+ " << (bonus * 100.0f) << "% Magnet). New Radius: " << player.GetMagnetRadius() << "\n";
    }
}

void AttractorbItem::OnLevelUp()
{
    // Passive item level-up side effects applied in Update() with Player context
}

void AttractorbItem::FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex)
{
    // Passive item: does not fire active projectiles
}
