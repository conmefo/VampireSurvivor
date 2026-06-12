#pragma once

#include <SFML/Graphics.hpp>
#include <string>

struct PowerUpData
{
    std::string id;
    std::string title;
    std::string description;
    std::string textureId;
    int currentLevel;
    int maxLevel;
    int price;
    
    // Cached rects for O(1) texture swapping without pointer indirection
    sf::IntRect iconRect;
    sf::IntRect emptyBoxRect;
    sf::IntRect filledBoxRect;
};
