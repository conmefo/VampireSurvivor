# Task 2.4: Implement Static Header Elements and the Main Background Panel

## Objective
Now that the interactive scroll view and slider are functional, implement the static structural containers surrounding them. You will render the primary outer background panel, the main "PowerUp Selection" typography header, and the horizontal "Refund PowerUps" action button. Keep this isolated from economy database mechanics for now.

## 1. Architectural Layout Composition
All these new elements must be instantiated and managed directly inside `PowerUpState`. They sit outside the `UIScrollView` hierarchy so that they remain completely static on the screen when the grid scrolls.
* **Main Background Panel:** Use a large `NineSliceComponent` to render the primary deep-blue container background that holds the entire menu layout.
* **Refund Button:** This is a wide horizontal button element. It must use a `NineSliceComponent` for its blue base texture to allow horizontal stretching without distorting its corners.

## 2. Visual & Positioning Requirements

### A. The Header Stack Alignment
Inside `PowerUpState::SetupUI()`, arrange the elements sequentially from top to bottom relative to the Main Background Panel's origin:
1. **Title Typography:** Center the text `"PowerUp Selection"` horizontally at the top margin of the panel using the standard game font.
2. **Refund Button Placement:** Position the `"Refund PowerUps"` button directly below the title text, aligned perfectly to the horizontal center of the main panel. Place the text string `"Refund PowerUps"` at the absolute center of this button component.
3. **Scroll View Re-anchoring:** Shift the top boundary of your existing `UIScrollView` downward so that it sits neatly below this new Refund Button, preventing any visual overlapping.

## 3. Strict Project Code Style Guidelines
Your generated implementation must perfectly respect our explicit repository constraints:
* **Allman-style Braces:** Every single opening curly brace `{` must start on an independent, clean newline.
* **No-Space Keyword Formatting:** Eliminate all whitespace buffers after logical flow keywords. You must strictly write `if()`, `for()`, and `while()` without exceptions.

## 4. Verification Execution
Run the application and open the PowerUp menu. Verify that:
1. The deep-blue main background panel frames the entire screen center beautifully.
2. The title and the wide blue "Refund PowerUps" button are perfectly centered and remain locked at the top when you scroll the grid underneath them.
3. The corners of the stretched Refund Button look sharp and clear, confirming correct 9-slice rendering.

