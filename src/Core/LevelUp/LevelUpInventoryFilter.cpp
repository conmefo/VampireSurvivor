#include "LevelUpInventoryFilter.h"
#include "../../Entities/Weapons/WeaponInventory.h"
#include "../../Entities/Weapons/Weapon.h"
#include "../../Entities/Weapons/WeaponFactory.h"
#include "../../Entities/Player.h"
#include "../../Core/Data/WeaponDataManager.h"

std::vector<LevelUpOption> LevelUpInventoryFilter::GetAvailableOptions(
    const WeaponInventory& inventory,
    const WeaponDataManager& weaponData,
    const std::unordered_set<std::string>& banishedItemIds,
    bool limitBreakEnabled,
    const Player* player) const
{
    std::vector<LevelUpOption> options;

    const auto& allWeapons = weaponData.GetAllWeapons();
    const int currentWeaponCount = static_cast<int>(inventory.GetWeapons().size());
    const int maxWeapons = inventory.GetMaxSlots();

    for (const auto& [id, profile] : allWeapons)
    {
        if (banishedItemIds.find(id) != banishedItemIds.end())
        {
            continue;
        }

        const auto& deltas = weaponData.GetLevelDeltas(id);
        int maxLevel = static_cast<int>(deltas.size()) + 1;

        if(profile.IsPowerUp())
        {
            if(!player)
            {
                continue;
            }

            const int currentLevel = player->GetPassiveLevel(id);
            if(currentLevel == 0 && !profile.IsUnlocked())
            {
                continue;
            }
            if((currentLevel == 0 && player->GetPassiveCount() >= 6) ||
               (currentLevel > 0 && currentLevel >= maxLevel))
            {
                continue;
            }

            LevelUpOption opt;
            opt.id = id;
            opt.name = profile.GetName();
            opt.description = profile.GetDescription();
            opt.frameName = profile.GetFrameName();
            opt.currentLevel = currentLevel;
            opt.maxLevel = maxLevel;
            opt.type = LevelUpOptionType::PassiveUpgrade;
            opt.rarity = profile.GetRarity();
            opt.isNew = currentLevel == 0;
            options.push_back(opt);
            continue;
        }

        // Do not offer a weapon card that this build cannot instantiate.
        if(!WeaponFactory::SupportsBulletType(profile.GetBulletType()))
        {
            continue;
        }

        if(!inventory.HasWeapon(id) && !profile.IsUnlocked())
        {
            continue;
        }

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
            if (currentWeaponCount < maxWeapons)
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
    bool limitBreakEnabled,
    const Player* player) const
{
    std::vector<LevelUpOption> ownedOptions;

    if(player)
    {
        for(const auto& entry : weaponData.GetAllWeapons())
        {
            const std::string& id = entry.first;
            const WeaponProfile& profile = entry.second;
            if(!profile.IsPowerUp() || banishedItemIds.find(id) != banishedItemIds.end())
            {
                continue;
            }

            const int currentLevel = player->GetPassiveLevel(id);
            const int maxLevel = static_cast<int>(weaponData.GetLevelDeltas(id).size()) + 1;
            if(currentLevel > 0 && currentLevel < maxLevel)
            {
                LevelUpOption opt;
                opt.id = id;
                opt.name = profile.GetName();
                opt.description = profile.GetDescription();
                opt.frameName = profile.GetFrameName();
                opt.currentLevel = currentLevel;
                opt.maxLevel = maxLevel;
                opt.type = LevelUpOptionType::PassiveUpgrade;
                opt.rarity = profile.GetRarity();
                opt.isNew = false;
                ownedOptions.push_back(opt);
            }
        }
    }

    for (const auto& weapon : inventory.GetWeapons())
    {
        if(weapon->GetProfile().IsPowerUp())
        {
            continue;
        }

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
