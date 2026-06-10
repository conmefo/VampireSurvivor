# Architectural Roadmap: Data-Driven Flyweight UI Stamping Engine

Before proceeding, here is our refined architectural vision for the PowerUp list system. We are completely abandoning the traditional, heavy object-oriented hierarchy where every card is an independent UI object. Instead, we are implementing an ultra-optimized, stateless **Data-Driven UI Stamping Engine**. 

We will divide this implementation into two strictly isolated atomic phases:
1. **Phase 1 (Current Task): `UIGridLayout` Engine.** A purely mathematical spatial container that acts as a factory stamp, reusing exactly ONE set of visual primitives to sequentially draw the entire dataset.
2. **Phase 2 (Future Task): `UIScrollView`.** A separate clipping container that manages mouse-wheel input, scrolling offsets, and view clipping matrices.

---

# Task 2.1: Implement the Flyweight `UIGridLayout` Component

## Objective
Implement strictly **Phase 1**. Do not write any code related to scrolling, clipping views, or mouse-wheel scrolling filters yet. Your sole goal is to create a layout manager that manages a pure data vector, computes a 4-column geometric layout, and draws every single row utilizing a single shared pool of SFML visual components.

## 1. The Pure Data Model (`PowerUpData`)
Create a new file named `src/Core/Data/PowerUpData.h`. This file must define a lightweight, stateless C++ structure containing zero graphical overhead, zero SFML transform properties, and no active sprite states.
* It must store basic metrics: A string for the item title, an integer for the current upgrade level, and an integer for the maximum possible level.
* **CPU Cache Optimization:** To eliminate expensive string-lookup operations inside the frame-by-frame render loop, this structure must pre-cache raw, immutable pointers (`const sf::IntRect*`) pointing directly to the resolved coordinate rectangles inside the central `TextureAtlas`. It requires pointers for the outer card background, the inner ornamental frame, the item icon, the empty checkbox state, and the filled checkbox state.

## 2. Component Architecture: `UIGridLayout`
Create `src/UI/Components/UIGridLayout.h` and `.cpp` inheriting from your base UI container class (`UIElement`).
* **The Shared Flyweight Primitives:** This class must declare exactly **one single instance** of each required graphical element as private member variables: one `sf::Sprite` for the outer background, one `sf::Sprite` for the inner frame, one `sf::Sprite` for the item graphic icon, one `sf::Sprite` for the progression checkboxes, and one `sf::Text` object for the title typography.
* **The Dataset:** Maintain a contiguous `std::vector<PowerUpData>` representing the raw information of all available power-ups.
* **Layout Grid Properties:** Implement configuration variables for the column limit (hardcoded or configured to 4), cell width, cell height, horizontal spacing padding, and vertical spacing padding.

## 3. Detailed Algorithmic Requirements

### A. The Stamping Render Loop (Inside the `Draw` Pass)
Within the overridden `Draw` function, you must execute a single monolithic loop through the `PowerUpData` vector. For every element in the dataset, perform the following structural steps:
1. **Coordinate Calculation:** Calculate the target absolute `X` and `Y` coordinates for the current card. The column index is derived from the current loop index modulo the column limit. The row index is derived from the current loop index divided by the column limit. Multiply these indexes by their respective cell dimensions combined with the padding spacings, then add the local origin offset of the grid container.
2. **Commit Background Frame:** Set the position of the single shared background sprite to the calculated `X` and `Y` coordinates. Dereference the current item's pre-cached background rectangle pointer, apply it to the sprite, and immediately issue a draw call to the window.
3. **Commit Title Text:** Update the single shared text object with the current item's title string. Position it using a pre-configured relative offset bound to the card's calculated coordinates, and draw.
4. **Commit Icons:** Position the single inner frame sprite and item icon sprite using their respective local relative center offsets, apply their cached texture rectangles from the data model, and draw both sequentially.
5. **Nested Checkbox Stamp:** Run a nested sub-loop iterating from 0 up to the maximum level of the current item. For each step, calculate a horizontally shifted coordinate based on a fixed spacing interval. Set the position of the single shared checkbox sprite. Compare the current checkbox index against the item's current upgrade level metric: if the index is less than the current level, apply the filled texture rectangle pointer; otherwise, apply the empty rectangle pointer. Issue a draw call for each checkbox instance sequentially.
6. **Dynamic Bounding Height:** At the end of the loop or whenever the dataset updates, calculate the total cumulative height of the grid based on the final number of rows generated. Update the grid component's internal dimensions so that future boundary systems can read its true vertical size.

### B. Mathematical Input Resolution (O(1) Grid Inversion)
You must completely bypass heavy bounding-box collision detection loops or raycasts when tracking user interactions. Implement an interaction query method that accepts a local cursor coordinate vector and resolves the target data index mathematically:
1. Divide the cursor's local `X` and `Y` positions by the sum of the cell dimensions and padding spacing. Cast these results to integers to instantly yield the target grid column and row.
2. Perform a safety validation check to determine if the cursor is hovering directly over the blank padding gaps: subtract the aligned column/row pixel offsets from the cursor coordinates. If the remaining remainder values exceed the raw cell width or cell height boundaries, the cursor is resting on empty spacing, and the method must immediately return a null/invalid index (-1).
3. If valid, convert the 2D row and column grid coordinate into a flat 1D array index using the standard matrix formula: row index multiplied by the total column count plus the column index. Validate this index against the bounds of your dataset vector, and return the resolved index.

## 4. Strict Project Code Style Guidelines
Your generated source files must flawlessly observe our explicit repository styling constraints:
* **Allman-style Braces:** Every opening curly brace `{` must reside on its own clean, dedicated newline.
* **No-Space Keyword Formatting:** There must be zero white space left between structural control flow keywords and their opening parentheses. You must strictly write `if()`, `for()`, and `while()` without exceptions.

## 5. Verification Execution
Inside the `PowerUpState` overlay, instantiate this hybrid `UIGridLayout` container. Generate a mock vector containing 15 distinct `PowerUpData` entities. Populate their internal rectangle pointers once from the centralized `TextureAtlas` during setup. Verify that the system cleanly stamps out all 15 cards in a flawless 4-column matrix. It is perfectly expected and correct if the lower rows overflow beyond the bottom screen edge for now, as the scroll window handler will be isolated to the next sub-task.
```