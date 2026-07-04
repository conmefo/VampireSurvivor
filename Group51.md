# Weekly Report

## General Information

*   **Group ID:** Group51
*   **Group Name:** Group51
*   **Project Name:** Vampire Survivors Clone (Antigravity Engine)
*   **Date range:** 2026-06-29 – 2026-07-04

## Tasks Completed This Week

**[Your Student ID] – [Your Full Name]**

*   Architected the `TextureAtlas` sub-system to parse and load external `ui_atlas.json` layout metadata.
*   Solved the spatial origins mismatch between Unity (Bottom-Left) and SFML (Top-Left) by deriving the geometric inversion formula ($Y_{\text{SFML}} = H_{\text{texture}} - Y_{\text{source}} - H_{\text{sprite}}$).
*   Designed the `NineSliceComponent` rendering utility implementing the 9-Patch algorithm for distortion-free UI scaling.
*   Implemented the "Dirty Flag" optimization pattern to defer heavy vertex calculations to a single frame update, minimizing CPU overhead.

*Evidence: Code commits in `src/Core/Resources/TextureAtlas.cpp`, `src/UI/Components/NineSliceComponent.cpp`, and architectural specifications in `InstructionList/0008_9SlicesScaleInstruction.md` and `InstructionList/0013_JsonReaderInstruction.md`.*

**[Other Member's Student ID] – [Other Member's Full Name]**

*   *(To be filled in by your group member)*

## AI Usage Declaration

This week, we utilized AI (Gemini) extensively as a pair-programming architect and code reviewer, focusing heavily on establishing our core UI rendering systems without sacrificing engine performance. We tackled two major architectural challenges: Texture Atlas Deserialization and Dynamic UI Scaling (9-Slice).

**How we used AI:**

**Challenge 1: Texture Atlas Deserialization (Spatial Mismatch)**
1.  **Identifying the Problem:** I discovered a fundamental spatial conflict: Unity's exported metadata uses a Bottom-Left origin, while our SFML engine requires a Top-Left origin.
2.  **My Suggestion & Prompting:** Instead of asking the AI to write the code blindly, I formulated the mathematical inversion formula (`Y_SFML = H_texture - Y_source - H_sprite`) myself. I drafted a comprehensive technical specification blueprint detailing the encapsulation rules and mathematical proof, and provided this to the AI.
3.  **AI Assistance:** I asked the AI to review my logic for any architectural sanity or OOP violations based on our `CoreManifesto.md`. The AI verified my formula, warned me against writing a fragile custom C++ string parser, and suggested integrating the industry-standard `nlohmann/json` library via CMake. It then generated the structural implementation plan for `TextureAtlas.h/.cpp`, which I reviewed and integrated.

**Challenge 2: Nine-Slice UI Scaling & Optimization**
1.  **Identifying the Problem:** Scaling UI textures directly caused severe visual distortion (stretched borders). We needed a 9-Patch algorithm to dynamically scale UI panels without distortion, but doing complex vertex math every frame would bottleneck the CPU.
2.  **My Suggestion & Prompting:** I designed the architectural concept of a `NineSliceComponent` that processes exactly 36 vertices (9 quads) in a single GPU draw call. To solve the performance issue, I outlined the "Dirty Flag" optimization pattern (caching size mutations and only recalculating geometry when explicitly changed). I prompted the AI with this strict design specification, forbidding it from calculating geometry inside the active render loop.
3.  **AI Assistance:** The AI acted as my implementation assistant. I provided it with the mathematical requirements (screen coordinates vs UV texture mapping), and the AI successfully drafted the tedious quad-generation C++ code, rigorously mapping the correct corners and edges based on my specified margins. The AI also strictly adhered to our Zero-Space control flow and Allman bracing standards, significantly accelerating development while following my exact structural design.

Link: https://drive.google.com/file/d/1l6bO6fog1eM6K4_10oMshg5GawmEkr_o/view?usp=sharing

## Tasks Planned for Next Week

*   Implement the unified `ResourceManager` to transition entirely to string-based asset pools, replacing manual enums.
*   Build the `MainMenuState` integrating dynamic UI bounds and hierarchical sprite animators.
*   Migrate raw assets to the Unity-style categorized directory architecture and automate CMake post-build copy steps.

## Issues

*   **Issue:** We experienced high CPU usage due to recalculating UI vertex geometry every single frame whenever a window resized.
*   **Resolution:** Addressed by designing a "Dirty Flag" pattern inside the `NineSliceComponent`. It now caches property mutations and defers all heavy math calculations until just before rendering, ensuring single draw calls and maximum performance.
