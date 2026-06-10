#pragma once

#include "EnemyBase.h"

#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <vector>

struct EnemyAnimationFrame
{
    const sf::Texture* texture = nullptr;
    sf::IntRect rect;
};

struct EnemyAnimationDefinition
{
    float frameDuration = 0.1f;
    bool isLooping = true;
    std::vector<EnemyAnimationFrame> frames;
};

struct EnemyDefinition
{
    std::string id;
    std::string name;
    EnemyStats stats;
    float spriteScale = 1.0f;
    std::unordered_map<std::string, EnemyAnimationDefinition> animations;
};
