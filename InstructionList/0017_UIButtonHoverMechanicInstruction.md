# Proposal: Hybrid State Management for `UIButton`

## 1. Context & The Problem
Currently, our `UIButton` component handles user interactions (Hover, Press) by adjusting the opacity/color of a single texture. However, after parsing the new asset `.json` files, we discovered that some UI elements have dedicated textures for specific states (e.g., separate images for `Hover` and `Press`). 

If we strictly switch to a "texture-only" approach, buttons without dedicated state textures will lose their interactive feedback. Therefore, we need a flexible solution.

## 2. The Hybrid Solution
I propose upgrading the `UIButton` to a **Hybrid Fallback System**. The core logic is simple: **Texture Priority with Opacity Fallback**. 

When the button state changes, the class will evaluate the available assets:
* **If a specific texture exists** for the current state (Hover/Press), the button will apply that texture and ensure opacity is set to 100%.
* **If a specific texture does NOT exist**, the button will fall back to using the default (Idle) texture and apply the opacity/color-tinting math to provide visual feedback.

## 3. OOP Architecture & Implementation Details

To keep the codebase clean, scalable, and strictly aligned with OOP principles, we should structure the component as follows:

### A. State Enumeration
Define an internal or public enum (e.g., `ButtonState`) containing `Idle`, `Hover`, and `Pressed`. This centralizes the state tracking and makes the event-handling logic much cleaner than using multiple boolean flags.

### B. Encapsulated Asset Storage
The `UIButton` class should hold optional references or pointers to the state textures (e.g., `IdleTexture`, `HoverTexture`, `PressTexture`). 
* The `IdleTexture` is strictly required.
* The `HoverTexture` and `PressTexture` are optional and should default to null/empty upon initialization.

### C. Flexible Setters (Abstraction)
Provide explicit setter methods (e.g., `setHoverTexture`, `setPressTexture`) so that the developer constructing the UI can inject these textures only if they exist in the parsed JSON data. If the developer doesn't call these setters, the button gracefully degrades to the opacity logic.

### D. Centralized Update Logic
The visual updating logic should be abstracted into a single private method (e.g., `updateVisuals()`). This method checks the current `ButtonState` enum and applies the `if(texture exists)` fallback logic internally. This hides the complexity from the rest of the game loop.

## 4. Why This is the Best Approach
* **High Reusability:** We can use the exact same `UIButton` class for major graphical buttons (Play, Quit) and minor utility buttons (Close, Next) without forcing the design team to create 3 separate sprites for every single minor button.
* **Bulletproof Scalability:** If an asset path in the JSON breaks or a texture fails to load, the button will simply fall back to the opacity effect. It will never break the UI or leave the player wondering if the game crashed.
* **Clean API:** The usage remains extremely simple for whoever is building the UI panels. Pass a texture, and it works. Pass three textures, and it still works flawlessly.

## Implementation Plan
* **Status:** Completed. 
* **Details:** We added `SetHoverTexture` and `SetPressTexture` to `UIButton`, storing reference to `TextureAtlas`. The underlying `UIPanel` background texture is automatically swapped during state transitions via `UpdateVisuals()`. If a texture is unassigned, it defaults to color tinting.