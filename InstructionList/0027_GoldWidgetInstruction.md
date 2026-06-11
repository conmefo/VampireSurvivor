**Context:** We are developing a Vampire Survivors clone in C++ using SFML. We strictly follow our `Core Manifesto` (Allman bracing style, Zero-Space control flow rule, and absolute Encapsulation). We have already implemented the static data layer (`CharacterDataManager`) and the dynamic data layer (`PlayerProgressionManager`). 

Our overlay states (like the Character Selection Screen) use a semi-transparent dark background that dims the underlying state. To avoid the gold display being visually dimmed out, we are implementing it as a reusable foreground widget.

**Current Task (Task 4): Implement the GoldDisplayWidget Class**

Please write the complete, production-ready C++ code (both Header and Source files) for a reusable UI component named `GoldDisplayWidget`.

**Architectural Guidelines & Requirements:**
1. **Single Responsibility & Composition:** This class is solely responsible for rendering the gold currency box, its icon, and the text displaying the player's current balance. It does NOT manage player data itself.
2. **Dependency Injection (No Singletons):** To comply with the "Zero Singleton" rule, the widget must NOT fetch gold data from a global instance. Instead, pass a reference or pointer to the `PlayerProgressionManager` into its constructor or initialization method.
3. **Rendering & Positioning:**
   - The widget must have a `Draw(sf::RenderWindow& window)` method (or inherit from `sf::Drawable` if standard in the project).
   - It must accept a position parameter (e.g., `sf::Vector2f position` or `SetPosition(float x, float y)`) so it can be placed flexibly in different layouts (e.g., centered in MainMenuState but potentially aligned differently elsewhere).
4. **Dynamic Text Polling:** Inside its update or draw cycle, it must poll the current gold balance directly from the injected `PlayerProgressionManager` and update the `sf::Text` string dynamically, ensuring that any gold spent/earned while the overlay is active reflects instantly on screen.
5. **Coding Standards Compliance (Non-Negotiable):**
   - **Allman Bracing Style:** Every opening and closing brace `{}` must reside on its own dedicated line, vertically aligned.
   - **Zero-Space Rule:** No spaces between control flow keywords and their opening parentheses (e.g., use `if()`, `for()`, `while()`).
   - **Encapsulation:** Keep all SFML graphical objects (`sf::Sprite`, `sf::Text`, `sf::RectangleShape`) and data bindings strictly `private`.

**Deliverable:**
Provide the complete C++ implementation split into `GoldDisplayWidget.h` and `GoldDisplayWidget.cpp`. Ensure absolute conformity with the manifesto formatting standards.

- **Visual & Positional Identity (Pixel-Perfect Alignment):** The widget's visual appearance—including its background box, gold icon texture, font type, size, and coloring—must look **exactly identical** to the original gold display previously rendered in `MainMenuState`. Furthermore, when instantiated within the `CharacterSelectionScreen` overlay, it must be hardconfigured or placed at the **exact same screen coordinates** (centered horizontally at the top, kẹp giữa/perfectly aligned between the "BACK" and "Enter Co-op" buttons as shown in the mockup image). This ensures a seamless, flicker-free visual transition when the overlay opens.

## Feedback / Implementation Plan

Based on the requirements and our `CoreManifesto.md`, I propose the following optimizations for `GoldDisplayWidget`:

1. **Inherit from `UIPanel` (Composition & Reusability):** Instead of reinventing the background rendering, `GoldDisplayWidget` will inherit from the engine's `UIPanel` class. This automatically hooks it into the `NineSliceComponent` architecture, ensuring pixel-perfect visual parity with the rest of the UI without duplicating draw logic.
2. **Text Polling Optimization (Dirty Flag):** Calling `std::to_string(gold)` and recalculating text alignment every single frame is CPU-inefficient. I will add an `int m_lastKnownGold = -1;` cache. The widget will poll the `PlayerProgressionManager` every frame, but will *only* update the `sf::Text` string and re-center the layout if the value has actually changed.
3. **Asset Injection:** To render the gold icon and font properly without Singletons, the widget constructor will accept the `TextureAtlas` (or `ResourceManager`/`StateContext`) alongside the `PlayerProgressionManager`.
4. **Hard-coded Default Coordinates:** As requested, I will provide a static configuration block or constant within the class to enforce the exact "center-top" screen coordinates, ensuring the flicker-free transition over the `MainMenuState`.

Please review and approve these optimizations so I can proceed with writing `GoldDisplayWidget.h` and `GoldDisplayWidget.cpp`.