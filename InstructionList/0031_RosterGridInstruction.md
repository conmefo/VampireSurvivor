**Context:** We are developing a Vampire Survivors clone in C++ using SFML. We strictly adhere to our `Core Manifesto` (Allman bracing style, Zero-Space control flow rule, and absolute Encapsulation). We have successfully implemented the single `CharacterCardWidget`.

**Current Task (Task 8): Implement the RosterGridPanel Class**

Please write the complete, production-ready C++ code (both Header and Source files) for the `RosterGridPanel` class, replacing its previous stub. This class is responsible for arranging multiple `CharacterCardWidget` components into a clean, responsive grid layout inside the main board.

**Architectural Guidelines & Requirements:**
1. **Grid Layout Generation & Dynamic Math:**
   - The panel must dynamically calculate the positions of its child cards using loop indices. Do NOT hard-code absolute positions for individual cards.
   - Define strict grid constraints using `private static constexpr` values in the header (Tier 2 Constant Management), such as `MAX_COLUMNS = 4`, `CARD_SPACING_X = 15.0f`, `CARD_SPACING_Y = 15.0f`, and `GRID_START_OFFSET`.
   - Formula logic: For each card at index `i`, its local grid column is `i % MAX_COLUMNS` and its row is `i / MAX_COLUMNS`.

2. **Data Hydration:**
   - The panel must provide an initialization method that accepts references to both `CharacterDataManager` and `PlayerProgressionManager`.
   - It will iterate through all profiles from the manager, check their unlock status, instantiate a `std::unique_ptr<CharacterCardWidget>` for each, and store them in an internal `std::vector`.

3. **Single-Selection Logic & Mediator Pattern:**
   - Track the currently active selection using a `std::string m_selectedCharacterId` member variable.
   - When creating child cards, bind a custom callback lambda to each card's click event. 
   - When a card is clicked, the `RosterGridPanel` must intercept the event, loop through all other cards to reset their state to `Normal` (or `Locked`), set the clicked card's state to `Selected`, update `m_selectedCharacterId`, and propagate the selection changed event upwards to the parent `CharacterSelectionView` using a registered parent callback (`std::function<void(const std::string&)>`).

4. **Lifecycle Cascading:**
   - Standardize on `Update(float deltaTime)`, `HandleEvent(const sf::Event& event, const sf::RenderWindow& window)`, and `Draw(sf::RenderWindow& window)`.
   - These methods must seamlessly loop through the internal vector and cascade the calls down to every child card.

5. **Coding Standards Compliance (Non-Negotiable):**
   - **Allman Bracing Style:** Every opening and closing brace `{}` must reside on its own dedicated line, vertically aligned.
   - **Zero-Space Rule:** No spaces between control flow keywords and their opening parentheses (e.g., use `if()`, `for()`, `while()`).
   - **Encapsulation:** Keep the vector of child cards, constraints, and selection states strictly `private`.

**Deliverable:**
Provide the complete C++ implementation split into `RosterGridPanel.h` and `RosterGridPanel.cpp`. Ensure full compatibility with the existing UI view system and compile using cmake.
