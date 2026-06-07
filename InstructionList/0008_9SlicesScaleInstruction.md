# Specification: Nine-Slice Scaling Component (UI Renderer)

## 1. Overview & Intent
The `NineSliceComponent` is a highly optimized, specialized rendering utility designed for the UI system. 
* **The Problem:** Scaling a standard texture (like a button or panel) directly causes severe visual distortion, particularly stretching the borders and blurring the corners.
* **The Solution:** This component implements the 9-Slice (or 9-Patch) algorithm. It conceptually divides a source texture into a 3x3 grid. When the UI element resizes, the 4 corners remain static in size, the 4 edges stretch only along their specific axis, and the center stretches in both directions.
* **The Goal:** To allow UI elements to dynamically resize to any dimension while perfectly preserving the thickness and crispness of their original borders, executing this in exactly **one single draw call** per element.

## 2. Strict OOP & Architectural Philosophy
This component is governed by absolute encapsulation. It operates as a black box to the rest of the engine.

### 2.1. Data Hiding
The owning entity (e.g., a `Button` class) only dictates the desired width and height. It has absolutely no knowledge of how the texture is sliced, nor does it interact with the vertices. All mathematical data, margins, and geometric arrays must be strictly `private`.

### 2.2. The "Dirty Flag" Optimization Pattern
To preserve CPU cycles, this component must never calculate geometry inside the render loop. It utilizes a `Dirty Flag` pattern:
* Multiple property changes (e.g., changing width, then height, then margins in the same frame) will only toggle a boolean flag (`isDirty = true`).
* The heavy mathematical calculation of vertices is deferred and executed **only once** during the update phase, right before rendering, and only if the flag is true.

## 3. Core State Variables (Private Scope)
* `sourceTexture`: A reference to the original sprite sheet or image.
* `margins` (Top, Bottom, Left, Right): The pixel distances from the texture edges that define the 3x3 slicing grid.
* `targetSize`: The intended final dimensions of the UI element on the screen.
* `vertexArray`: A contiguous block of memory holding exactly 36 vertices (9 rectangular quads * 4 vertices each). This ensures all 9 slices are batched and sent to the GPU in a single operation.
* `isDirty` (Boolean): The optimization flag tracking whether the internal `vertexArray` needs to be rebuilt.

## 4. Lifecycle & Execution Flow

### Phase 1: Property Mutation (Setters)
Whenever a public method like `SetSize(width, height)` or `SetMargins(...)` is called, the component simply stores the new values and sets `isDirty = true`. **No vertex math is performed here.**

### Phase 2: The Update Phase (Logic)
Triggered once per frame by the engine's main loop.
1. It checks the `isDirty` flag.
2. If `false`, it does nothing (saving CPU).
3. If `true`, it invokes a private `RecalculateVertices()` method, then resets `isDirty = false`.

### Phase 3: The Draw Phase (Rendering)
Triggered by the rendering pipeline. This phase is "dumb" and mathematically completely passive. It simply takes the pre-calculated `vertexArray` and submits it to the Graphics API. 

## 5. Algorithmic Concept: Vertex Recalculation
The private recalculation method must construct 9 distinct quads. For every single vertex (36 in total), the algorithm must calculate two distinct vectors:
1. **Screen Position (X, Y):** Where does this point land on the monitor?
    * *Example:* The Top-Right corner quad's X position is calculated as `targetSize.width - margins.right`.
2. **Texture Coordinates (U, V):** Which exact pixel from the `sourceTexture` should be painted here?
    * *Example:* The UV mapping for the corners strictly matches the raw pixel coordinates defined by the `margins`, ensuring 1:1 scale mapping without interpolation.

## 6. Coding Standards (Engine Manifest)
Any implementation of this specification must strictly obey the **antigravity** engine's formatting manifest:
* **Allman Bracing:** All opening curly braces `{}` must be placed on a new, dedicated line.
* **Zero-Space Control Flow:** There must be absolutely no spaces between control keywords and their opening parentheses. Implementations must use `if(isDirty)`, `while(...)`, `for(...)`.