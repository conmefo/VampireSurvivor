# Task 2.2: Implement the `UIScrollView` Clipping Container (Mouse-Wheel Layer)

## Objective
Implement the foundational viewport and clipping system for our scrollable interface by creating the `UIScrollView` component. For this sub-task, focus **strictly** on vertical navigation driven by the mouse wheel and boundary containment clamping. Do not design or render the graphical scrollbar track/thumb assets yet—that interaction loop is completely isolated to the next phase.

## 1. Component Architecture
Create `src/UI/Components/UIScrollView.h` and `.cpp` inheriting from your base UI container class (`UIElement`).
* **The Content Target:** It must manage exactly one internal pointer referencing a generic child container: `UIElement* m_content` (which will host our `UIGridLayout` from Phase 1).
* **The Viewport Metrics:** Maintain bounding dimensions representing the fixed, physical layout window on the screen (e.g., `m_viewWidth`, `m_viewHeight`).
* **State Metrics:** Maintain a internal float tracking the current vertical scrolling displacement offset (`m_scrollY`).

## 2. Detailed Algorithmic & Hardware Requirements

### A. Hardware Clipping Pass (Inside the `Draw` Method)
To force all overflow elements to immediately turn invisible when escaping the bounds of the viewport rectangle, you must leverage SFML's native `sf::View` matrix mapping:
1. **Initialize Local View Matrix:** Setup an internal `sf::View` instance whose rectangular dimensions map 1:1 with the bounding box of this `UIScrollView` component.
2. **Viewport Optimization (`setViewport`):** Convert the absolute pixel layout position of this component on the monitor screen into normalized screen ratios (from `0.0f` to `1.0f`) relative to your total virtual resolution dimensions. Apply this calculated fraction bounding area to the view using `sf::View::setViewport()`.
3. **Target Positioning:** Set the absolute coordinate of your internal `m_content` pointer, making sure its local vertical coordinate is systematically offset by subtracting your accumulated `m_scrollY`.
4. **The Render Pipeline Switch:** Before triggering drawing on the child content, save the window's original view configuration. Switch the active window view matrix over to your newly configured clipping view. Call `m_content->Draw(window)`.
5. **Context Restoration:** Immediately after drawing the child contents, restore the window's default view matrix so that subsequent system layouts (top bars, bottom bars, back actions) render normally without viewport truncation.

### B. Mouse Wheel Processing & Boundary Clamping
* Override your central input detection router to capture `sf::Event::MouseWheelScrolled`.
* **Hover Filter Check:** Before applying physics offsets, calculate mathematically if the coordinates of the cursor fall strictly within the physical bounding box dimensions of this `UIScrollView`. If the mouse is drifting outside, discard the event.
* **The Scroll Formula:** If the event passes validation, multiply the scroll delta metric by a designated velocity constant (e.g., `scrollSpeed = 30.0f`) and subtract it from your running `m_scrollY` accumulator.
* **Deterministic Containment Clamping:** Prevent the view from drifting into empty space. You must clamp `m_scrollY` securely between two critical bounding parameters:
  * **Upper Limit:** `0.0f` (The absolute top edge of the content alignment).
  * **Lower Limit:** `m_content->GetHeight() - m_viewHeight`. If the cumulative height of the structural content layout is smaller than the current physical viewport window height, the lower bound must default strictly to `0.0f` to completely disable scrolling artifacts.

## 3. Strict Project Code Style Guidelines
Your generated implementation must perfectly respect our explicit code formatting constraints:
* **Allman-style Braces:** Every single opening curly brace `{` must start on an independent, clean newline.
* **No-Space Keyword Formatting:** Eliminate all whitespace buffers after logical flow keywords. You must strictly write `if()`, `for()`, and `while()` without exceptions.

## 4. Verification Setup
Inside `PowerUpState`, update your layout hierarchy. Instantiate the new `UIScrollView` at a fixed location on the screen. Take your `UIGridLayout` from Phase 1 (stamped with its 15 mock data items) and bind it as the content child of the scrollview. Run the application, verify that hovering your mouse over the list and scrolling the mouse wheel slides the grid up and down seamlessly, and ensure that cards are sliced perfectly at the top and bottom viewport seams without any bleeding pixels.
```