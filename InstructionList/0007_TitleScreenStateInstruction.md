# Specification: Title Screen State & Pixelation Reveal Effect

## 1. Overview
This document details the transition into the **Title Screen State** (immediately following the Photosensitivity Warning State). The state introduces the main background and title via a dynamic, shader-driven pixelation reveal, followed by a staggered, cascading fade-in of interactive menu elements.

## 2. Visual Elements & Assets
* **Composite Background:** A single pre-rendered texture passed dynamically into the state. This texture contains both the raw background (`background.png`) and the game title (`title.png`) pre-positioned in the upper-center area.
* **Menu Items:** An array of three distinct textures (e.g., menu buttons or interactive prompts) positioned horizontally across the screen.

## 3. The Pixelation Fragment Shader
The core visual hook is a fragment shader that pixelates the `Composite Background`.

### 3.1. Shader Logic
* The shader requires a uniform variable representing the current pixel block size (e.g., `u_pixelSize`).
* It samples the original texture by snapping the current UV coordinates to the nearest multiple of `u_pixelSize`, effectively grouping physical pixels into large, solid-colored mosaic blocks.

### 3.2. Transition Animation
* **Initial State:** The state begins with `currentPixelSize` set to a large maximum value (e.g., `64.0f`), abstracting the image into unrecognizable large blocks.
* **Interpolation:** Over a set duration, `currentPixelSize` interpolates downwards towards `1.0f`.
* **The Threshold Snap:** To optimize rendering and ensure pixel-perfect clarity, once `currentPixelSize` drops below a predefined threshold (e.g., `PIXEL_SNAP_THRESHOLD = 1.5f`), the shader effect is completely bypassed. The engine stops applying the fragment shader and renders the `Composite Background` normally.

## 4. Staggered UI Reveal (Cascading Fade)
As the pixelation clears up, the three menu items will appear sequentially from left to right.

### 4.1. Reveal Trigger
* The UI reveal does not wait for the pixelation to finish entirely. It is triggered when `currentPixelSize` reaches a specific "clarity threshold" (e.g., when the background becomes recognizable).

### 4.2. Staggered Fading Logic
* The system utilizes the established `FaderComponent` for each of the three textures.
* **Delay Calculation:** To achieve the left-to-right cascading effect, a staggered delay is applied to each fader based on its index.
    * Texture 0 Delay: `0.0f` seconds.
    * Texture 1 Delay: `STAGGER_INTERVAL` (e.g., `0.2f` seconds).
    * Texture 2 Delay: `STAGGER_INTERVAL * 2`.
* Once the individual delay concludes, the `FaderComponent` interpolates the texture's alpha from `0.0f` to `1.0f`.



## 5. Strict Coding Conventions
As dictated by the engine's Core Manifesto, all C++ implementation for this state must strictly adhere to the project's formatting rules:

1. **Allman Style Bracing:** All braces `{}` must open on a new line.
2. **Zero-Space Control Flow:** No spaces between control keywords and parentheses.
3. **No Hard-Coding:** All timing values, pixel sizes, and thresholds must be handled as variables or class-scoped constants.


### 3.3. Implementation Reference (External Source)
* **Source Material:** The exact GLSL fragment shader code and the mathematical implementation for this pixelation effect have already been prototyped and validated in an external testing environment.
* **Action Required:** The system/developer must reference the local project located at `D:\SFML_test`. Extract the relevant shader logic from this directory and adapt it to fit the **antigravity** engine's shader pipeline. Do not attempt to reinvent the shader math; ensure visual parity with the existing prototype.


### 4. The "Press to Start" Phase
As the pixelation effect nears completion and the `Composite Background` resolves, the system transitions into an idle input-waiting phase.

* **Trigger:** When `currentPixelSize` drops below the predefined clarity threshold, a "PRESS TO START" text prompt is activated at the bottom of the screen.
* **Blinking Animation:** The text utilizes the `FaderComponent` to continuously alternate its alpha value between `0.0f` and `1.0f` (Ping-Pong effect), creating a smooth, rhythmic blinking without hard-coded state toggles.
* **Input Unlocking:** During the active pixelation reveal, all generic user inputs are strictly ignored. Input handling is only enabled once the "PRESS TO START" prompt begins blinking.

### 5. State Transition & Input Handling
* **Interaction:** Once the system is listening for input, any valid key press, mouse click, or gamepad button down will trigger the progression.
* **Exit Logic:** 1. Immediately lock all further inputs.
    2. Halt the "PRESS TO START" blinking animation (either hide it or fade it out).
    3. Trigger the `ScreenTransitionManager` (or internal StateManager logic) to seamlessly transition into the succeeding state (e.g., `MainMenuState`), where the interactive menu buttons will perform their staggered cascading reveal.