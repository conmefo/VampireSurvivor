# Specification: Photosensitivity Warning State

## 1. Overview
This document outlines the requirements for the **Photosensitivity Warning State**. This state acts as an intermediary health and safety warning screen.
* **Preceding State:** Loading State (progress bar/asset loading completion).
* **Succeeding State:** Main Menu State / Title Screen State.

## 2. Visual Design & Layout
The screen displays a static text warning against a completely blank background. All UI elements must be strictly centered.

* **Background:** * Color: Solid Black (`#000000`).
    * Opaque, filling the entire viewport.
* **Header Text:**
    * Content: `PHOTOSENSITIVITY WARNING`
    * Color: Red (e.g., `#FF4444` or standard warning red).
    * Alignment: Center-aligned, positioned in the upper-middle section of the screen.
* **Body Text (Paragraph 1):**
    * Content: *"This game contains bright flashing lights. Please immediately stop playing and consult a doctor if you experience lightheadedness, altered vision, eye or face twitching, jerking or shaking of arms or legs, disorientation, confusion, or momentary loss of awareness."*
    * Color: White (`#FFFFFF`).
    * Alignment: Center-aligned, positioned below the header.
* **Body Text (Paragraph 2):**
    * Content: *"If you or any of your relatives have a history of seizures or epilepsy, consult a doctor before playing."*
    * Color: White (`#FFFFFF`).
    * Alignment: Center-aligned, positioned below the first paragraph with appropriate line spacing.
* **Prompt Text:**
    * Content: `PRESS TO START`
    * Color: White (`#FFFFFF`).
    * Alignment: Center-aligned, positioned near the bottom of the screen.

## 3. Animations & Transitions
The state relies entirely on alpha-blending (opacity) for its visual effects.

* **Entry Animation (Fade-In):**
    * Upon entering this state from the Loading State, the background initializes as solid black.
    * All text elements (Header, Body, Prompt) start at `Opacity = 0.0`.
    * The text elements gradually fade in to `Opacity = 1.0` over a defined duration (e.g., `1.5` to `2.0` seconds).
* **Exit Animation (Fade-Out):**
    * Triggered by user input (see Section 4).
    * All text elements fade out from `Opacity = 1.0` to `Opacity = 0.0` over a defined duration (e.g., `1.0` second).
    * Once the fade-out animation completes (`Opacity == 0.0`), the system triggers the state transition to the Main Menu/Title Screen.

## 4. Input Handling
During this state, the game must listen for any generic input to proceed.

* **Trigger:** Any Key Down (Keyboard), Any Button Press (Gamepad), or Screen Tap/Mouse Click.
* **Logic:** * Input is ignored *during* the initial fade-in animation to prevent accidental skipping before the warning is readable.
    * Once the fade-in is complete, any valid input will lock further input and immediately start the fade-out animation.

## 5. Technical Constraints & Standards

### 5.1. Directory Structure
All scripts, UI layouts, and localization files must strictly adhere to the project architecture defined in `README.md`.
* State logic should be placed in the designated `states/` or `screens/` directory.
* UI layouts (if using a visual editor or XML/JSON format) must be saved in the appropriate `ui/` or `layouts/` directory.

### 5.2. Coding Conventions
All source code written for this state must strictly follow the project's coding rules:
* **Allman Style:** Braces `{}` must be placed on the line immediately following the control statement.
* **No Spacing After Keywords:** There must be absolutely **no space** between control keywords (`if`, `for`, `while`, `switch`, etc.) and their opening parenthesis `(`.
