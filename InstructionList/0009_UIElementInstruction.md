# Specification: UI Core Architecture (Part 1)

## 1. Architectural Overview
The UI system in the **antigravity** engine is built upon a strict, component-based, polymorphic hierarchy. To prevent "God Classes" and ensure the Single Responsibility Principle (SRP), UI entities are divided into discrete layers.

This document covers the foundation:
1.  **`UIElement`**: The abstract base skeleton.
2.  **`UIPanel`**: The static, rendering-only container utilizing 9-Slice composition.

## 2. Layer 1: UIElement (The Abstract Base)

### 2.1. Philosophy & Intent
`UIElement` is a pure abstract base class (Interface). It exists solely to provide a unified memory layout for the `UIManager` or `StateManager`. It guarantees that any UI object can be updated and drawn uniformly via a `std::vector<UIElement*>`, regardless of its underlying complexity. **It does not render anything itself.**

### 2.2. Core State Variables (Protected Scope)
To maintain encapsulation while allowing derived classes to function efficiently, spatial data is kept `protected`.
* `position` (Vector2): The absolute X and Y coordinates on the screen.
* `size` (Vector2): The width and height of the bounding box.
* `isActive` (bool): A flag determining if the element should be updated and drawn. If `false`, the system completely skips it.

### 2.3. Public Interface Methods
* `virtual ~UIElement() = default;`: Ensures safe polymorphic destruction.
* `virtual void Update(float deltaTime) = 0;`: Pure virtual method forcing derived classes to define their logic.
* `virtual void Draw(RenderTarget& target) = 0;`: Pure virtual method forcing derived classes to define their rendering submission.
* `void SetPosition(Vector2 pos)` / `Vector2 GetPosition() const`
* `void SetSize(Vector2 newSize)` / `Vector2 GetSize() const`

## 3. Layer 2: UIPanel (The Static Container)

### 3.1. Philosophy & Intent
`UIPanel` inherits directly from `UIElement`. It represents a static visual container (e.g., the Gold Display frame, a dialog box background, or the Version string frame). 
It is **strictly non-interactive**. It does not listen for mouse clicks or keyboard focus. Its sole responsibility is to adapt its visual borders to the `size` defined by its parent class.

### 3.2. Composition (Strict Adherence)
`UIPanel` must not reinvent rendering mathematics. It relies entirely on Composition by owning specialized components:
* `NineSliceComponent backgroundRenderer`: Handles the stretching of the UI borders without distortion.
* `FaderComponent fader`: (Optional/Recommended) Allows the panel to smoothly fade in/out alongside the global `ScreenTransitionManager`.

### 3.3. Implementation Logic
* **Constructor:** Receives the source texture and margins, immediately passing them to the internal `NineSliceComponent`.
* **Overriding `Update`:** * Passes the `deltaTime` to the `FaderComponent`.
    * Synchronizes its spatial data: If the panel's `size` or `position` changed, it invokes `backgroundRenderer.SetSize(...)` to trigger the dirty flag optimization inside the 9-Slice component.
* **Overriding `Draw`:**
    * Simply calls `backgroundRenderer.Draw(target)`.

### 3.4. Code Manifesto Compliance
All generated implementations must respect the core engine rules.
