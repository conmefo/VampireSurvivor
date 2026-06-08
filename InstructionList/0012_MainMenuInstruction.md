# Specification: Main Menu State Implementation

## 1. Architectural Overview
The `MainMenuState` represents the primary interactive hub. It inherits the persistent `Composite Background` and `Title` from the preceding `TitleScreenState` without reloading. Its primary responsibility is to instantiate, position, and manage a collection of `UIElement` objects (specifically `UIPanel` and `UIButton` instances) via an internal `UIManager`, and to handle the staggered fade-in presentation.

## 2. Component Integration & Scaling Strategy
* **Strict OOP Usage:** The state must exclusively use the newly established `UIPanel` (for static frames) and `UIButton` (for interactive elements). Raw sprites should only be used for the background and the selection cursors.
* **Nine-Slice Scaling:** All buttons and panels rely on their internal `NineSliceComponent`.
* **Global UI Scale:** Every UI element will be initialized with a configurable base scale multiplier (currently defaulting to `1.0f`). The engine will multiply the base texture dimensions by this scale to determine the `targetSize` before passing it to the `SetSize()` method of the `UIElement`.

## 3. Exact Layout & Relative Anchoring (Zero Hard-Coding)
Coordinates must be calculated dynamically based on the current window/viewport dimensions.

### 3.1. Top Bar Elements (HUD)
Anchored relative to the top edge of the screen, with consistent top-margin padding.
* **Cloud Sync Button (`UIButton`):** Square dimensions. Anchored to the Top-Left corner.
* **Platform Text ("STEAMRIP"):** Raw text or texture. Positioned dynamically to the immediate right of the Cloud Sync Button.
* **QUIT Button (`UIButton`):** Red texture. Anchored relatively between the Top-Left elements and the center axis.
* **Gold Display (`UIPanel`):** Black/Gold texture. Non-interactive. Anchored exactly at the Top-Center axis. Contains a child coin icon and numeric text aligned to its right inner bound.
* **OPTIONS Button (`UIButton`):** Blue texture. Anchored to the Top-Right corner.

### 3.2. Central Menu Cluster
This cluster is anchored relative to the center of the screen, shifted slightly downwards to sit beneath the game title.
* **START Button (`UIButton`):** Blue texture. The root anchor of the cluster, placed directly on the central vertical axis.
* **ONLINE Button (`UIButton`):** Blue texture. Placed strictly on the central vertical axis, positioned directly below the START button utilizing a predefined vertical padding variable.
* **POWER UP Button (`UIButton`):** Green texture. Placed strictly on the central vertical axis, positioned directly below the ONLINE button.
* **COLLECTION Button (`UIButton`):** Blue texture. Anchored relative to the POWER UP button, positioned to its immediate left.
* **UNLOCKS Button (`UIButton`):** Blue texture. Anchored relative to the POWER UP button, positioned to its immediate right.

### 3.3. Bottom Elements
* **Credits Button (`UIButton`):** Pill-shaped texture. Anchored to the Bottom-Center axis, near the bottom edge of the screen.
* **Version Text (`UIPanel` or Text):** Non-interactive string. Anchored strictly to the Bottom-Right corner.

### 3.5. Text Rendering & Dynamic Content (Addendum)
The UI assets provided are strictly blank background panels (e.g., the blue button, the gold display frame). All text content (such as "START", "OPTIONS", "STEAMRIP", or the dynamic gold value "77") must be rendered using dynamic text objects (e.g., `sf::Text`), not baked into textures.

* **Text Composition:** `UIPanel` (and its derivative `UIButton`) must optionally support holding a Text component. 
* **Dynamic Alignment:** If a button is assigned a text string (e.g., "START"), the class must calculate the local bounds of the text object and perfectly center it both horizontally and vertically within the `targetSize` of the 9-Slice background.
* **Update Logic:** If the text content changes (e.g., the gold value updates), the alignment mathematics must be recalculated immediately to ensure the text remains centered or properly anchored (e.g., right-aligned for the gold number).
* **Font Management:** Text objects should not load their own font files. They must receive a reference or pointer to a pre-loaded font managed by a central resource system to avoid memory duplication.

