**Context:** We are developing a Vampire Survivors clone in C++ using SFML. We strictly adhere to our `Core Manifesto` (Allman bracing style, Zero-Space control flow rule, and absolute Encapsulation). We have successfully implemented the character card grid, and now we need to complete the last inner component of the main board.

**Current Task (Task 9): Implement the DetailPanel Class**

Please flesh out the complete, production-ready C++ code (both Header and Source files) for the `DetailPanel` class, replacing its previous stub. This panel sits at the bottom inside the `MainBoardPanel` and displays the selected character's full name, starting weapon icon, and dynamic passive description text.

**Architectural Guidelines & Requirements:**
1. **Visual Composition:** The panel must internalize and arrange:
   - An `sf::Text` for the character's full display name.
   - An `sf::Sprite` (and optional framing box) to render the character's starting weapon icon based on the profile's `startingWeaponId`.
   - An `sf::Text` wrapper supporting multi-line description string formatting for the passive skill.
2. **Interface for Selection Changes:**
   - Provide a public method: `void SetCharacterProfile(const CharacterProfile& profile)`.
   - When called, the panel must immediately refresh its internal text strings and query the asset manager (passed during initialization) to change the starting weapon sprite texture.
3. **Internal Layout Metrics (Anti-Hard-Coding):**
   - Relative positioning is mandatory. Define internal offsets using `private static constexpr float` metrics within the header file (e.g., `TEXT_PADDING_X`, `ICON_SIZE`, `SPACE_BETWEEN_ELEMENTS`).
   - The weapon icon box and text blocks must align perfectly with respect to the panel's own relative bounding box. No raw magic numbers allowed in the `.cpp` file layout math.
4. **Lifecycle Methods:**
   - Implement standard `Update(float deltaTime)`, `HandleEvent(const sf::Event& event)` (can be empty), and `Draw(sf::RenderWindow& window)`.
   - The `Draw` method must render the sub-panel background/borders first, followed by the text elements and the weapon icon sprite.
5. **Coding Standards Compliance (Non-Negotiable):**
   - **Allman Bracing Style:** Every opening and closing brace `{}` must reside on its own dedicated line, vertically aligned.
   - **Zero-Space Rule:** No spaces between control flow keywords and their opening parentheses (e.g., use `if()`, `for()`, `while()`).
   - **Encapsulation:** Keep all internal SFML graphical elements, text configurations, and dimensions strictly `private`.

**Deliverable:**
Provide the complete C++ implementation split into `DetailPanel.h` and `DetailPanel.cpp`. Ensure clean compilation with the existing layout system.