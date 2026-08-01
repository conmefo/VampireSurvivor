# Weekly Report

  

## General Information

  

*   **Group ID:** Group51

*   **Group Name:** Group51

*   **Project Name:** Vampire Survivors Clone

*   **Date range:** 2026-07-26 – 2026-08-01

  

## Tasks Completed This Week

  

**Vo Ba Thong - 25125036**

  

*   Designed the `WeaponLevelDelta` value object pattern and refactored `WeaponProfile` to dynamically load and merge multi-tier level stats from JSON data (`WEAPON_DATA.json`).

*   Implemented `WeaponFactory` (Factory Pattern) to centralize weapon instantiation and `IWeaponLevelObserver` to decouple weapon evolution signals from inventory logic.

*   Derived trigonometric radial targeting step algorithms and implemented quadratic emission rate scaling ($\text{Rate} \propto \text{Area}^2$) with fixed flame dimensions for Santa Water physics and visual density.

*   Engineered the Power-Up shop progression system featuring dynamic price markup calculations ($\text{Price} = (\text{Level} + 1) \times \text{BasePrice} + \text{TotalShopLevels} \times \text{Markup}$) and persistent save state serialization (`save_data.json`).

*   Created a 3-tier additive stat pool (`Might`, `Area`, `Duration`, `Cooldown`, `Amount`, `MoveSpeed`) calculated at stage start and applied dynamically to all active weapons during combat.

*   Designed interactive MAX level checkbox controls (`menu_checkbox_24_bg` + `yes16`), dynamic card tinting (`sf::Color(125, 125, 125)`), text bounding box centering math, and bold typography hierarchy (`courier_bold.ttf`).

  

**Ho Quang Minh - 25125060**

  

*   Optimized spatial grid collision detection bounds and tilemap rendering pipeline for seamless large-map navigation.

*   Implemented sound effect triggers for UI button interactions, power-up purchases, projectile impacts, and stage soundtrack transitions.

*   Refactored enemy pool memory pre-allocation and sprite animation batch updates to maintain smooth 60 FPS performance during heavy mob waves.

  

## AI Usage Declaration

  

This week, we utilized AI (Gemini) extensively as a pair-programming technical advisor and code refactoring assistant, focusing on establishing a clean, scalable Power-Up progression shop and integrating a multi-tiered stat multiplier system into our game loop without performance regressions.

  

**How we used AI:**

  

**Challenge 1: Multi-Tiered Stat Buff Pipeline & Shop Inflation Math**

  

1.  **Identifying the Problem:** Integrating persistent shop buffs into active combat weapons was challenging because applying multipliers in the wrong order or modifying base weapon profiles directly led to cumulative stat corruption during runs. Furthermore, shop item prices needed to scale dynamically based on total purchased upgrades across the entire store.

2.  **My Suggestion & Prompting:** I formulated a 3-tier decoupled architecture: Base Character Stats + Shop Power-Up Passives + In-Game Weapon Level Deltas. I defined the mathematical formula for price inflation ($\text{Price} = (\text{Level} + 1) \times \text{BasePrice} + \text{Markup}$) and specified that global passives must act as read-only multipliers on runtime weapon execution.

3.  **AI Assistance:** I prompted the AI to review my proposed mathematical model and check for potential control-flow side effects. The AI verified the mathematical correctness, helped write the clean C++ data structures for `PlayerProgressionManager`, and assisted in writing unit-safe stat getters (`GetMightMultiplier`, `GetAreaMultiplier`, `GetCooldownMultiplier`) that prevent divide-by-zero or negative cooldown errors.

  

**Challenge 2: Stateful Shop UI & Precise Text Alignment**

  

1.  **Identifying the Problem:** In the Power-Up Shop, maxed-out items required a custom checkbox interface (active vs. sealed toggle), while description text spans varied wildly in length, causing visual clipping against shop buttons.

2.  **My Suggestion & Prompting:** I designed the UI layout rules: text description bounding boxes must be dynamically calculated and centered relative to the detail container, maxed-out cards must replace the standard "Buy" button with an interactive checkbox sprite (`menu_checkbox_24_bg` + `yes16`), and unactive/sealed cards must receive a consistent dark gray background tint (`sf::Color(125, 125, 125)`).

3.  **AI Assistance:** The AI acted as an implementation assistant for layout geometry math. I specified the bounding box constraints, and the AI calculated the exact SFML origin offsets (`setOrigin(left + width/2, top + height/2)`) and text wrapping limits (`TextUtility::WrapText`), eliminating visual clipping and ensuring pixel-perfect layout alignment across all screen resolution scaling modes.

  

## Tasks Planned for Next Week

  

*   Implement in-game chest drop rewards and item selection UI overlay.

*   Add weapon evolution merging mechanics when specific weapon and passive item requirements are met.

*   Expand enemy wave database configurations and balance stage difficulty progression.

  

## Issues

  

*   **Issue:** Combining multiple stat sources (Shop upgrades, character base stats, and weapon level deltas) risked mutating raw weapon profile data, causing stat multipliers to stack compoundingly every frame.

*   **Resolution:** Addressed by establishing a strict read-only execution flow where base weapon profiles remain immutable, and effective combat stats ($\text{Base} \times \text{Multiplier}$) are computed transiently inside weapon attack loops (`Weapon::Update` & `SantaWaterWeapon::FireOne`).