## 4. Staggered Reveal Animation
As established in the transition flow, the menu does not appear instantly.
* The state utilizes the `FaderComponent` embedded within each `UIElement`.
* Upon state initialization, a sequential, staggered delay is applied to the UI elements. They will fade from `0.0f` to `1.0f` opacity one by one (or row by row) to create a cascading entrance effect.

## 5. Cursor Management (External to Buttons)
The rotating selection arrows are strictly managed by the `MainMenuState`, completely decoupling them from the `UIButton` logic.
* **Instantiation:** The state holds two distinct cursor sprites (Left and Right) with their own frame-based animation logic.
* **Dynamic Snapping:** During the `Update` loop, the state polls the `UIManager` for the currently focused `UIButton`. If a button in the Central Menu Cluster is focused, the state retrieves its bounding box (`GetPosition()` and `GetSize()`) and calculates the exact `X, Y` coordinates to place the cursors flanking the button.
* **Visibility:** If a non-cluster button (like QUIT or OPTIONS) is focused, the cursors are temporarily hidden.

* Zero-space control flow syntax (`if(condition)`, `for(int i)`).

## Implementation Plan

The architecture outlined for the `MainMenuState` perfectly extends our established OOP UI framework, ensuring a highly responsive and resolution-independent interface.

To accomplish this systematically, I propose the following implementation phases:

### Phase 1: Text Integration in UIPanel
- **Text Member:** Add `sf::Text m_text;` to `UIPanel` (which `UIButton` inherits from).
- **Public API:** Add `void SetText(const std::string& text, const sf::Font& font, unsigned int charSize);`
- **Dynamic Alignment:** Implement an internal `CenterText()` method that queries `m_text.getLocalBounds()` and `m_size`, calculating the exact center origin. This method will automatically fire within `SetSize()` and `SetText()`.
- **Fader Support:** Ensure `UIPanel::Update(...)` cascades the `m_fader` alpha value to `m_text.setFillColor()` to keep text fading synchronized with the 9-Slice background.

### Phase 2: UIManager Creation
- The specification dictates that the state polls a `UIManager`. I will build a dedicated `UIManager.h/.cpp` class that:
  - Holds a container of `std::unique_ptr<UIElement>`.
  - Manages bulk `Update(dt)` and `Draw(window)`.
  - Routes `HandleEvent(event, window)` to all elements, allowing them to internally manage their own focus (`IsFocused()`) based on mouse/input coordinates.

### Phase 3: MainMenuState Assembly
- **Resource Loading:** Pre-register and load all necessary textures (e.g., Blue button, Green button, cursor animation frames) into the `ResourceManager` directly inside `LoadingState.cpp` during its simulated loading sequence. We will add new `TextureID`s for them.
- **Dynamic Anchoring:** In `MainMenuState::Init()`, utilize `sf::RenderWindow::getSize()` to calculate fluid coordinates.
  - *Example:* Center cluster starts at `(width / 2.0f, height * 0.4f)`.
- **Staggered Fader Delay:** Iterate through the `UIManager` elements upon state startup, assigning each a delayed `FadeIn()` to create the cascading reveal.

### Phase 4: Cursor Logic
- The cursors are animated using multiple texture frames to simulate 3D rotation along their horizontal axis.
- Add two `sf::Sprite` members to `MainMenuState` for the left and right cursors. The right cursor will reuse the left cursor's textures but its scale will be flipped horizontally (`setScale(-1.0f, 1.0f)`) to point inward.
- In `Update(dt)`, cycle the current sprite texture across the frames based on an animation timer.
- **Positioning:** Iterate directly through the central cluster `UIButton` instances. If `button->IsFocused()` returns true, retrieve its `GetPosition()` and `GetSize()` to precisely snap the cursor sprites to its left and right boundaries. Hide the cursors if no central cluster button is focused.

I await your approval to begin executing this plan!