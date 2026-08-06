#pragma once

#include "LevelUpOption.h"
#include "LevelUpInventoryFilter.h"
#include <vector>
#include <unordered_set>
#include <random>

class WeaponInventory;
class WeaponDataManager;

class LevelUpRollEngine
{
public:
    LevelUpRollEngine();
    explicit LevelUpRollEngine(unsigned int seed);
    ~LevelUpRollEngine() = default;

    // Rolls level up choices according to Wiki weighted math, Luck multipliers, and owned priority checks
    std::vector<LevelUpOption> RollChoices(
        int playerLevel,
        float luckTotal,
        bool isHealthMissing,
        const WeaponInventory& inventory,
        const WeaponDataManager& weaponData,
        const std::unordered_set<std::string>& banishedItemIds,
        bool limitBreakEnabled = false);

private:
    LevelUpOption RollWeightedRandom(const std::vector<LevelUpOption>& pool);
    LevelUpOption CreateFallbackOption(bool isHealthMissing);

    std::mt19937 m_rng;
    LevelUpInventoryFilter m_filter;
};
