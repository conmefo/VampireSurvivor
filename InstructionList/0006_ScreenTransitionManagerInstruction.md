# Specification: ScreenTransitionManager

## 1. Overview
The `ScreenTransitionManager` is a global-level utility designed to orchestrate fluid visual transitions between different Game States. It strictly adheres to Object-Oriented Principles, ensuring decoupled architecture, flexibility, and the absolute elimination of hard-coded values. 

Its primary responsibilities are:
1. Masking the viewport during heavy asset loading.
2. Intercepting and blocking anomalous user inputs during state swaps.
3. Providing a highly convenient API for the `StateManager` to trigger seamless transitions.

## 2. Architecture & OOP Design
To prevent spaghetti code and maintain strict encapsulation, this manager **must not be a globally accessible Singleton**.
* **Ownership:** It should be composed within (owned by) the `StateManager` or the core `Engine` class.
* **Separation of Concerns:** It handles only the visual transition layer and input blocking. It does not manage state lifecycles directly; instead, it uses a callback/delegate mechanism to notify the `StateManager` when it is safe to swap states in the background.
* **Component Reuse:** It should internally utilize the established normalized `fadeRatio` logic (0.0 to 1.0) to calculate transition progress.

## 3. Core Properties (No Hard-coding)
* `transitionColor`: The color of the overlay. **Must not be hard-coded to black.** The system must allow dynamic color injection (e.g., black for normal transitions, white for flashbangs, red for game-over screens).
* `transitionDuration`: The time taken for one half of the transition (e.g., Fade to opaque). Passed dynamically.
* `transitionPhase`: An internal state machine tracking the current phase:
    * `Idle`: Fully transparent, inactive.
    * `FadingOut`: Interpolating opacity from 0.0 to 1.0. Inputs are blocked.
    * `Swapping`: Reached 1.0 opacity. Background operations (loading/unloading) occur here.
    * `FadingIn`: Interpolating opacity from 1.0 to 0.0. Inputs remain blocked.
* `onOpaqueCallback`: A function pointer or generic callback assigned by the caller, executed exactly when the screen becomes fully opaque.

## 4. Public Interface & Usability (Convenience)
The API must be extremely simple for the caller.

* `StartTransition(duration, targetColor, callbackFunction)`: The primary method. It initializes the fade-out, stores the color and duration, and saves the callback to be fired at the peak of the transition.
* `ProceedToFadeIn()`: Called explicitly by the `StateManager` once the new state has finished loading its heavy assets behind the opaque screen, signaling the manager to reveal the new state.
* `Update(deltaTime)`: Steps the internal logic and ratio forward based on the current phase.
* `Draw(renderTarget)`: Renders the full-screen geometric overlay (e.g., a rectangle matching the viewport size) using the `transitionColor` multiplied by the current normalized `fadeRatio`.
* `IsTransitioning() const`: Returns true if not in the `Idle` phase, used globally to block inputs.

## 5. Transition Lifecycle (The "V-Curve" Workflow)
1. **Trigger:** `StateManager` calls `StartTransition(...)`. Phase becomes `FadingOut`.
2. **Peak:** `fadeRatio` reaches `1.0`. Phase becomes `Swapping`. The manager executes the stored `onOpaqueCallback`.
3. **Background Work:** Inside the callback, `StateManager` safely destroys the old state and loads the new one.
4. **Resume:** `StateManager` finishes loading and calls `ProceedToFadeIn()`. Phase becomes `FadingIn`.
5. **Completion:** `fadeRatio` returns to `0.0`. Phase becomes `Idle`. Normal gameplay and inputs resume.