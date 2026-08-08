#pragma once

#include "Core/Resources/TextureAtlas.h"
#include "UI/Components/NineSliceComponent.h"
#include <SFML/Graphics.hpp>
#include <functional>

#include <vector>
#include <string>

struct ChestRewardItem
{
    std::string iconKey;       // Atlas key e.g. "WandFire", "Garlic", "Whip", "CoinBag"
    std::string itemName;      // Display title e.g. "Fire Wand"
    std::string description;   // Description string from weapon/item JSON
    int newLevel = 1;          // Level rank e.g. 5
    bool isEvolution = false;  // Evolution indicator
};

struct TreasureRewardViewContext
{
    TextureAtlas& atlas;
    const sf::Font& font;

    NineSliceComponent& nineSliceBg;
    sf::Vector2f panelPos;
    sf::Vector2f panelSize{640.0f, 860.0f};
    sf::Vector2f viewSize{1920.0f, 1080.0f};
    sf::Vector2f viewCenter{960.0f, 540.0f};

    int goldReward = 0;
    int currentRunGold = 0;
    int itemCount = 1; // 1, 3, or 5 item chest
    std::vector<ChestRewardItem> rewards;

    std::function<void(int)> onGoldAdded;
    std::function<void()> onComplete;
    std::function<void()> triggerOpening;
    std::function<void()> triggerReward;
    std::function<void()> finish;
};
