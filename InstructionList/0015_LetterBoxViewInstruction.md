# Technical Specification: Fixed Aspect Ratio Viewport Scaling & Virtual Canvas Pipeline

## 1. Architectural Core: The Virtual Canvas Concept
The scaling pipeline enforces a strict, immutable **Virtual Canvas** matching a 16:10 aspect ratio (e.g., a native logical coordinate space of $1600 \times 1000$ units). The core engine loop logic, entity positioning, and UI placements operate exclusively within this decoupled coordinate ecosystem. 

Rather than modifying element positions when the hardware display resolution shifts, the rendering pipeline treats the window as a volatile display target and leverages a dedicated projection matrix layer (`sf::View`). This maps the immutable virtual layout onto any physical fullscreen monitor configuration.

---

## 2. Absolute Coordinate Mapping Justification
By establishing a non-negotiable virtual resolution, the necessity for dynamic runtime asset anchoring across varying screen sizes is entirely neutralized. 
* **Immutable Anchors:** The center of the screen is definitively fixed (e.g., exactly at coordinate `(800, 500)`), and the boundaries are permanently constrained between `(0,0)` and `(1600, 1000)`.
* **Structural Benefits:** Layout designers can utilize absolute, constant coordinates for structural UI clusters (such as HUD placement, menu boundaries, and button positions). 
* **Encapsulation Bounds:** While element-to-screen placement is absolute, internal component-level math (such as centering text labels within a button box via local bounding dimensions) remains localized to maintain typographic integrity.

---

## 3. Aspect Ratio Adaptation Pipeline (Letterboxing & Pillarboxing)
When the game initializes in a fullscreen environment, the system detects the physical monitor's native dimensions and evaluates the aspect ratio discrepancy between the hardware and the 16:10 virtual canvas. To prevent structural stretching, warping, or asset clipping, the engine adjusts the Normalized Device Coordinates (NDC) rendering viewport.

### 3.1 Scenario A: Wide Display Discrepancy (Pillarboxing)
When the physical display configuration is wider than the target 16:10 ratio (such as standard 16:9 Full HD, 2K, 4K displays, or 21:9 UltraWide monitors):
* The engine calculates a fractional width multiplier based on the ratio mismatch.
* The viewport width is constrained to center the 16:10 canvas perfectly on the horizontal axis.
* The remaining unmapped rendering areas on the extreme left and right boundaries default to empty frame buffers, producing sharp, clean vertical black bars (Pillarbox effect).

### 3.2 Scenario B: Tall Display Discrepancy (Letterboxing)
When the physical display configuration is taller or more square than the target 16:10 ratio (such as legacy 4:3 or 5:4 monitors):
* The engine scales the viewport based on the vertical ratio restriction.
* The viewport height is constrained to center the 16:10 canvas perfectly on the vertical axis.
* The unmapped spaces at the top and bottom boundaries form horizontal black bars (Letterbox effect).

---

## 4. UI Framework Integration & Input Synchronization
Enforcing a fixed viewport projection introduces a decoupling layer that must be handled by the interaction management systems (`UIManager`).

* **Spatial Event Translation:** Raw mouse coordinates captured via OS window events reflect physical screen pixels. Before checking interactive states (`IsFocused()`), the engine routes these raw screen coordinates through a coordinate space inversion pass. This scales and shifts the mouse pointer into the virtual 16:10 canvas grid.
* **Input Isolation:** If the user moves the mouse into the black pillarbox/letterbox margin zones, the transformation math evaluates to coordinates lying outside the `(0,0)` to `(1600, 1000)` bounding grid. The `UIManager` immediately flags these coordinates as out-of-bounds, preventing accidental element activation or ghost focus states.
* **Rendering Uniformity:** Because the canvas projection is executed globally at the start of the scene-drawing pass, all active elements, trailing cursor animations, and UI panels draw natively inside the active matrix without requiring custom scaling code inside individual classes.

---

## Feedback & Implementation Plan (AI Review)

**Review Status: APPROVED**

The Virtual Canvas architecture is an incredibly clean and robust solution. It completely eliminates the complex, error-prone math of dynamically anchoring UI elements during resize events, and cleverly leverages SFML's native projection matrices (`sf::View`) for maximum hardware efficiency. The letterboxing/pillarboxing logic correctly accounts for all possible physical monitor configurations.

