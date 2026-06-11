**Context:** We are developing a Vampire Survivors clone in C++ using SFML. We strictly adhere to our `Core Manifesto` (Allman bracing style, Zero-Space control flow rule, and absolute Encapsulation). In Task 5, we set up the `CharacterSelectionView` layout controller and created a stub header for `StatsPanel`.

**Current Task (Task 6): Implement the StatsPanel Class**

Please flesh out the complete, production-ready C++ code (both Header and Source files) for the `StatsPanel` class, replacing the previous stub. This component renders the grey sidebar panel on the far left that lists all character attributes (Max Health, Recovery, Armor, Move Speed, etc.).

**Architectural Guidelines & Requirements:**
1. **Inheritance & Background:** `StatsPanel` must inherit from `UIPanel` to leverage the `NineSliceComponent` for drawing its grey background box and gold borders.
2. **Interface for Data Ingestion:**
   - Provide a public method: `void SetCharacterProfile(const CharacterProfile& profile)`.
   - When a profile is injected via this method, the panel must extract its data using `profile.GetStat(statName)` and update the corresponding UI text strings.
3. **Data-Driven Row Formatting:**
   - To avoid hard-coding individual `sf::Text` elements for every single stat (which violates SRP and is error-prone), store the names of the stats you want to display in an internal collection (e.g., a `std::vector<std::string>` containing keys like `"MaxHealth"`, `"MoveSpeed"`, `"Armor"`).
   - Loop through this collection to query values from the profile, format them into human-readable rows (e.g., adding `+` or `%` formatting depending on the attribute type), and arrange them vertically.
4. **Layout & Vertical Alignment Mechanics:**
   - Implement private layout constants (Tier 2 Class-Scoped Constants) like `ROW_PADDING`, `TEXT_SIZE`, and `START_Y_OFFSET`. 
   - Calculate row text positions dynamically within a loop based on their index to enforce perfect vertical listing. No hard-coded absolute Y-coordinates for individual text items.
5. **Lifecycle Methods:** Standardize on `Update(float deltaTime)`, `HandleEvent(const sf::Event& event)` (can be empty if no input is handled), and `Draw(sf::RenderWindow& window)`. The `Draw` method must render the background panel first, followed by the text rows.
6. **Coding Standards Compliance (Non-Negotiable):**
   - **Allman Bracing:** Every single opening and closing brace `{}` must reside on its own dedicated line, vertically aligned.
   - **Zero-Space Rule:** No spaces between control flow keywords and their opening parentheses (e.g., use `if()`, `for()`, `while()`).
   - **Encapsulation:** Keep all internal layout strings, text arrays, and asset bindings strictly `private`.

**Deliverable:**
Provide the complete C++ implementation for `StatsPanel.h` and `StatsPanel.cpp`. Ensure flawless compilation alongside the existing `CharacterSelectionView` architecture.