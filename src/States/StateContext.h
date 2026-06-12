#pragma once

#include <SFML/Graphics.hpp>
#include "../Core/Resources/ResourceManager.h"
#include "../Core/Resources/ResourceIdentifiers.h"
#include "../Core/Resources/TextureAtlas.h"

// Forward declarations
class StateManager;
class CharacterDataManager;
class PlayerProgressionManager;
class WeaponDataManager;
class PowerUpDataManager;

struct StateContext
{
    StateContext(StateManager& sm, ResourceManager<sf::Texture, std::string>& tex, ResourceManager<sf::Font, FontID>& fnt, TextureAtlas& atl, class AnimationLibrary& anims, CharacterDataManager& cdm, PlayerProgressionManager& ppm, WeaponDataManager& wdm, PowerUpDataManager& pudm)
        : stateManager(sm)
        , textures(tex)
        , fonts(fnt)
        , atlas(atl)
        , animationLibrary(anims)
        , characterData(cdm)
        , progressionData(ppm)
        , weaponData(wdm)
        , powerUpData(pudm)
    {
    }

    StateManager& stateManager;
    ResourceManager<sf::Texture, std::string>& textures;
    ResourceManager<sf::Font, FontID>& fonts;
    TextureAtlas& atlas;
    class AnimationLibrary& animationLibrary;
    CharacterDataManager& characterData;
    PlayerProgressionManager& progressionData;
    WeaponDataManager& weaponData;
    PowerUpDataManager& powerUpData;
};
