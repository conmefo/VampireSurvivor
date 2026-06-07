#pragma once

#include <SFML/Graphics.hpp>
#include "../Core/Resources/ResourceManager.h"
#include "../Core/Resources/ResourceIdentifiers.h"

class StateManager;

struct StateContext
{
    StateContext(StateManager& sm, ResourceManager<sf::Texture, TextureID>& tex, ResourceManager<sf::Font, FontID>& fnt)
        : stateManager(sm)
        , textures(tex)
        , fonts(fnt)
    {
    }

    StateManager& stateManager;
    ResourceManager<sf::Texture, TextureID>& textures;
    ResourceManager<sf::Font, FontID>& fonts;
};
