#pragma once

#include <string>

// Observer interface — notified by Weapon when evolution is triggered.
// Implemented by WeaponInventory.
class IWeaponLevelObserver
{
public:
    virtual ~IWeaponLevelObserver() = default;
    virtual void OnWeaponEvolution(const std::string& evolvedWeaponId) = 0;
};
