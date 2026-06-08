#pragma once

#include <SFML/Graphics.hpp>
#include "../Core/Resources/ResourceManager.h"
#include "../Core/Resources/ResourceIdentifiers.h"
#include "../Core/Resources/TextureAtlas.h"

class StateManager;

struct StateContext
{
    StateContext(StateManager& sm, ResourceManager<sf::Texture, std::string>& tex, ResourceManager<sf::Font, FontID>& fnt, TextureAtlas& atl, class AnimationLibrary& anims)
        : stateManager(sm)
        , textures(tex)
        , fonts(fnt)
        , atlas(atl)
        , animationLibrary(anims)
    {
    }

    StateManager& stateManager;
    ResourceManager<sf::Texture, std::string>& textures;
    ResourceManager<sf::Font, FontID>& fonts;
    TextureAtlas& atlas;
    class AnimationLibrary& animationLibrary;
};
