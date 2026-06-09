#pragma once

#include <SFML/Graphics.hpp>
#include <string>

struct PowerUpData
{
    std::string title;
    int currentLevel;
    int maxLevel;
    
    // Cached rects for O(1) texture swapping without pointer indirection
    sf::IntRect iconRect;
    sf::IntRect emptyBoxRect;
    sf::IntRect filledBoxRect;
};
