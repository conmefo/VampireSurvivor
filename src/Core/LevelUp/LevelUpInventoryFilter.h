#pragma once

#include "LevelUpOption.h"
#include <string>
#include <vector>
#include <unordered_set>

class WeaponInventory;
class WeaponDataManager;

class LevelUpInventoryFilter
{
public:
    LevelUpInventoryFilter() = default;
    ~LevelUpInventoryFilter() = default;

    // Filters candidate options available to the player from the global database
    std::vector<LevelUpOption> GetAvailableOptions(
        const WeaponInventory& inventory,
        const WeaponDataManager& weaponData,
        const std::unordered_set<std::string>& banishedItemIds,
        bool limitBreakEnabled = false) const;

    // Returns owned options that are upgradeable
    std::vector<LevelUpOption> GetOwnedUpgradeableOptions(
        const WeaponInventory& inventory,
        const WeaponDataManager& weaponData,
        const std::unordered_set<std::string>& banishedItemIds,
        bool limitBreakEnabled = false) const;
};
