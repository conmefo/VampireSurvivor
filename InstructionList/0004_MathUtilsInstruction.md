# Specification: Core Math & Animation Utilities

## 1. Overview
This specification outlines the architecture and requirements for the **Core Math & Animation Utilities** module within the **antigravity** engine. This module provides essential mathematical operations, easing functions, and physics-based interpolation tools necessary for UI transitions, entity movement, and overall "game feel" (juice).

The module is strictly divided into two distinct architectural patterns:
1.  **Stateless Utilities:** Pure mathematical functions that take inputs and return outputs without holding any internal state.
2.  **Stateful Components:** Objects or components that manage time, state, and apply the stateless utilities over multiple frames.

## 2. Directory Structure
All implementations must strictly adhere to the project architecture defined in the root `README.md`. 
*   **Stateless Math/Easing Functions:** Must be placed in the designated utility directory (e.g., `src/utils/` or `src/core/math/`).
*   **Stateful Animation Components:** Must be placed in the component or animation directory (e.g., `src/components/` or `src/core/animation/`).

## 3. Feature Requirements

### 3.1. Stateless Utilities (`MathUtils` Namespace / Static Class)
This module must provide purely functional, static methods.

*   **Linear Interpolation (Lerp):**
    *   `Lerp(float a, float b, float t)`: Standard interpolation between two floats.
    *   `LerpVector(Vector2 a, Vector2 b, float t)`: Interpolation for 2D coordinates.
    *   `LerpColor(Color a, Color b, float t)`: Interpolation for color blending (useful for damage flashes, screen tinting).
*   **Easing Functions (Tweening):**
    *   Must accept a normalized time variable `t` (0.0 to 1.0) and return the eased value.
    *   **Required Variations:** `EaseIn`, `EaseOut`, and `EaseInOut`.
    *   **Required Curves:** `Sine`, `Quad`, `Cubic`, `Quart`, `Quint`, `Expo`, `Circ`, `Back`, `Elastic`, and `Bounce`.
*   **Helper Math:**
    *   `Clamp(value, min, max)`: Restricts a value within a given range.
    *   `Normalize(value, min, max)`: Converts a value to a 0.0 - 1.0 range.

### 3.2. Stateful Components (Animation Controllers)
These classes manage the lifecycle of an animation, tracking the elapsed time (`deltaTime`) and updating variables accordingly.

#### 3.2.1. Tweener Class
*   **Purpose:** Manages a single or multiple value transitions over a fixed duration using the Easing functions defined in `MathUtils`.
*   **Features:**
    *   Start, Pause, Resume, and Stop controls.
    *   Callbacks or events upon completion (`OnTweenComplete`).
    *   Ability to chain or loop tweens.

#### 3.2.2. Spring Class (Physics-based Animation)
*   **Purpose:** Simulates a damped harmonic oscillator for dynamic, physics-based interpolations (e.g., floating combat text, UI pop-ups, knockback effects).
*   **Properties:**
    *   `Tension` (Stiffness of the spring).
    *   `Damping` (Resistance/Friction to stop the bouncing).
    *   `TargetValue` (The resting position the spring aims for).
    *   `CurrentVelocity` (The ongoing momentum of the value).
*   **Behavior:** Unlike fixed-duration tweens, the Spring continues to update based on physics calculations until the velocity drops below a negligible threshold and the value reaches the target.

## 4. Coding Conventions & Standards
All code implemented for this module must strictly follow the project's formatting rules:

*   **Allman Style Bracing:** Opening and closing braces `{}` must always be on a new line.
*   **Strict Spacing Rule:** There must be **zero spaces** between control flow keywords (`if`, `for`, `while`, `switch`) and their opening parenthesis `(`.

