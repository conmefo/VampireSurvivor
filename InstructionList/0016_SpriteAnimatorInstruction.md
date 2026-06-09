# Technical Specification: Sprite Animator System & Hierarchical Animation Library

## 1. Architectural Overview: The Projector Pattern
To achieve a robust, scalable animation system that decoupling visual presentation from game logic, we employ the "Projector Pattern" based on **Composition**. Instead of entities (like Monsters) carrying their own complex rendering logic, each interactive entity possesses a `SpriteAnimator` component. This component acts as a "film projector," strictly responsible for cycling through visual data based on time, while remaining entirely agnostic of the entity's underlying game logic.

---

## 2. The Data Layer: Hierarchical Animation Library
The library acts as a centralized, hierarchical "Archival System" (The Drawer System). It prevents data redundancy and ensures that animation metadata is loaded into memory only when required.

### 2.1 The Two-Tiered Data Structure
To manage a high volume of diverse entities (Goblin, Orc, Boss, etc.), the library implements a nested mapping strategy. This eliminates "flat list" collisions and allows for clean categorization.
* **Tier 1 (Namespace/Species):** The top-level key corresponds to the entity type (e.g., "Orc", "Goblin"). This functions as an isolated memory "drawer."
* **Tier 2 (State/Action):** The second-level key corresponds to the animation state (e.g., "Walk", "Attack", "Die").
* **Storage:** The underlying data is `AnimationData`, an immutable object containing a sequence of `sf::IntRect` coordinates mapped from the global Texture Atlas, frame duration timings, and looping metadata.

### 2.2 Memory Management Strategy
* **Initialization:** The system is data-driven, parsing external configuration files (e.g., JSON) to populate the library.
* **Lazy Loading:** For performance, the library supports deferred loading. Asset files for a specific species (e.g., "Dragon") are only read into memory when the first instance of that species is instantiated in the game world, keeping the footprint minimal during lighter scenes.

---

## 3. The Functional Layer: Sprite Animator Component
The `SpriteAnimator` is a lightweight, stateful component attached to every entity that requires rendering. It does not store the animation data itself; it merely holds a "lens" (a pointer or reference) pointing to the specific `AnimationData` currently being projected.

### 3.1 Statefulness & Playback Logic
The component maintains the current playback status, independent of other instances of the same species:
* **Playback Tracking:** Tracks the accumulated elapsed time to calculate the current frame index, ensuring smooth synchronization regardless of fluctuating framerates.
* **Looping Control:** A boolean flag dictates whether the animation sequence should restart upon reaching the final frame or freeze on the last image (crucial for "Death" or "Single-trigger" animations).
* **State Transition API:** Entities trigger visual changes by sending a command (e.g., "Play this state"). The animator immediately switches its internal reference to the new animation sequence, resets the playback timer to zero, and begins the new projection.

---

## 4. Operational Lifecycle: The Interaction Pipeline
The system operates through a continuous, decoupled interaction loop:

1.  **Triggering:** The Entity (Monster) logic determines a state change (e.g., Health = 0). The Entity commands its attached `SpriteAnimator` to "Switch to Death State."
2.  **Retrieval:** The `SpriteAnimator` requests the specific `AnimationData` from the `AnimationLibrary` using the predefined Species and State keys.
3.  **Projection:** During the `Update` phase, the `SpriteAnimator` consumes `DeltaTime` to advance its internal frame counter. It calculates the correct `sf::IntRect` from the sequence and updates the target `sf::Sprite`.
4.  **Reporting:** Once the sequence concludes (and if it is non-looping), the animator can optionally signal completion back to the Entity, enabling the Entity to finalize its own lifecycle (e.g., removing the object from the game world or transitioning to a "Corpse" state).

---

## 5. Architectural Advantages
* **Memory Optimization (Flyweight Pattern):** Thousands of entities share the exact same `AnimationData` instances in the Library. Only the lightweight state (current frame, timer) is duplicated per entity.
* **Open/Closed Principle:** The engine's core code remains untouched when adding new monsters. To introduce a new creature, one only needs to add a new asset entry in the filesystem and the corresponding JSON configuration.
* **Decoupling:** The renderer does not need to know if a character is a "Goblin" or an "Orc." It only interacts with the `SpriteAnimator` interface, which is uniform across the entire engine.
* **Data-Driven Workflow:** Because definitions are stored in external files, visual adjustments to animation speeds or frame sequences can be performed without recompiling the project, significantly accelerating the balancing and tuning phase of development.


# Implementation Directive: Sprite Animator System

## 1. Architectural Foundation
* **Data-Driven Decoupling:** The `animations.json` must NOT contain raw pixel coordinates (`x, y, w, h`). Instead, it must store **Reference Keys** (e.g., `"orc_walk_01"`). The `AnimationLibrary` must query the `TextureAtlas` using these keys to resolve the final `sf::IntRect`. This ensures that if the Atlas layout changes, we only update the Atlas map, not the animation sequences.
* **Component-based Animation:** Follow the "Projector Pattern." `SpriteAnimator` is a component. `AnimationLibrary` is the data provider. The Entity is the controller.

## 2. Strict Code Style Guidelines
Every line of code generated must adhere to these two non-negotiable rules:
* **Allman-style Braces:** Opening braces `{` must always be on a new line.
* **No-Space Keyword Formatting:** Strictly remove spaces after keywords (e.g., use `if()`, `for()`, `while()`, `switch()`, `function()` instead of `if ()`).

## 3. Implementation Logic Requirements
* **Defensive Lookups:** When `AnimationLibrary` resolves keys via `TextureAtlas`, if a key is missing or invalid, the system must log a warning and return a default (empty) `IntRect` instead of crashing.
* **Callback Safety:** Ensure the `std::function<void()>` completion callback is checked for null before invocation to prevent illegal access errors.
* **Memory Ownership:** `AnimationLibrary` owns the `AnimationData`. `SpriteAnimator` holds a raw `const*` to the data. Ensure no lifecycle issues (dangling pointers) occur during State destruction.
* **Injection Principle:** No singletons. All libraries must be passed through `StateContext` or dependency injection to maintain testability and clean architecture.

## 4. JSON Schema Proposal
The structure of `animations.json` should mirror this hierarchy to maintain the "Drawer System":
```json
{
    "Orc": {
        "Walk": {
            "frameDuration": 0.1,
            "isLooping": true,
            "frames": ["orc_walk_01", "orc_walk_02", "orc_walk_03"]
        }
    }
}