#pragma once

#include <string>

enum class LevelUpOptionType
{
    WeaponUpgrade,
    PassiveUpgrade,
    FloorChicken,
    GoldBag
};

struct LevelUpOption
{
    std::string id;
    std::string name;
    std::string description;
    std::string frameName;
    int currentLevel = 0;
    int maxLevel = 1;
    LevelUpOptionType type = LevelUpOptionType::WeaponUpgrade;
    int rarity = 100;
    bool isNew = false;
};
