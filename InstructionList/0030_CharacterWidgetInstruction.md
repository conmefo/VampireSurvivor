**Context:** We are developing a Vampire Survivors clone in C++ using SFML. We strictly adhere to our `Core Manifesto` (Allman bracing style, Zero-Space control flow rule, and absolute Encapsulation). We have successfully implemented the `StatsPanel` and the high-level layout controller.

**Current Task (Task 7): Implement the CharacterCardWidget Class**

Please write the complete, production-ready C++ code (both Header and Source files) for the `CharacterCardWidget` class. This class represents a single interactive grid slot/button for an individual character within the character roster.

**Architectural Guidelines & Requirements:**
1. **State Management:** The widget must track its own visual state using an internal enum:
   ```cpp
   enum class CardState
   {
       Normal,
       Selected,
       Locked
   };
   ```
2. **Data Ingestion & Visual Setup:**
   - The constructor or initializer must accept a `const CharacterProfile&` and a boolean flag indicating whether the character is unlocked (queried from `PlayerProgressionManager`).
   - If the character is **Locked**, the card must render the character's sprite as a solid black silhouette (using `sf::Sprite::setColor(sf::Color::Black)`) as seen with Gennaro in the mockup.
   - If the character is **Selected**, the card must display a distinct thick gold selection border highlighted around its bounding frame (as seen with Pasqualina).
3. **Interactive Capabilities (Click Detection):**
   - Implement `HandleEvent(const sf::Event& event, const sf::RenderWindow& window)` to capture mouse interactions.
   - It must detect when the player hovers over or clicks inside the card's local bounding box.
   - **Decoupled Callbacks:** Do NOT hard-code any screen transition or selection logic inside this class. Instead, provide a mechanism to register a click callback, such as `std::function<void(const std::string& characterId)> m_onClickCallback;`. When clicked (and not locked), it executes this callback, passing its own character ID upwards to the parent container.
4. **Layout Composition:** Each card compositionally owns its visual components: an outer border texture/panel, the character inner sprite, and the tiny starting weapon preview icon drawn adjacent to it.
5. **Coding Standards Compliance (Non-Negotiable):**
   - **Allman Bracing:** Every single opening and closing brace `{}` must reside on its own dedicated line, vertically aligned.
   - **Zero-Space Rule:** No spaces between control flow keywords and their opening parentheses (e.g., use `if()`, `for()`, `while()`).
   - **Encapsulation:** All internal graphical dimensions, sprite textures, states, and callback handlers must be strictly `private`.

**Deliverable:**
Provide the complete C++ implementation split into `CharacterCardWidget.h` and `CharacterCardWidget.cpp`. Ensure seamless integration and compliance with the project's rendering flow.