**Implementation Plan:**

1. **Virtual Space Definition:** I will define the immutable virtual canvas sizes (`constexpr float VIRTUAL_WIDTH = 1600.0f; constexpr float VIRTUAL_HEIGHT = 1000.0f;`) within a core configuration header.
2. **Aspect Ratio Math (The Viewport Calculator):** I will write a geometric scaling function to calculate the fractional `sf::FloatRect` for the viewport. If the window's aspect ratio exceeds `1.6f`, it applies Pillarboxing. If it falls below `1.6f`, it applies Letterboxing.
3. **Core Event Loop (`main.cpp`):**
   - Apply the calculated `sf::View` upon window creation.
   - Intercept `sf::Event::Resized` events to dynamically recalculate and apply the `sf::View` when the user resizes the window or toggles fullscreen.
4. **Input Boundary Enforcement:** I will update `UIManager::HandleEvent()` to instantly reject any mouse events where the `mapPixelToCoords` mapped virtual coordinate falls outside the `0-1600` or `0-1000` boundaries, guaranteeing input isolation.
5. **UI Anchoring Overhaul:** I will update `MainMenuState` and `TitleState` to replace their dynamic `width / 2.0f` math with absolute, immutable coordinates centered directly on `800, 500`.

Awaiting your approval to begin generating code!









# Technical Specification: Aspect Ratio Preservation & Viewport Centering

## 1. Virtual Canvas Initialization
The engine shall operate on an immutable **Virtual Canvas** with a fixed resolution of $1600 \times 1000$ (16:10 aspect ratio). This canvas acts as the sole coordinate space for all gameplay logic and UI rendering. The engine is strictly configured for fullscreen-only mode, meaning all viewport calculations occur exactly once during the initialization sequence. No runtime resizing support is required.

## 2. Viewport Centering (Physical Screen Alignment)
To ensure the game is perfectly centered on any physical display regardless of its native aspect ratio, the `sf::View` viewport must be mathematically calculated to maintain the 16:10 ratio with even distribution of "dead space" (black bars).

* **The Centering Math:** Upon initialization, compare the physical monitor's aspect ratio to the 16:10 target ratio. 
* **Calculation logic:** * If the physical screen is wider (Pillarbox): `viewWidth` is restricted, and `viewX` must be set to `(1.0f - viewWidth) / 2.0f`.
    * If the physical screen is taller (Letterbox): `viewHeight` is restricted, and `viewY` must be set to `(1.0f - viewHeight) / 2.0f`.
* **Alignment Requirement:** This logic guarantees that the virtual canvas is anchored exactly in the center of the hardware display, leaving equal margins on both opposing sides.

## 3. Camera Projection (Virtual Canvas Alignment)
Once the viewport is positioned on the physical screen, the `sf::View` camera itself must be locked to the center of the Virtual Canvas to ensure the game world is correctly framed.

* **Center Locking:** The `sf::View` center must be explicitly set to the geometric middle of the virtual coordinates: `sf::Vector2f(800.0f, 500.0f)`. 
* **Consistency:** This ensures that the world origin is not clamped to the top-left corner, but aligns perfectly with the center of the centered viewport.

## 4. Input Translation Pipeline
To handle user interaction within this scaled environment, input translation must be applied before any state logic (like `IsFocused()`) is processed.

* **Coordinate Mapping:** All mouse/input events must pass through `window.mapPixelToCoords()`. This function performs the necessary transformation from physical window pixels to the virtual $1600 \times 1000$ canvas coordinates.
* **Input Isolation:** The system must validate that the resulting virtual coordinates lie strictly within the $[0, 1600]$ and $[0, 1000]$ bounds. If the input is detected outside these bounds (i.e., within the pillarbox/letterbox margins), the event must be ignored to prevent ghost interactions.

## 5. Summary of Implementation
* **Initialization Phase:** Perform aspect ratio calculation, apply viewport offsets, and lock the camera center exactly once.
* **Update/Event Phase:** Apply coordinate mapping via `mapPixelToCoords` for all UI interactions.
* **Constraint:** Strictly enforce the virtual coordinate system; do not introduce dynamic scaling factors elsewhere in the rendering pipeline.