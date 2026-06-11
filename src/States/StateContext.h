#pragma once

#include <SFML/Graphics.hpp>
#include "../Core/Resources/ResourceManager.h"
#include "../Core/Resources/ResourceIdentifiers.h"
#include "../Core/Resources/TextureAtlas.h"

// Forward declarations
class StateManager;
class CharacterDataManager;
class PlayerProgressionManager;

struct StateContext
{
    StateContext(StateManager& sm, ResourceManager<sf::Texture, std::string>& tex, ResourceManager<sf::Font, FontID>& fnt, TextureAtlas& atl, class AnimationLibrary& anims, CharacterDataManager& cdm, PlayerProgressionManager& ppm)
        : stateManager(sm)
        , textures(tex)
        , fonts(fnt)
        , atlas(atl)
        , animationLibrary(anims)
        , characterData(cdm)
        , progressionData(ppm)
    {
    }

    StateManager& stateManager;
    ResourceManager<sf::Texture, std::string>& textures;
    ResourceManager<sf::Font, FontID>& fonts;
    TextureAtlas& atlas;
    class AnimationLibrary& animationLibrary;
    CharacterDataManager& characterData;
    PlayerProgressionManager& progressionData;
};
