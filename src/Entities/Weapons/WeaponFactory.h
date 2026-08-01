#pragma once

#include <memory>
#include <string>
#include "../../Core/Data/WeaponDataManager.h"

class Weapon;

// Factory responsible for constructing Weapon subclass instances by weapon ID.
// Centralizes the if/else dispatch that was previously in GameState.
class WeaponFactory
{
public:
    explicit WeaponFactory(WeaponDataManager& weaponData);

    // Creates the correct Weapon subclass for the given weapon id.
    // Returns nullptr if the id is unknown.
    std::unique_ptr<Weapon> Create(const std::string& weaponId) const;

private:
    WeaponDataManager& m_weaponData;
};
