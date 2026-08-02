#include "WeaponFactory.h"
#include "Weapon.h"
#include "WhipWeapon.h"
#include "MagicMissileWeapon.h"
#include "FireballWeapon.h"
#include "RunetracerWeapon.h"
#include "KnifeWeapon.h"
#include "AxeWeapon.h"
#include "GarlicWeapon.h"
#include "SantaWaterWeapon.h"
#include "LightningRingWeapon.h"
#include <iostream>

WeaponFactory::WeaponFactory(WeaponDataManager& weaponData)
    : m_weaponData(weaponData)
{
}

std::unique_ptr<Weapon> WeaponFactory::Create(const std::string& weaponId) const
{
    const WeaponProfile& profile = m_weaponData.GetWeaponById(weaponId);
    if(profile.GetId() == "UNKNOWN")
    {
        std::cerr << "WeaponFactory: Unknown weapon id '" << weaponId << "'\n";
        return nullptr;
    }

    const std::vector<WeaponLevelDelta>& deltas = m_weaponData.GetLevelDeltas(weaponId);
    std::unique_ptr<Weapon> weapon;

    const std::string& bulletType = profile.GetBulletType();

    if(bulletType == "WHIP")
    {
        weapon = std::make_unique<WhipWeapon>(profile);
    }
    else if(bulletType == "MAGIC_MISSILE")
    {
        weapon = std::make_unique<MagicMissileWeapon>(profile);
    }
    else if(bulletType == "FIREBALL")
    {
        weapon = std::make_unique<FireballWeapon>(profile);
    }
    else if(bulletType == "KNIFE")
    {
        weapon = std::make_unique<KnifeWeapon>(profile);
    }
    else if(bulletType == "AXE")
    {
        weapon = std::make_unique<AxeWeapon>(profile);
    }
    else if(bulletType == "GARLIC")
    {
        weapon = std::make_unique<GarlicWeapon>(profile);
    }
    else if(bulletType == "DIAMOND")
    {
        weapon = std::make_unique<RunetracerWeapon>(profile);
    }
    else if(bulletType == "HOLYWATER")
    {
        weapon = std::make_unique<SantaWaterWeapon>(profile);
    }
    else if(bulletType == "LIGHTNING" || bulletType == "TP_ELEC1")
    {
        weapon = std::make_unique<LightningRingWeapon>(profile);
    }
    else
    {
        std::cerr << "WeaponFactory: No subclass for bullet type '" << bulletType << "'\n";
        return nullptr;
    }

    weapon->SetLevelDeltas(deltas);
    return weapon;
}
