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
#include "SongOfManaWeapon.h"
#include "../Items/AttractorbItem.h"
#include "../../Core/Audio/AudioIdentifiers.h"
#include <iostream>
#include <unordered_map>
#include <unordered_set>

namespace {
// Maps bullet type strings to their firing SFX IDs.
SfxID GetFireSfxForBulletType(const std::string& bulletType)
{
    static const std::unordered_map<std::string, SfxID> sfxMap = {
        {"WHIP", SfxID::WhipAttack},
        {"MAGIC_MISSILE", SfxID::MagicMissileFire},
        {"FIREBALL", SfxID::FireballFire},
        {"KNIFE", SfxID::KnifeFire},
        {"AXE", SfxID::AxeFire},
        {"GARLIC", SfxID::GarlicPulse},
        {"DIAMOND", SfxID::MagicMissileFire}, // Runetracer reuses missile sound
        {"HOLYWATER", SfxID::SantaWaterThrow},
        {"LIGHTNING", SfxID::LightningStrike},
        {"TP_ELEC1", SfxID::LightningStrike},
        {"SONG", SfxID::SongOfManaPulse},
        {"MANNAGGIA", SfxID::SongOfManaPulse}
    };

    auto it = sfxMap.find(bulletType);
    return (it != sfxMap.end()) ? it->second : SfxID::None;
}
} // namespace

WeaponFactory::WeaponFactory(WeaponDataManager& weaponData)
    : m_weaponData(weaponData)
{
}

bool WeaponFactory::SupportsBulletType(const std::string& bulletType)
{
    static const std::unordered_set<std::string> supportedTypes = {
        "WHIP", "MAGIC_MISSILE", "FIREBALL", "KNIFE", "AXE", "GARLIC",
        "DIAMOND", "HOLYWATER", "LIGHTNING", "TP_ELEC1", "SONG",
        "MANNAGGIA", "MAGNET"
    };
    return supportedTypes.find(bulletType) != supportedTypes.end();
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

    if(!SupportsBulletType(bulletType))
    {
        std::cerr << "WeaponFactory: No subclass for bullet type '" << bulletType << "'\n";
        return nullptr;
    }

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
    else if(bulletType == "SONG" || bulletType == "MANNAGGIA")
    {
        weapon = std::make_unique<SongOfManaWeapon>(profile);
    }
    else if(bulletType == "MAGNET")
    {
        weapon = std::make_unique<AttractorbItem>(profile);
    }
    weapon->SetLevelDeltas(deltas);
    weapon->SetFireSfx(GetFireSfxForBulletType(bulletType));
    return weapon;
}
