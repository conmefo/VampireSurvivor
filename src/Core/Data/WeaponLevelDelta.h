#pragma once

#include <string>

// Value Object: immutable delta applied to weapon stats on each level-up.
// Populated from WEAPON_DATA.json level entries [1..N].
struct WeaponLevelDelta
{
    float power    = 0.0f;
    float area     = 0.0f;
    float speed    = 0.0f;
    float duration = 0.0f; // seconds
    int   amount   = 0;

    std::string addEvolvedWeapon; // e.g. "BORA" — empty if no evolution at this level
};
