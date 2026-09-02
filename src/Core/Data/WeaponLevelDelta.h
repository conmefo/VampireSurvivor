#pragma once

#include <string>
#include <unordered_map>

// Value Object: immutable delta applied to weapon stats on each level-up.
// Populated from WEAPON_DATA.json level entries [1..N].
struct WeaponLevelDelta
{
    float power          = 0.0f;
    float area           = 0.0f;
    float speed          = 0.0f;
    float duration       = 0.0f; // seconds
    float hitBoxDelay    = 0.0f; // seconds
    float magnet         = 0.0f; // percentage ratio (e.g. 0.50 = +50%)
    int   amount         = 0;
    int   interval       = 0;    // ms
    int   repeatInterval = 0;    // ms
    int   penetrating    = 0;
    int   poolLimit      = 0;
    float knockback      = 0.0f;
    std::unordered_map<std::string, float> specialStats;

    std::string addEvolvedWeapon; // e.g. "BORA" — empty if no evolution at this level
};
