#include "LevelUpRollEngine.h"
#include "../../Entities/Weapons/WeaponInventory.h"
#include "../../Core/Data/WeaponDataManager.h"
#include <algorithm>
#include <numeric>

LevelUpRollEngine::LevelUpRollEngine()
    : m_rng(std::random_device{}())
{
}

LevelUpRollEngine::LevelUpRollEngine(unsigned int seed)
    : m_rng(seed)
{
}

LevelUpOption LevelUpRollEngine::CreateFallbackOption(bool isHealthMissing)
{
    LevelUpOption option;
    if (isHealthMissing)
    {
        option.id = "FLOOR_CHICKEN";
        option.name = "Floor Chicken";
        option.description = "Restores 30 Health Points.";
        option.frameName = "FloorChicken";
        option.currentLevel = 0;
        option.maxLevel = 1;
        option.type = LevelUpOptionType::FloorChicken;
        option.rarity = 100;
        option.isNew = false;
    }
    else
    {
        option.id = "GOLD_BAG";
        option.name = "Coin Bag";
        option.description = "Grants 25 Gold Coins.";
        option.frameName = "CoinGold";
        option.currentLevel = 0;
        option.maxLevel = 1;
        option.type = LevelUpOptionType::GoldBag;
        option.rarity = 100;
        option.isNew = false;
    }
    return option;
}

LevelUpOption LevelUpRollEngine::RollWeightedRandom(const std::vector<LevelUpOption>& pool)
{
    if (pool.empty())
    {
        return CreateFallbackOption(false);
    }

    int totalWeight = 0;
    for (const auto& opt : pool)
    {
        totalWeight += std::max(1, opt.rarity);
    }

    std::uniform_int_distribution<int> dist(0, totalWeight - 1);
    int roll = dist(m_rng);

    int accumulated = 0;
    for (const auto& opt : pool)
    {
        accumulated += std::max(1, opt.rarity);
        if (roll < accumulated)
        {
            return opt;
        }
    }

    return pool.back();
}

std::vector<LevelUpOption> LevelUpRollEngine::RollChoices(
    int playerLevel,
    float luckTotal,
    bool isHealthMissing,
    const WeaponInventory& inventory,
    const WeaponDataManager& weaponData,
    const std::unordered_set<std::string>& banishedItemIds,
    bool limitBreakEnabled)
{
    std::vector<LevelUpOption> chosenOptions;

    float luck = std::max(1.0f, luckTotal);
    std::uniform_real_distribution<float> floatDist(0.0f, 1.0f);

    // 1. Determine total slots (3 base, 4th slot based on Luck formula)
    float chanceFourth = 1.0f - (1.0f / luck);
    int targetSlots = (floatDist(m_rng) < chanceFourth) ? 4 : 3;

    // 2. Fetch active candidate pool and owned upgradeable pool
    std::vector<LevelUpOption> activePool = m_filter.GetAvailableOptions(inventory, weaponData, banishedItemIds, limitBreakEnabled);
    std::vector<LevelUpOption> ownedPool = m_filter.GetOwnedUpgradeableOptions(inventory, weaponData, banishedItemIds, limitBreakEnabled);

    if (activePool.empty())
    {
        for (int i = 0; i < targetSlots; ++i)
        {
            chosenOptions.push_back(CreateFallbackOption(isHealthMissing));
        }
        return chosenOptions;
    }

    std::unordered_set<std::string> chosenIds;

    // 3. Owned Item Priority Pass (executed up to twice for slots 0 & 1)
    int x = (playerLevel % 2 == 0) ? 2 : 1;
    float chanceOwned = (0.3f * static_cast<float>(x)) / luck;

    for (int check = 0; check < 2 && chosenOptions.size() < static_cast<size_t>(targetSlots); ++check)
    {
        if (!ownedPool.empty() && floatDist(m_rng) < chanceOwned)
        {
            LevelUpOption ownedChoice = RollWeightedRandom(ownedPool);
            if (chosenIds.find(ownedChoice.id) == chosenIds.end())
            {
                chosenIds.insert(ownedChoice.id);
                chosenOptions.push_back(ownedChoice);
            }
            // If duplicate, it is discarded and left for general pool draw (per wiki spec)
        }
    }

    // 4. Fill remaining slots from global candidate pool using weighted random draw
    while (chosenOptions.size() < static_cast<size_t>(targetSlots))
    {
        // Filter out already chosen IDs from available active pool
        std::vector<LevelUpOption> remainingPool;
        for (const auto& opt : activePool)
        {
            if (chosenIds.find(opt.id) == chosenIds.end())
            {
                remainingPool.push_back(opt);
            }
        }

        if (remainingPool.empty())
        {
            // If active pool is exhausted, fill remaining slots with fallback option
            chosenOptions.push_back(CreateFallbackOption(isHealthMissing));
        }
        else
        {
            LevelUpOption rolled = RollWeightedRandom(remainingPool);
            chosenIds.insert(rolled.id);
            chosenOptions.push_back(rolled);
        }
    }

    return chosenOptions;
}
