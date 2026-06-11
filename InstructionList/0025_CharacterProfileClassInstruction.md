**Context:** We are building the Character Selection Screen for a Vampire Survivors clone using SFML in C++. We strictly follow our `Core Manifesto` (Allman bracing style, Zero-space control flow, pure Data-Driven Design, and absolute Encapsulation).

**Current Task (Task 1): Implement the Character Data Model**

Please create a C++ class (e.g., `CharacterProfile`) that acts as a pure data container for character information. This class will hold the data parsed from our external JSON configuration files.

**Strict Requirements:**
1. **No Rendering Logic:** This class must strictly adhere to the Single Responsibility Principle. It must NOT include `sf::Texture`, `sf::Sprite`, or any SFML graphics modules. It is strictly a raw data schema.
2. **Core Fields:** Include the following essential properties:
   - `std::string id` (System identifier, e.g., "antonio").
   - `std::string name` (Display name).
   - `std::string description` (Passive skill description).
   - `std::string portraitTextureId` (String ID for the Asset Manager to resolve later).
3. **Data-Driven Stats:** To fully comply with our Data-Driven pillars, do NOT hard-code individual stat variables (e.g., `float maxHealth`). Instead, implement the stats container using a dynamic dictionary (e.g., `std::unordered_map<std::string, float> stats;`).
4. **Manifesto Compliance (Formatting & OOP):**
   - Apply absolute **Allman** bracing (braces on new lines).
   - Enforce the **Zero-Space Rule** for any control flow (e.g., `if()`, `for()`, `while()`).
   - Implement absolute encapsulation: All member variables MUST be `private`. Provide appropriate `public` getter methods (marked as `const` where applicable).

**Deliverable:**
Please output ONLY the C++ header declaration (`.h` or `.hpp` equivalent) for this class. Do not implement the JSON parser, data manager, or any UI components yet. Focus purely on this single task.


You can see the final result of Character Selection screen at `D:\GitHub\VampireSurvivor\characterSelection.png`

## Feedback / Implementation Plan

Based on the review of `CoreManifesto.md` and the `characterSelection.png` mockup, I propose the following optimizations to the `CharacterProfile` data model:

1. **Add `startingWeaponId`**: The UI explicitly displays the character's starting weapon icon next to their portrait (e.g., Antonio's Whip, Imelda's Magic Wand). The data model needs a `std::string startingWeaponId` to reference the correct weapon data/icon.
2. **Add `basePrice`**: The UI shows a coin balance (2569) and locked characters (Gennaro is silhouetted). An `int basePrice` should be included to support the store/unlock system.

**Proposed Structure (`CharacterProfile.h`):**
- **Members (Private):** `id`, `name`, `description`, `portraitTextureId`, `startingWeaponId`, `basePrice`, and `stats` (`std::unordered_map`).
- **Methods (Public):** Constructor and `const` getters for all fields. Allman bracing and zero-space rules will be strictly applied.

Please review and approve these optimizations so I can proceed with generating the C++ header.