**Context:** We are developing a Vampire Survivors clone in C++ using SFML. We strictly adhere to our `Core Manifesto` (Allman bracing style, Zero-Space control flow rule, and absolute Encapsulation). All underlying data managers, core layout panels (`StatsPanel`, `RosterGridPanel`, `DetailPanel`), and widgets are now fully operational.

**Current Task (Task 10): Implement Action Buttons and Final State Integration**

Please write the complete, production-ready C++ code to implement the standalone functional buttons ("BACK", "Confirm") within the view layout, and fully construct the master `CharacterSelectionScreen` state class to wire the entire system together.

**Architectural Guidelines & Requirements:**
1. **Button Core Integration:**
   - Instantiate and position the "BACK" button and the "Confirm" button within the `CharacterSelectionView` orchestrator. 
   - Position them relative to the viewport boundaries using `private static constexpr` layout markers to ensure pixel-perfect conversion across different resolutions.
   - The "Confirm" button's visibility or active state should dynamically change based on selection (e.g., disabled/greyed out if no character is selected or if the selected character is currently locked).

2. **Master State Construction (`CharacterSelectionScreen`):**
   - Implement the actual game state class inheriting from your project's base `State` class.
   - It must encapsulate the `CharacterSelectionView` using a lifetime manager (`std::unique_ptr`).
   - In its initialization phase, use Dependency Injection to query the static `CharacterDataManager` and dynamic `PlayerProgressionManager` from the overarching `StateContext` and pass them into the view.

3. **Decoupled Event Routing & Transitions:**
   - Wire lambda callbacks upward from the view into the `CharacterSelectionScreen` to handle state routing cleanly:
     - **On "BACK" Clicked:** The state must intercept this and command the state machine to pop the overlay, smoothly resuming the underlying `MainMenuState`.
     - **On "Confirm" Clicked:** The state must query the currently selected and validated character ID, save this selection context into the global game session, and trigger a state transition to launch the active gameplay phase.

4. **Coding Standards Compliance (Non-Negotiable):**
   - **Allman Bracing Style:** Every opening and closing brace `{}` must reside on its own dedicated line, vertically aligned.
   - **Zero-Space Rule:** No spaces between control flow keywords and their opening parentheses (e.g., use `if()`, `for()`, `while()`, `switch()`).
   - **Encapsulation:** Protect all internal states, button configurations, and core context views as private members.

**Deliverable:**
Provide the complete C++ implementation split into `CharacterSelectionScreen.h` and `CharacterSelectionScreen.cpp`, alongside any final layout adjustments inside `CharacterSelectionView`. Ensure the entire character selection lifecycle compiles flawlessly and is ready for execution.