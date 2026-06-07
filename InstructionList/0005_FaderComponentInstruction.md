# Specification: FaderComponent

## 1. Overview
The `FaderComponent` is a modular logic component designed to calculate opacity transitions for UI elements and game entities. To ensure maximum flexibility and support for objects with varying natural transparencies (e.g., semi-transparent glass panels), this component **strictly prohibits hard-coded absolute alpha values**. Instead, it manages a normalized multiplier (`fadeRatio` from `0.0f` to `1.0f`), which the owning entity multiplies against its base alpha during rendering.

## 2. Directory Structure
* **File Location:** The component files (e.g., `FaderComponent.h` and `FaderComponent.cpp`) must be created within the designated component or UI utilities directory as defined in `README.md`.

## 3. Class Architecture & Properties
The component must be strictly state-driven and math-based. It does not handle direct rendering.

### 3.1. Internal State Variables
* `fadeRatio` (float): The current normalized multiplier, strictly clamped between `0.0f` and `1.0f`.
* `targetRatio` (float): The destination ratio (`0.0f` for invisible, `1.0f` for fully visible base alpha).
* `transitionSpeed` (float): The rate of change per second (calculated dynamically from a user-defined duration, preventing hard-coded increments).
* `isFading` (bool): Flag indicating if the transition logic is active.

### 3.2. Public Interface Methods
* `FaderComponent()`: Default constructor initializing `fadeRatio` to `1.0f` (fully visible by default) and `isFading` to `false`.
* `void SetDuration(float seconds)`: Dynamically sets the transition duration without hard-coding speed values.
* `void FadeIn()`: Sets `targetRatio` to `1.0f` and begins transition.
* `void FadeOut()`: Sets `targetRatio` to `0.0f` and begins transition.
* `void FadeToRatio(float ratio)`: Transitions to a specific fractional multiplier (e.g., `0.5f`).
* `void Update(float deltaTime)`: Core logic loop updating `fadeRatio` towards `targetRatio`.
* `float GetFadeRatio() const`: Returns the calculated `0.0f` to `1.0f` multiplier. The owning entity will use this to calculate its final render alpha (`FinalAlpha = BaseAlpha * GetFadeRatio()`).

## 4. Strict Non-Hardcoding Rules
* **No Magic Numbers:** Absolutely no hard-coded color bounds like `255` or `0` should exist in the core fading math. All calculations must use the normalized `0.0f` to `1.0f` space.
* **Reversibility:** Interrupted fades must reverse from the current `fadeRatio` smoothly, not snap to a default state.

## 5. Coding Conventions
All generated C++ code must strictly adhere to the following formatting rules:
1. **Allman Style:** Curly braces `{}` must always open on a new, dedicated line.
2. **Zero Spacing in Control Flow:** There must be **no space** between keywords (`if`, `for`, `while`) and their opening parenthesis `(`.