# Task 2.3: Implement the Flyweight `UIScrollbar` with Vertical 3-Slice and Drag-Drop Sync

## Objective
Implement the final visual and interactive sub-component for our scroll system: the `UIScrollbar`. This component will render a vertical scrollbar using the provided pixel-art texture coordinates, handle direct mouse drag-and-drop mechanics, and establish a bi-directional sync loop with the `UIScrollView` implemented in Task 2.2.

## 1. Asset Specifications & Texture Mapping
Implement a customized, lightweight **Vertical 3-Slice rendering technique** for both assets to preserve pixel-art border clarity while stretching heights dynamically. Do not use full 9-slice components. Divide each texture asset into a Top boundary, a repeatable Middle corridor, and a Bottom boundary.

Use the exact coordinates resolved from the central `TextureAtlas`:
* **Slider Thumb (`menu_slider_button_strip3`):**
  * Source Rect: X=722, Y=751, Width=8, Height=14
  * Slicing Matrix: Top Border Height = 4px, Bottom Border Height = 4px, Middle Segment Height = 6px.
* **Slider Track (`sliderrail_c4`):**
  * Source Rect: X=1015, Y=770, Width=8, Height=32
  * Slicing Matrix: Top Border Height = 8px, Bottom Border Height = 8px, Middle Segment Height = 16px.

## 2. Component Architecture
Create `src/UI/Components/UIScrollbar.h` and `.cpp` inheriting from `UIElement`.
* **State Metrics:** Maintain a boolean flag `m_isDragging`, a float tracking the local mouse click offset `m_dragOffsetY`, and a running float for the normalized scroll percentage (`m_scrollRatio`, clamped strictly from `0.0f` to `1.0f`).
* **Sizing Dimensions:** Maintain variables for the absolute track height and the dynamically computed thumb height.

## 3. Detailed Algorithmic Requirements

### A. Dynamic Thumb Rescaling Calculation
The scrollbar must not use a static height for the thumb asset. Inside a layout update pass, compute the thumb height dynamically based on the proportional ratio of the parent viewport container:
* **The Formula:** Multiply the total track height by the fraction of (`Viewport Height` divided by `Content Height` from the layout manager).
* **Boundary Safeguard:** Clamp the final calculated thumb height so that it never shrinks below a reasonable minimum value (e.g., equal to its native texture height of 14px). If the cumulative content height is less than or equal to the viewport height, set the ratio to `1.0f` and hide or disable the scrollbar interactions.

### B. The Vertical 3-Slice Render Pipeline
Inside the `Draw` method, render the Track and the Thumb by sequentially stamping out their 3 slices using shared primitive sprites:
1. **Draw Top Border:** Position the sprite at the base coordinate and draw the top boundary slice using its exact pixel dimensions.
2. **Draw Middle Segment:** Stretch or tile the middle slice vertically to span from the end of the top border down to the beginning of the bottom border, then draw.
3. **Draw Bottom Border:** Position the bottom slice at the very end of the requested component height and draw.
*Note:* The Track spans the entire designated height of the scrollbar container. The Thumb shifts its rendering destination vertically along the Track based on the current calculation of `m_scrollRatio`.

### C. State-Driven Drag-and-Drop Interaction Mechanics
Override your interaction engine to process mouse events directly:
1. **Mouse Button Press (`sf::Event::MouseButtonPressed`):** If the user clicks the left mouse button, mathematically verify if the cursor coordinates sit within the localized bounding rectangle of the calculated Thumb position. If true, activate `m_isDragging = true` and calculate `m_dragOffsetY` (the current mouse Y position minus the thumb's current local Y origin) to eliminate target snapping artifacts.
2. **Mouse Motion (`sf::Event::MouseMoved`):** If `m_isDragging` is active, resolve the new local Y coordinate of the thumb by taking the cursor's current Y position and subtracting `m_dragOffsetY`. Clamp this position strictly so the thumb cannot slide past the absolute top or bottom boundaries of the track. Divide the current relative thumb offset by the total travelable range (`Track Height - Thumb Height`) to derive the updated `m_scrollRatio`.
3. **Mouse Button Release (`sf::Event::MouseButtonReleased`):** When the left mouse button is unclicked, instantly reset `m_isDragging = false`.

### D. Bi-Directional Synchronization Loop
Integrate the `UIScrollbar` directly inside the `UIScrollView` structure to establish a unified communication loop:
* **Scrollbar-to-View Sync:** When the user drags the scrollbar thumb (modifying `m_scrollRatio`), the `UIScrollView` must intercept this ratio change and update its internal `m_scrollY` displacement equivalently: `m_scrollY = m_scrollRatio * (Content Height - Viewport Height)`.
* **View-to-Scrollbar Sync:** Conversely, when the user scrolls via the mouse wheel inside the viewport area (modifying `m_scrollY`), the scrollview must push an update to the scrollbar to re-calculate its ratio: `m_scrollRatio = m_scrollY / (Content Height - Viewport Height)`. The thumb must adjust its physical position automatically on the next frame.

## 4. Strict Project Code Style Guidelines
* **Allman-style Braces:** Every single opening curly brace `{` must start on an independent, clean newline.
* **No-Space Keyword Formatting:** Eliminate all whitespace buffers after logical flow keywords. You must strictly write `if()`, `for()`, and `while()` without exceptions.

## 5. Verification Execution
Inside `PowerUpState`, attach the `UIScrollbar` component directly to the right edge of your `UIScrollView`. Run the application. Verify that:
1. Lanning the mouse wheel correctly moves both the data grid and shifts the scrollbar thumb smoothly in parallel.
2. Clicking and dragging the thumb up and down seamlessly scrolls the underlying 15 flyweight power-up cards.
3. The graphics for the track and thumb remain clean, crisp pixel-art without blurry scaling artifacts.
```