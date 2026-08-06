#include "LevelUpInventoryFilter.h"
#include "../../Entities/Weapons/WeaponInventory.h"
#include "../../Entities/Weapons/Weapon.h"
#include "../../Core/Data/WeaponDataManager.h"

std::vector<LevelUpOption> LevelUpInventoryFilter::GetAvailableOptions(
    const WeaponInventory& inventory,
    const WeaponDataManager& weaponData,
    const std::unordered_set<std::string>& banishedItemIds,
    bool limitBreakEnabled) const
{
    std::vector<LevelUpOption> options;

    const auto& allWeapons = weaponData.GetAllWeapons();
    const int currentWeaponCount = static_cast<int>(inventory.GetWeapons().size());
    constexpr int MAX_WEAPONS = 6;

    for (const auto& [id, profile] : allWeapons)
    {
        if (banishedItemIds.find(id) != banishedItemIds.end())
        {
            continue;
        }

        const auto& deltas = weaponData.GetLevelDeltas(id);
        int maxLevel = static_cast<int>(deltas.size()) + 1;

        if (inventory.HasWeapon(id))
        {
            const Weapon* weapon = inventory.GetWeapon(id);
            if (weapon)
            {
                if (weapon->CanLevelUp() || limitBreakEnabled)
                {
                    LevelUpOption opt;
                    opt.id = id;
                    opt.name = profile.GetName();
                    opt.description = profile.GetDescription();
                    opt.frameName = profile.GetFrameName();
                    opt.currentLevel = profile.GetCurrentLevel();
                    opt.maxLevel = maxLevel;
                    opt.type = LevelUpOptionType::WeaponUpgrade;
                    opt.rarity = profile.GetRarity();
                    opt.isNew = false;
                    options.push_back(opt);
                }
            }
        }
        else
        {
            if (currentWeaponCount < MAX_WEAPONS)
            {
                LevelUpOption opt;
                opt.id = id;
                opt.name = profile.GetName();
                opt.description = profile.GetDescription();
                opt.frameName = profile.GetFrameName();
                opt.currentLevel = 0;
                opt.maxLevel = maxLevel;
                opt.type = LevelUpOptionType::WeaponUpgrade;
                opt.rarity = profile.GetRarity();
                opt.isNew = true;
                options.push_back(opt);
            }
        }
    }

    return options;
}

std::vector<LevelUpOption> LevelUpInventoryFilter::GetOwnedUpgradeableOptions(
    const WeaponInventory& inventory,
    const WeaponDataManager& weaponData,
    const std::unordered_set<std::string>& banishedItemIds,
    bool limitBreakEnabled) const
{
    std::vector<LevelUpOption> ownedOptions;

    for (const auto& weapon : inventory.GetWeapons())
    {
        std::string id = weapon->GetProfile().GetId();
        if (banishedItemIds.find(id) != banishedItemIds.end())
        {
            continue;
        }

        if (weapon->CanLevelUp() || limitBreakEnabled)
        {
            const auto& deltas = weaponData.GetLevelDeltas(id);
            LevelUpOption opt;
            opt.id = id;
            opt.name = weapon->GetProfile().GetName();
            opt.description = weapon->GetProfile().GetDescription();
            opt.currentLevel = weapon->GetProfile().GetCurrentLevel();
            opt.maxLevel = static_cast<int>(deltas.size()) + 1;
            opt.type = LevelUpOptionType::WeaponUpgrade;
            opt.rarity = weapon->GetProfile().GetRarity();
            opt.isNew = false;
            ownedOptions.push_back(opt);
        }
    }

    return ownedOptions;
}
