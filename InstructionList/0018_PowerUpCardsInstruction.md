# Task 1: Implement the Isolated `PowerUpCard` UI Component

## Objective
Create the visual building block for the PowerUp screen: the `PowerUpCard` component. For this initial task, bypass any external JSON databases or layout managers. The goal is strictly to render a single, visually perfect card using hardcoded dummy data to verify the rendering hierarchy.

## 1. Architectural Definition
Create `PowerUpCard.h` and `PowerUpCard.cpp`. This class should encapsulate its own rendering logic and maintain local transforms.
* **Crucial Rendering Rule:** Do NOT use `NineSliceComponent`. All graphical elements must use standard `sf::Sprite` with uniform scaling. The pixel-art aesthetics handle uniform scaling well, making 9-patch logic unnecessary here.
* **Asset Fetching:** Every single visual element (frames, icons, checkboxes) must retrieve its texture coordinates from the `TextureAtlas` using string keys.

## 2. Visual Elements to Render
The `PowerUpCard` must construct and organize the following sub-elements entirely from textures. All positions must be relative to the card's local origin:
* **Outer Background:** The main outer frame of the card (`sf::Sprite`).
* **Title Text:** The power-up name, centered at the top.
* **Inner Icon Frame:** A smaller frame sprite positioned in the center of the card.
* **Item Icon:** The actual item sprite, centered perfectly on top of the Inner Icon Frame.
* **Level Indicators (Checkboxes):** A horizontal row of sprites at the bottom. These are fully texture-driven. You must use specific textures for the states (e.g., an `empty_box` texture, and a `filled_box` or `tick_mark` texture overlay).

## 3. Dummy Initialization
Create a temporary `InitDummyData()` method. Hardcode the C++ with dummy Atlas keys (e.g., `bg_key`, `inner_frame_key`, `icon_might`, `checkbox_empty`, `checkbox_tick`) and values (Title: "Might", Max Level: 5, Current Level: 2) to construct the visual structure.

## 4. Verification Requirement
Instantiate one single `PowerUpCard` in the center of a testing state (e.g., `MainMenuState`). Verify that when the root card is repositioned or scaled, all nested sprites (frames, icons, checkboxes) and text transform correctly relative to the parent.


you can see the example of powerUp ui in this image:
![PowerUp UI](power_up_menu.png)

please make it like this. In the photo, there are multiple of powerUpCard. But firstly, i just need you to implement powerupCard.

## Implementation Plan

### Architectural Feedback
**The idea is excellent.** Bypassing `NineSliceComponent` for complex, ornamental pixel-art frames is the right decision. Using pure `sf::Sprite` composition with local relative offsets ensures pixel-perfect scaling without border distortion.

### 1. New Component: PowerUpCard
Create `src/UI/Components/PowerUpCard.h` and `src/UI/Components/PowerUpCard.cpp`.
* **Inheritance:** Inherit from `UIElement`.
* **Members:** `sf::Sprite m_bgSprite`, `sf::Text m_titleText`, `sf::Sprite m_innerFrameSprite`, `sf::Sprite m_iconSprite`, `std::vector<sf::Sprite> m_levelCheckboxes`.
* **Behavior:** `SetPosition()` and scaling math will calculate hierarchical offsets for all children sprites.

### 2. Initialization & Dummy Data
* Implement `InitDummyData()` inside `PowerUpCard`. 
* Query `TextureAtlas` for dummy keys (`bg_key`, `inner_frame_key`, etc.) mapping them closely to what is available in the `UI.png` atlas.

### 3. Verification in MainMenuState
* Instantiate a `PowerUpCard` in `MainMenuState::SetupUI()`.
* Call `InitDummyData()`, inject into `UIManager`, and verify rendering relative to the screen center.


# Technical Specification: PowerUpCard Component & Flyweight Stateful Rendering

## 1. Architectural Core: PowerUpState Overlay
To preserve strict adherence to the Single Responsibility Principle, the PowerUp interface shall not be integrated into `MainMenuState`. Instead, a dedicated, encapsulated **`PowerUpState`** will be introduced. 

This state operates as a transparent overlay managed via a Pushdown Automata execution stack within the centralized `StateManager`. When active, it intercepts target context inputs while allowing the visual layer of the underlying `MainMenuState` to remain rendered in the background buffer.

---

## 2. Component Design: PowerUpCard Hierarchy
The `PowerUpCard` component inherits from `UIElement`. It functions as a data-driven visual module that isolates its internal composition using relative coordinate space offsets bound to its local origin.

### 2.1 Uniform Sprite Scaling Rule
To preserve the fidelity of the raw pixel-art assets, the component strictly utilizes native `sf::Sprite` composition with uniform scaling vectors ($X = Y$). The structural layout manager completely bypasses `NineSliceComponent` processing, relying instead on uniform asset grid ratios to eliminate edge artifacts.

### 2.2 Component Visual Layer Layout
* **`m_bgSprite`:** The primary foundational framework bounding the entire component.
* **`m_titleText`:** Text label containing the asset designation, absolute-centered at the upper margin.
* **`m_innerFrameSprite`:** An ornamental interior border containing the graphical asset icon.
* **`m_iconSprite`:** The actual entity icon positioned exactly at the relative center of the internal frame.

---

## 3. Flyweight Rendering Pipeline: Level Indicator Checklist
To enforce absolute optimization bounds and prevent heap fragmentation from thousands of volatile objects, the card **must not** store a contiguous sequence of individual sprites (e.g., `std::vector<sf::Sprite>`) to track progression ranks.

Instead, the component implements a stateless **Flyweight Stamp Matrix** mechanism utilizing a single active asset entity.

### 3.1 Memory Allocation Boundaries
* Only **one** static instance of `sf::Sprite m_checkboxSprite` is allocated per card.
* Texture sub-rectangles for both states (`emptyBoxRect` and `filledBoxRect`) are cached as immutable constants fetched from the centralized `TextureAtlas`.

### 3.2 Iterative Drawing Cycle
During the structural draw pass, a localized execution sequence scales and shifts the single descriptor memory footprint across a deterministic linear path:

```cpp
// Architectural reference logic for sequential hardware rendering
for(int i = 0; i < maxLevel; ++i)
{
    float offsetX = startX + (i * spacingInterval);
    m_checkboxSprite.setPosition(offsetX, startY);

    if(i < currentLevel)
    {
        m_checkboxSprite.setTextureRect(filledBoxRect);
    }
    else
    {
        m_checkboxSprite.setTextureRect(emptyBoxRect);
    }

    window.draw(m_checkboxSprite);
}
```
This reduces the operational footprint of active UI element primitives down to a single instance allocation per card while rendering identical spatial results.

---

## 4. Verification & Testing Requirements
* **Isolation Test:** Instantiate the `PowerUpCard` entirely within the isolated boundary of the new `PowerUpState`.
* **Transform Integrity:** Inject hardcoded data via `InitDummyData()` using valid sub-rect strings to confirm the layout matrix remains robust when shifting parent positions.