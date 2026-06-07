# Specification: UIButton (Interactive UI Layer)

## 1. Architectural Overview & Philosophy
The `UIButton` class is the interactive layer of the UI system. It inherits directly from `UIPanel` (and inherently `UIElement`). 

By inheriting from `UIPanel`, it automatically gains the ability to render 9-Slice scalable borders and handle structural layout changes. Its exclusive new responsibility is to handle user input (mouse, keyboard, or gamepad), manage internal interactive states, and broadcast events to external listeners.

## 2. Interactive State Machine
To provide proper visual feedback, the button operates on a strict internal state machine. It must track its current condition using an internal enum (e.g., `ButtonState`).

* **Normal (Idle):** The default state. The user is not interacting with the button.
* **Hovered / Focused:** The mouse cursor is over the button's bounding box, OR the user has navigated to this button using keyboard/gamepad directional inputs.
* **Pressed:** The user is actively holding down the primary action button (Left Mouse Click, or Enter/A button) while the element is focused.
* **Disabled:** The button is visible but ignores all inputs and visual feedback triggers (e.g., a locked menu option).

## 3. Visual & Aesthetic Feedback
A button must visually react to state changes without hard-coding specific colors.

* **Tinting / Shading:** The button should expose a mechanism to apply a color tint or multiplier based on the current state (e.g., slightly darker when `Pressed`, slightly brighter when `Hovered`).
* **Selection Cursors (External Dependency):** As specified in the Main Menu design, when the button enters the Hovered/Focused state, it acts as an anchor. The external system (e.g., the current State) will read the button's boundaries to dynamically position specific visual indicators like rotating cursor sprites. The button itself does not manage or draw these cursors; it only provides its bounding box coordinates.

## 4. Input Handling & Bounding Logic
The button requires a dedicated method to process input events, typically called during the update phase or an event-polling phase.

* **Hitbox Detection:** It must use its `position` and `size` (inherited from `UIElement`) to calculate an Axis-Aligned Bounding Box (AABB) to detect mouse intersections.
* **Focus Management:** It must expose methods (e.g., `SetFocus(bool)`) to allow an external `UIManager` to manually set its hover state. This is critical for keyboard/gamepad navigation, where the mouse coordinates are irrelevant.

## 5. Event Delegation (The Callback System)
The button must remain completely decoupled from game logic. It must not know what the "START" or "QUIT" actions do.

* **Callback Mechanism:** The class must utilize function pointers or functional wrappers (like `std::function` in C++) to store a generic callback action (`onClickAction`).
* **Trigger Condition:** The callback is executed strictly when the button transitions from the `Pressed` state back to the `Hovered` state (a full click release) while the cursor/focus remains inside the button's bounds.
* **Setter Interface:** The class must provide a public method (e.g., `SetOnClickCallback(...)`) allowing the instantiating State to inject the specific game logic into the button.

## 6. Adherence to Core Manifesto
Any C++ implementation of this specification must strictly obey the engine's formatting manifesto:
* Absolute data encapsulation (all state variables and callbacks are private).
* Strict Allman bracing.
* Zero-space control flow for all hit-detection and state-checking logic.

## Implementation Plan

The proposed `UIButton` design is highly modular and optimally decoupled. It fully respects the Single Responsibility Principle by deferring actual game logic to the external callback.

To successfully execute this:

### 1. NineSliceComponent & UIPanel Upgrades
- **Color Support:** Add `void SetColor(const sf::Color& color)` to `NineSliceComponent` so it can tint its internal `m_vertexArray`.
- **Panel Exposure:** Add `virtual void SetColor(const sf::Color& color)` to `UIPanel` to pass the color down to the `NineSliceComponent`.

### 2. UIButton Implementation
- Create `UIButton.h` and `UIButton.cpp`.
- **Inheritance:** `class UIButton : public UIPanel`
- **State Enum:** `enum class ButtonState { Normal, Hovered, Pressed, Disabled };`
- **Private State Variables:**
    - `ButtonState m_state;`
    - `sf::Color m_normalColor`, `m_hoverColor`, `m_pressedColor`, `m_disabledColor`;
    - `std::function<void()> m_onClickAction;`
    - `bool m_isFocused;`
- **Hitbox Logic:**
    - `bool Contains(const sf::Vector2f& point) const;`
- **Input & Focus:**
    - `void HandleEvent(const sf::Event& event, const sf::RenderWindow& window);`
    - `void SetFocus(bool focused);`
    - Internal state logic to handle transitions (e.g., `Normal` -> `Hovered` -> `Pressed` -> `Hovered` [TRIGGER CALLBACK]).
- **Visual Feedback:**
    - Override or provide internal logic to call `SetColor(...)` based on `m_state`.

I am ready to proceed once you confirm this plan!