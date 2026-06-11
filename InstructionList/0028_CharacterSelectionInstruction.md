**Context:** We are developing a Vampire Survivors clone in C++ using SFML. We strictly adhere to our `Core Manifesto` (Allman bracing style, Zero-Space control flow rule, and absolute Encapsulation). We have successfully established our static and dynamic data layers, as well as the top-level reusable `GoldDisplayWidget`.

**Current Task (Task 5): Implement the CharacterSelectionPanel Class (Layout Controller)**

Please write the complete, production-ready C++ code (both Header and Source files) for the `CharacterSelectionPanel` class. This class acts as the primary central container and layout manager for the Character Selection UI, visually representing the large blue-purple background board.

**Architectural Guidelines & Requirements:**
1. **Inheritance & Visual Parity:** This class must inherit from `UIPanel` to natively utilize the engine's `NineSliceComponent` rendering. It is responsible for drawing the main central frame of the character selection screen.
2. **Single Responsibility (Layout Management):** This panel does not manage game states or raw JSON loading. Its primary responsibility is **Spatial Orchestration**—defining, calculating, and enforcing the boundaries and relative positioning of its child zones to ensure layout consistency across different screen resolutions.
3. **Composition & Child Component Placeholders:** To prevent a monolithic class structure, the layout must be divided into three distinct sub-zones. For this task, declare private member instances (or unique pointers) for the following three placeholder classes (you may forward-declare or create empty stub headers for them to ensure compilation):
   - `StatsPanel` (Positioned on the left zone, handles character attributes).
   - `RosterGridPanel` (Positioned in the center/upper-right zone, handles the grid of character buttons).
   - `DetailPanel` (Positioned at the bottom zone, handles the character description and the "Confirm" button).
4. **Relative Positioning Logic (Anti-Hard-Coding):** - The panel must feature an initialization or resize method (e.g., `InitializeLayout(const sf::Vector2f& targetSize)`) that centers the main board on the screen.
   - The bounding boxes, positions, and offsets of the three child panels must be computed **relatively** using internal layout metrics (e.g., side padding, top/bottom margins, inner spacing constants). 
   - All layout metrics must be defined as `private static constexpr` constants within the header file (Tier 2 Constant Management). Absolutely no raw magic numbers are permitted within the positioning math.
5. **Event & Render Delegation:** - Implement standard lifecycle methods: `Update(float deltaTime)`, `HandleInput(const sf::Event& event)`, and `Draw(sf::RenderWindow& window)`.
   - The `Draw` method must first render the panel's own nine-slice background, and then sequentially delegate the `Draw` call to its child components (`StatsPanel`, `RosterGridPanel`, `DetailPanel`).
   - The `Update` and `HandleInput` methods must similarly cascade down to the active child components.
6. **Coding Standards Compliance (Non-Negotiable):**
   - **Allman Bracing Style:** Every opening and closing brace `{}` must reside on its own dedicated line, vertically aligned with its statement.
   - **Zero-Space Rule:** No spaces between control flow keywords and their opening parentheses (e.g., use `if()`, `for()`, `while()`, `switch()`).
   - **Encapsulation:** All structural dimensions, SFML layout fields, and child component instances must be strictly `private`.

**Deliverable:**
Provide the complete C++ implementation split into `CharacterSelectionPanel.h` and `CharacterSelectionPanel.cpp`. Do not include sample usage code or main logic. Focus entirely on a production-ready, perfectly aligned layout container class.