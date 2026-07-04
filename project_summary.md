# Project Status & Architecture Summary

We have successfully established the foundational architecture for the **VampireSurvivor** clone project, ensuring it is highly modular, deeply optimized, and strictly compliant with the engine's `CoreManifesto.md`.

Here is a comprehensive breakdown of everything we have built together so far:

## 1. Core State & Transition Management
* **`StateManager`:** The central hub handling the stack of active states (`PushState`, `PopState`).
* **`ScreenTransitionManager`:** A robust visual manager that executes the "V-Curve" fade workflow, ensuring smooth cross-fading and input locking between major state transitions.

## 2. Title Screen (The Visual Hook)
* **`TitleState` Implementation:** A fully functional title screen that bypasses standard immediate rendering for a dynamic visual reveal.
* **Pixelate Shader (`pixelate.frag`):** A custom GLSL fragment shader that groups physical pixels into mosaic blocks. It interpolates dynamically and smartly snaps off rendering when the threshold is reached.
* **Dynamic Compositing:** Uses `sf::RenderTexture` internally to merge `background.png` and `title.png` perfectly into a single texture before applying the shader, preventing stretching or tearing.
* **Cascading Effects & Interactive Prompt:** Utilizes `FaderComponent` to trigger a staggered fade-in of character decorations, followed by a smooth, ping-pong blinking "PRESS TO START" prompt.

## 3. UI Core Architecture
Built with strict Object-Oriented Principles, focusing on polymorphic flexibility and composition over inheritance.
* **`UIElement` (Abstract Base):** The pure interface holding protected spatial data (`position`, `size`) and enforcing `Update` and `Draw` overrides. It utilizes event-driven `virtual` setters to avoid polling CPU overhead.
* **`UIPanel` (Static Container):** A non-interactive layout container that owns a fader and the background renderer.

## 4. UI Rendering Optimization
* **`NineSliceComponent`:** A highly specialized renderer that executes the 9-Slice (9-Patch) algorithm.
    * **Dirty Flag Pattern:** Defers heavy mathematical calculations to a single frame update, only when properties explicitly change.
    * **Single Draw Call:** Packs exactly 36 vertices into a single `sf::VertexArray` (using `sf::Quads`), allowing infinitely scalable UI windows without distorted borders, all sent to the GPU in one operation.
    * **Per-Vertex Coloring:** Allows the entire panel to be tinted smoothly.

## 5. Interactive UI Layer
* **`UIButton`:** Inherits from `UIPanel` to gain automatic 9-slice borders.
    * **Internal State Machine:** Tracks `Normal`, `Hovered`, `Pressed`, and `Disabled` states.
    * **AABB Bounding Logic:** Uses `Contains(point)` to accurately map window pixels to screen coordinates for precise mouse hover detection.
    * **Decoupled Callbacks:** Manages click interactions via `std::function<void()>`, ensuring the UI layer has absolutely zero hard-coded game logic.

## 6. Coding Standards
Throughout all implementations, we rigorously enforced your **Core Manifesto**:
* **Allman Bracing:** Every brace `{}` opens on a new line.
* **Zero-Space Control Flow:** Strict `if(...)`, `while(...)`, `for(...)` syntax.
* **Data Hiding:** Absolute encapsulation with no `public` variables.
* **Communication Protocol:** All implementation plans are now explicitly appended to the `Instruction.md` document for transparent historical tracking.

***

**Everything currently compiles flawlessly via `CMake`.** We have laid the perfect groundwork to immediately begin developing the `MainMenuState` or jumping into gameplay systems!

***

## Update: Resource Management Integration
* **Completed:** Implemented the strongly-typed `ResourceManager` template class and fully replaced string-based asset loading across the engine.
* **Core Components:**
  * `ResourceManager<Resource, Identifier>`: Generic manager providing strict enum-based `Load` and `Get` functionality.
  * `ResourceIdentifiers.h`: Houses `TextureID` and `FontID` to eliminate hard-coding.
  * `StateContext`: A central struct used to inject the `StateManager` alongside the Textures and Fonts managers seamlessly into every `BaseState`.
* **Integration:** Updated `BaseState`, `LoadingState`, `WarningState`, `TitleState`, and `MainMenuState` to completely decouple their I/O requirements, vastly improving runtime memory safety and reducing frame stuttering.

***

## Update: MainMenuState & Dynamic Layout
* **Text Integration:** Upgraded `UIPanel` and `UIButton` to inherently support centered `sf::Text` components using dynamic alignment recalculations based on their bounding box.
* **UIManager:** Built a robust `UIManager` for batch-processing updates, rendering, and routing input events to a collection of UI elements.
* **Cursor Positioning Protocol:** Implemented a direct polling system inside `MainMenuState` where cursor visuals smoothly snap to the bounds of the currently focused button by iterating the central cluster and querying `IsFocused()`, achieving an optimal "Single Source of Truth."
* **Animated Elements:** Implemented a lightweight animation cycle for the navigation cursors (cycling through frame textures) and used horizontal scale flipping for perfect symmetry without redundant assets.

***

## Update: Texture Atlas & UV Mapping Support
* **Atlas UV Mapping:** Upgraded the `NineSliceComponent` to calculate UV coordinates relative to a designated `sf::IntRect`, achieving pure Texture Atlas (Sprite Sheet) compatibility for optimal GPU batching without duplicating memory.
* **Component Pipeline:** Updated `UIPanel` and `UIButton` constructors to seamlessly accept specific rect coordinates, natively allowing UI layout files to extract tiny individual frames from one massive, globally loaded `.png` file.
* **JSON Deserialization:** Integrated `nlohmann_json` via CMake to parse external `ui_atlas.json` layout files.
* **TextureAtlas Core Manager:** Built `TextureAtlas` class injected into `StateContext` to natively convert external engine spatial coordinates (Unity bottom-left) into accurate SFML coordinate space (top-left) utilizing deterministic mathematical inversion `(Y_SFML = H_texture - Y_source - H_sprite)`. This cache maps texture sub-rectangles via $O(1)$ string lookups.

***

## Update: Smart Atlas Controller & Asset Factory
* **Unified String-Based Asset Pool:** Completely eradicated the `TextureID` enum system, refactoring `ResourceManager` to speak natively in `std::string` identifiers. This achieves absolute parity with the JSON layout structure and eliminates manual enumeration management.
* **Dual-Modality Resolution Engine:** Upgraded `TextureAtlas` into an omniscient asset factory by injecting the `ResourceManager` reference directly into its constructor. The `GetTextureData("assetName")` method now seamlessly checks if an asset is a sliced sub-rectangle (Modality A) or autonomously fetches it from the `ResourceManager` as a full-texture standalone asset (Modality B).
* **Boilerplate Eradication:** Consolidated UI component constructors (`UIPanel`, `UIButton`). They no longer require complex manual texture passing and geometry configuration; they simply accept `(TextureAtlas& atlas, const std::string& assetId)` and delegate the entire parsing logic to the underlying components.

***

## Update: Virtual Canvas & Fullscreen Geometry Pipeline
* **Immutable Resolution Constraint:** Enforced a strict logical coordinate space (`1600x1000`, 16:10 aspect ratio) using a new `WindowSettings.h` configuration layer. The engine forces initialization into `sf::Style::Fullscreen` mode automatically.
* **Hardware Aspect Ratio Adaptation:** Built an autonomous viewport calculator in `main.cpp` that queries the native physical monitor resolution (`sf::VideoMode::getDesktopMode()`) and computes fractional letterboxing or pillarboxing offsets. SFML's native `sf::View` matrix applies this math directly to the GPU without modifying any engine anchors.
* **Input Isolation Boundary:** Upgraded `UIManager::HandleEvent()` to instantly intercept and drop raw mouse inputs (via `window.mapPixelToCoords()`) if the user's cursor physically lands inside the black pillarbox/letterbox margin zones, guaranteeing no ghost clicks on overlapping elements.
* **Anchor Decoupling:** Stripped out dynamic positioning logic from `TitleState` and `MainMenuState`, perfectly centering elements deterministically at absolute coordinates (e.g., `800, 500`) relative to the immutable `VIRTUAL_WIDTH` and `VIRTUAL_HEIGHT`.

***

## Update: Hierarchical Sprite Animator System
* **Projector Pattern Integration:** Implemented the `SpriteAnimator` component. This lightweight, decoupled system isolates playback math (delta time accumulation, looping, and frame indexing) away from entity logic.
* **Centralized Data Library:** Created `AnimationLibrary`, an offline JSON-driven metadata storage cache that organizes `AnimationData` in a two-tiered Species-to-State hierarchy.
* **Dependency Chain Security:** Extracted the raw animation parsing logic completely away from SFML rendering calls. The `AnimationLibrary` delegates directly back to the `TextureAtlas` using JSON reference keys (`"orc_walk_01"`), safely bridging abstract data back to the monolithic Texture Atlas without risking duplication or crashes.
* **Zero Singleton Architecture:** The entire animation library structure sits purely inside `main.cpp` and passes safely downwards via the `StateContext` dependency injection chain, fully eliminating global mutable state.

***

## Update: Asset Directory Architecture Migration
* **Physical Restructuring:** Migrated all loose monolithic `.png` and `.json` assets from the project root into a strictly categorized Unity-style architecture (`Assets/Data/`, `Assets/Graphics/Spritesheets/`, `Assets/Graphics/Backgrounds/`, etc.). 
* **Audio Provisioning:** Created `Assets/Audio/SFX/` and `Assets/Audio/Music/` directories to future-proof the engine for the upcoming audio system.
* **CMake Pipeline Automation:** Updated `CMakeLists.txt` to inject a `POST_BUILD` command. This physically copies the `Assets/` directory into the output build target folder, guaranteeing that all hardcoded engine paths (`"Assets/Data/animations.json"`) resolve correctly relative to the executable without relying on the IDE's working directory.

***

## Update: TitleScreen State Shader & Staggered Reveal
* **Composite Texture Pipeline:** Re-architected `TitleState` to use `sf::RenderTexture` to pre-bake the `introBG_0.png` and `title.png` into a single flattened buffer at initialization. The background correctly scales (`std::max` ratio) to cover the entire letterboxed view without distortion.
* **Fragment Shader Integration:** Successfully injected `pixelate.frag` directly into the render loop, passing `u_pixelSize` via SFML uniform hooks. Achieved perfect pixel-snapping abstraction that gracefully degrades back to raw GPU rendering once the resolution clarity threshold is crossed.
* **Sequential Component Loading:** Re-used `FaderComponent` inside a state-isolated `FadeItem` vector. This allows the newly parsed `illustrations.png` segments to sequentially stagger fade-in only after the shader relinquishes control.
* **Blinking Feedback Loop:** Re-purposed `FaderComponent` for Ping-Pong alpha rendering, generating a smooth, infinitely blinking "PRESS TO START" prompt that acts as the final input gateway into the `MainMenuState`.

***

## Update: Hybrid State Management for UIButton
* **Dynamic Texture Priority:** Upgraded `UIButton` to inherently support separate assets for `Hover` and `Press` states, storing a reference to the `TextureAtlas`.
* **Graceful Degradation:** Modified the internal `UpdateVisuals()` logic to check if a specific state texture exists. If it does, the texture is swapped with opacity forced to 100%. If it does not exist, the button falls back to the default `Idle` texture and applies mathematical color-tinting, ensuring zero visual disruption for UI elements without dedicated hover sprites.
* **UIPanel Texture Swap:** Enhanced the base `UIPanel` with a public `SetTexture(atlas, assetId)` method, securely allowing the `UIButton` subclass to change the `NineSliceComponent`'s source data post-initialization without breaking encapsulation.

***

## Update: Advanced UI Alignment & Dimensional Control
* **NineSlice Dynamic Margins:** Exposed `SetMargins()` on `UIPanel` and `UIButton`, securely allowing dynamic recalculation of the 9-Patch protected border regions post-initialization without disrupting the rendering pipeline.
* **Text Alignment System:** Overhauled text positioning within `UIPanel`. Replaced static centering with a dedicated `TextAlignment` enum (`Left`, `Center`, `Right`). The `AlignText()` logic automatically calculates exact SFML origin offsets and applies standard padding, ensuring text gracefully hugs the correct border of any fluid-width UI container.
* **Modular MainMenu Creation:** Upgraded the `createButton` helper lambda in `MainMenuState` to dynamically accept individual width, height, and text sizes. This allows distinct button groups (e.g., central clusters vs. header/footer action buttons) to exist cleanly within the same local factory method while using distinct visual layouts.
* **Layered Compositing Expansion:** Augmented `MainMenuState` to natively support z-indexed rendering. Implemented a semi-transparent `sf::RectangleShape` overlay that draws explicitly behind the `UIManager` but on top of the composite background, natively separating foreground interactive elements from static backdrops.

***

## Update: Character Data Model Implementation
* **Created:** `CharacterProfile` data class located in `src/Core/Data/CharacterProfile.h`.
* **Manifesto Compliance:** Designed strictly as a pure data container following Absolute Encapsulation (all members private with public `const` getters). Utilizes Allman bracing style.
* **Data-Driven Pillars:** Extracted standard RPG variables (max health, move speed) into a dynamic dictionary (`std::unordered_map<std::string, float> stats;`) to enforce zero hard-coded stats in C++.
* **Added Properties:** Supported the UI system by including `portraitTextureId`, `startingWeaponId`, and `basePrice` fields to represent unlock progression and distinct visual traits natively decoupled from `sf::Texture`.

***

## Update: Virtual Button System & Detail Panel (Phase 3.1)
* **O(1) Matrix Detection:** Implemented `sf::Event::MouseButtonPressed` routing inside `UIGridLayout` to figure out which card was clicked using division/remainder bounds checking instead of iterating through visual hitboxes.
* **Dynamic Draw-Time Tints:** Implemented the Flyweight render pass in `UIGridLayout`. Right before `m_stampCard.Draw` is executed for index `i`, we dynamically swap its inner `m_bgNineSlice` color! On Press, we drop it to a tinted `150,150,150`. On Release, it stamps the bright yellow `selectionSquare_03` `NineSliceComponent` right over the bounds of the card.
* **`UIDetailPanel` Architecture:** Constructed a bottom layout spanning the entire width of the scrolling frame (`750px`). The layout anchors exactly at the bottom border of `UIScrollView` and spans down `155px` to rest identically parallel to the main background margin. The layout reads data emitted from the `m_onSelectionChanged` grid callback.

***

## Update: UI Layout Polish & Dynamic 9-Slice Geometry
* **Dynamic Corner Scaling:** Upgraded `NineSliceComponent` to replace automatic ratio calculations with explicit, integer-based `SetCornerScale()`, ensuring pixel-art frames upscale dynamically without geometry distortion. Exposed this scale parameter through the `UIPanel` and `UIButton` hierarchy.
* **Scroll View Boundaries:** Added comprehensive padding parameters (`m_paddingLeft`, `m_paddingTop`, `m_paddingBottom`) to `UIScrollView`. Mouse clicks are now strictly clipped inside the visible render rect, and out-of-bounds dragging passes fake `MouseMoved` events to reliably un-hover nested UI elements.
* **PowerUpState Enhancements:** Expanded the main `UIGridLayout` boundaries with precise scrollview offsets to achieve a 15px topmost overscroll threshold. Grid selection now defaults to the first item (`Index 0`) instantly upon load.
* **Detail Panel Upgrade:** Swapped raw descriptive text for dynamically sized item icons (`sf::Sprite`) and framed them perfectly using texture coordinates extracted directly from the layout JSON. The "Buy" button was fully upgraded with transparent hover opacities and 2x pixel corners.
* **MainMenu Polish:** Fixed texture-swap priority issues on transparent buttons and scaled the animated selection arrows `2.0x` while increasing their positional padding bounds to prevent clipping.

***

## Update: Character Data Management & Parsing (Task 2)
* **Created:** `CharacterDataManager` core logic in `src/Core/Data/CharacterDataManager.h` and `.cpp`.
* **Manifesto Compliance:** Achieved 100% adherence to Allman bracing and the Zero-Space control flow rule. Designed entirely independent of `GetInstance()` Singleton architecture to support strict dependency injection.
* **JSON Hydration:** Implemented dynamic external data loading using `nlohmann_json`, mapping the `characters` JSON array directly into an encapsulated `std::unordered_map<std::string, CharacterProfile>`.
* **Safe Operations:** Implemented graceful failure handling within `GetCharacterById()` by returning a statically constructed `m_fallbackProfile` and logging an error if an invalid `id` is queried, preventing runtime segmentation faults while satisfying the `const CharacterProfile&` return signature constraint.

***

## Update: GoldDisplayWidget & Overlay UI (Task 4)
* **Created:** Reusable `GoldDisplayWidget` class in `src/UI/Elements/GoldDisplayWidget.h` and `.cpp`.
* **Composition & Reusability:** Inherits directly from `UIPanel` to automatically leverage the `NineSliceComponent` architecture, ensuring pixel-perfect parity with `MainMenuState` without duplicating draw logic.
* **Performance Optimization:** Implemented a dirty-flag polling system (`m_lastKnownGold`). The widget constantly monitors the injected `PlayerProgressionManager` but only executes expensive string allocations and `sf::Text` updates when the balance physically changes.
* **Seamless Overlay Architecture:** Exposed `DEFAULT_X` and `DEFAULT_Y` layout constants directly derived from the `MainMenuState` coordinates. When instantiated over the `CharacterSelectionScreen`, the widget perfectly aligns, providing a flicker-free visual anchor during state transitions.

***

## Update: View / Screen Layout Controller Pattern (Task 5)
* **Created:** `CharacterSelectionView` as the top-level layout orchestrator and `MainBoardPanel` for the central character selection blue-purple board.
* **Strict Encapsulation:** Decoupled layout math from the state layer. The `CharacterSelectionView` coordinates standalone components (`StatsPanel`, `MainBoardPanel`, `GoldDisplayWidget`, `ConfirmButton`) as structural siblings, passing exact layout coordinates relative to the screen size.
* **Anti-Hard-Coding Compliance:** Replaced all magic numbers with `private static constexpr` layout metrics (e.g., `BOARD_WIDTH`, `STATS_MARGIN_RIGHT`). `StatsPanel` is now elegantly positioned to the left of the main board dynamically without using negative coordinates or visual clipping hacks.
* **Cascading Lifecycle:** Both the View and the Panel accurately cascade standard lifecycle events (`Update`, `HandleEvent`, `Draw`) to all instantiated unique pointers, guaranteeing deterministic rendering order and event capture.

***

## Update: StatsPanel Data-Driven Implementation (Task 6)
* **Dynamic Data Ingestion:** Updated `CharacterProfile` with a `GetStat()` helper method for direct access to individual attributes.
* **Procedural Row Formatting:** `StatsPanel` loops through an internal vector of stat keys to procedurally generate and align 19 rows of `sf::Text` instead of relying on hard-coded individual text variables.
* **Smart String Formatting:** The panel dynamically formats attribute values with appropriate `+` or `%` symbols depending on the specific stat (e.g., adding a percentage sign to 'Might' and 'Speed', while leaving 'MaxHealth' as a flat integer).
* **Relative Vertical Layout:** Uses strict class-scoped constants (`ROW_PADDING`, `START_Y_OFFSET`) to calculate perfect vertical alignment, along with explicitly grouped visual gaps to match the game's exact UI layout.

***

## Update: CharacterCardWidget Interactive Element (Task 7)
* **State Management:** Implemented an internal `CardState` enum (`Normal`, `Selected`, `Locked`) to drive visual state changes without external logic leaking into the component.
* **Visual Fidelity:**
  * **Locked:** Instantly applies `sf::Color::Black` to the character sprite, rendering it as a dark silhouette, and dims the character name.
  * **Selected/Normal:** Dynamically swaps between two distinct `NineSliceComponent` instances (`"frameB9"` for the bright gold selection border and `"frame5_c4"` for standard display).
* **Decoupled Callbacks:** Integrates an `m_onClickCallback` (`std::function`) to safely bubble up click events with the specific `m_characterId`, completely avoiding hard-coded state transitions within the card itself.
* **Layout Composition:** Safely composes and positions multiple layers within the widget bounds: the name label, portrait sprite, weapon preview icon, and nine-slice borders.

***

## Update: RosterGridPanel Implementation (Task 8)
* **Dynamic Grid Layout:** Dynamically loops through characters and maps them to a responsive UI grid layout (`MAX_COLUMNS = 4`) using strict math constraints without absolute positioning magic numbers.
* **Data Hydration:** Implemented the `InitializeRoster` method, retrieving profile data from `CharacterDataManager` and mapping unlock statuses directly via `PlayerProgressionManager`.
* **Single-Selection Mediator Flow:** Wrote a unified mediator callback that seamlessly resets previous selections, sets the current widget state to `Selected`, and cleanly propagates the change to the parent View without circular dependencies.
* **Cascading Lifecycle Operations:** Encapsulated `Update`, `HandleEvent`, and `Draw` seamlessly to all instantiated child `CharacterCardWidget` elements using simple vector iteration.

***

## Update: DetailPanel Implementation (Task 9)
* **Visual Composition:** Built the panel with private encapsulated `sf::Text` and `sf::Sprite` elements to display the selected character's name, description, and starting weapon preview.
* **Dynamic Updates:** Created `SetCharacterProfile(...)` which instantly synchronizes all strings and spawns a new sprite for the weapon icon using the `TextureAtlas`.
* **Internal Layout Metrics:** Replaced magic numbers with strict `private static constexpr` metrics (`TEXT_PADDING_X`, `SPACE_BETWEEN_ELEMENTS`, etc.).
* **Cascading Positioning:** Leveraged `SetPosition()` to recalculate layout dynamically based on text bounds, keeping the weapon icon and description text neatly aligned.
* **Coding Standards:** Ensured strict Allman bracing and Zero-Space control flow compatibility.

***

## Update: Action Buttons and Final State Integration (Task 10)
* **Dependency Injection:** Expanded the overarching `StateContext` struct and `main.cpp` entry point to statically instantiate and distribute the `CharacterDataManager` and `PlayerProgressionManager` throughout all states cleanly.
* **CharacterSelectionView Buttons:** Successfully integrated and laid out the "BACK" and "Confirm" `UIButton` elements within `CharacterSelectionView`, strictly using relative `constexpr` positioning for viewport scaling.
* **Dynamic Confirm Validation:** Wired the "Confirm" button to validate against the `PlayerProgressionManager` dynamically. When an unlocked character is selected, it shifts its `ButtonState` to `Normal`. For locked characters (e.g. Gennaro), it resets to `Disabled`.
* **State Encapsulation:** Created the official `CharacterSelectionScreen` state object managing the `CharacterSelectionView` overlay. It cleanly passes its initialized dependencies down into the View and intercepts the upward bubbling lambda callbacks to either pop the state context (BACK) or advance the state machine into gameplay (Confirm).

***

## Update: Font Typography Refinements
* **Font Asset Expansion:** Integrated `assets/fonts/courier_bold.ttf` alongside the primary font, utilizing a robust static fallback loading pattern to prevent runtime crashes if the asset path is unresolved.
* **StatsPanel Readability:** Applied the bold font dynamically to the procedurally generated statistic values inside `StatsPanel`, guaranteeing visual contrast against the standard property labels while retaining global encapsulation standards.

***

## Update: Authentic Character Data Integration
* **Data Migration:** Replaced the mock character JSON loading pipeline with direct integration of the authentic, original `CHARACTER_DATA.json` asset from the game.
* **JSON Pre-processing:** Implemented parsing logic to drill into the specific array schemas, extracting exactly the base profile strings (`charName`, `surname`, `description`, `spriteName`) and float-based stats needed to perfectly map to our `CharacterProfile` class.
* **Sanitization:** Added automatic `.png` stripping for image asset references to ensure compatibility with our internal `TextureAtlas` ID formatting.
* **UI Math & Formatting Optimization:** Standardized UI parsing across `StatsPanel.cpp` to accurately match camelCase keys natively loaded from the new data format. Implemented dynamic arithmetic to correctly translate internal `1.1f` data constraints into explicit `+10%` display outputs for variables like `Might` and `Speed`.

***

## Update: Weapon Data Integration & Dynamic Asset Loading
* **Weapon Data Model:** Implemented `WeaponProfile` and `WeaponDataManager` to natively load `WEAPON_DATA.json`. This abstracts weapon stats and links the raw weapon internal names to their explicit rendering frame names (e.g., `WHIP` -> `whip_frame`).
* **Dynamic Sprite System:** Upgraded `main.cpp` to utilize `std::filesystem::directory_iterator` for autonomous loading. The engine now parses the entire `Assets/Graphics/Characters` directory, loads each texture individually, and automatically cross-references and parses its matching JSON map from `Assets/Data/CharacterAtlas` if available.
* **Sprite Coordinate Mapping:** Integrated the newly loaded texture mappings seamlessly with `CharacterProfile`. The engine safely requests exact bounding boxes (`92x92px`) from the `TextureAtlas` dynamically, completely bypassing monolithic hardcoding for all 60 character files.
* **UI Scaling Consistency:** Embedded mathematical bounding box scaling (`.getLocalBounds()`) directly inside UI element constructors, guaranteeing that regardless of the native asset size, all UI icons clamp exactly to design specs (`92x92` for portraits, `42x42` for items) without distorting resolution or padding.

***

## Update: DetailPanel UI Revamp & Shared Text Utility
* **NineSlice Background Borders:** Replaced standard bounding frames with the `NineSliceComponent` engine, utilizing `"frameB10"` dynamically anchored to an `80x80` footprint for the item preview block inside the `DetailPanel`. Configured dirty-flag updates to reliably flush math calculations natively inside the layout loops.
* **Drop Shadow Rendering:** Advanced `CharacterCardWidget` to dynamically render multi-pass drop shadows exclusively for item icons using `sf::Color::Black` offset tinting before drawing the standard sprite overlay.
* **Global Text Wrapping Support:** Abstracted text wrapping calculations away from specific states into a globally accessible namespace (`UI::TextUtility`). This allows responsive paragraph recalculations directly aligned to specific `MaxWidth` pixel thresholds anywhere in the engine.
* **Layout Mathematics Integration:** Refined relative layout flows in `DetailPanel` to securely calculate remaining window sizes based on dynamic padding limits, perfectly scaling font boundaries for long character descriptions and matching all label font sizes appropriately.

***

## Update: Authentic PowerUp Data & Dynamic UI Sync (Phase 4)
* **PowerUp Data Manager:** Implemented `PowerUpProfile` and `PowerUpDataManager` to load `POWERUP_DATA.json` authentically via `nlohmann_json`. This architecture gracefully handles dynamic stat arrays, bypassing hardcoded UI loop constraints.
* **Dynamic Pricing Engine:** Expanded `PlayerProgressionManager` to calculate power-up upgrade inflation based on the original game's formula: `BasePrice * (1 + 0.1 * TotalPurchasedPowerUps)`. 
* **Save State Serialization:** Built native `Save()` and `Load()` methods into `PlayerProgressionManager`, serializing gold, unlocked characters, and purchased power-ups securely into a local `save_data.json` file.
* **StatsPanel Sync & Gold Rendering:** Fully connected the `StatsPanel` within the `CharacterSelectionView` directly to the active `PowerUpDataManager`. The panel dynamically maps base properties (white text) alongside aggregated power-up buffs formatted in gold text (`sf::Color(255, 215, 0)`), perfectly matching the original UI aesthetics.
* **PowerUpState Reactivity:** Completely rewired `PowerUpState.cpp` to iterate real profiles loaded dynamically. Buying or refunding updates the backend state instantly, saving to disk, and cascading UI `.RefreshData()` calls to accurately lock buttons or update textual inflation costs in real-time.

***

## Update: Co-op Mode UI Integration & Layout Polish
* **Dynamic Co-op Resizing:** Enhanced `CharacterSelectionView` to dynamically shrink the `MainBoardPanel` by 240px and spawn two separate 181x200 player cards (`UIPlayerCoopCard`) seamlessly when transitioning into Co-op mode.
* **Component Encapsulation & Bug Fixes:** Fixed severe selection state bugs where the `Enter Co-op` button is autonomously disabled if `ReadyToStart` is triggered in single-player mode, preventing invisible UI layout crashes. Refined `RosterGridPanel` to properly reset confirmed cards back to a normal state if the user clicks a different character.
* **Compositing Consistency:** Upgraded `CharacterSelectionScreen` to abandon the dark overlay pattern. It now perfectly mimics `MainMenuState` by directly recreating the composite background texture, appending the 3 title illustrations, and explicitly drawing the identical `m_topBarBg` black bar, creating a perfectly seamless transition.
* **PowerUp Text Flow Control:** Leveraged the shared `UI::TextUtility` globally inside `UIDetailPanel` for PowerUps. Dynamically calculates `maxDescWidth` via floating boundaries to automatically wrap description strings so they never intersect with the `Buy` button.

***

## Update: Co-op Mode Selection Flow Bug Fixes
* **Roster Selection Reset:** Implemented `ClearSelection()` within `RosterGridPanel` to properly reset all character cards back to a normal (unhighlighted) state without triggering residual callbacks.
* **Co-op State Transitions:** Fixed multiple logical edge cases in `CharacterSelectionView` where transitioning from Single Player to Co-op, or from Player 1 to Player 2, left ghost data in memory. The system now strictly disables the Confirm button and clears the `m_hoveredCharacterId` during these transitions, requiring explicit and valid selections from all players.
* **Locked Character Handling in Co-op:** Enhanced `UIPlayerCoopCard` interaction. Clicking a locked (unavailable) character in Co-op mode now dynamically resets the player's UI card back to its default, unassigned visual state by securely passing `nullptr` through the `SetCharacterProfile` pipeline, ensuring accurate user feedback.

***

## Update: Player Character & AnimationComponent Integration
* **Component Layer Isolation:** Implemented `AnimationComponent` to encapsulate frame-switching and delta-time math away from entity logic. This fulfills the Single Responsibility Principle and Composition Over Inheritance.
* **Player Entity:** Created `Player` inheriting from `Agent`. The player updates its world position based on WASD input and forwards delta time to the `AnimationComponent`. Constants like `ANIMATION_SPEED` and `BASE_MOVE_SPEED` are safely encapsulated as Tier 2 `private static constexpr`.
* **Data-Driven Extraction:** Updated `CharacterProfile` and `CharacterDataManager` to extract `spriteName` and `walkingFrames` directly from the `CHARACTER_DATA.json`, ensuring character properties natively drive rendering configurations.
* **Inverted Camera Dependency:** Rewired `GameState` so the simulation strictly moves the `Player`, and the rendering viewport bounds track the Player's position afterward, solving the prior camera-driven tight-coupling issue.
* **Selection State Injection:** Passed `selectedCharacterId` via Constructor Injection into `GameState` to cleanly transition from `CharacterSelectionScreen` into active gameplay using the user's explicit choice.

***

## Update: Asynchronous Map Loading & Infinite Camera
* **TileMapManager & Background Threading:** Completely extracted `TileMap` JSON parsing and heavy Image blitting into a dedicated background worker thread within `StageLoadingState`. 
* **Texture Finalization Pipeline:** Separated heavy CPU generation (`sf::Image`) from GPU upload (`sf::Texture`). The worker thread handles all `sf::Image` operations, while the main thread polls for completion and invokes `FinalizeMapTexture()` strictly inside the render loop to comply with SFML's thread-safety guidelines.
* **Infinite Map Scaling:** Removed legacy camera boundary clamps. Vampire Survivors maps repeat endlessly; the `ApplyCameraToView` function now allows infinite panning, while the `TileMap::Draw` and collision logic loops dynamically duplicate the physical map boundaries indefinitely.
* **Sprite Trailing (Ghosting) Effect:** Implemented an authentic shadow-trail effect directly in `Player.cpp`. Instead of maintaining heavy history buffers, the engine draws multiple overlapping copies of the exact current sprite offset backwards relative to the live velocity vector. Ghost opacity diminishes mathematically with a dark blue tint (`sf::Color(30, 30, 80)`), achieving instantaneous crisp feedback when moving.
* **Loading Screen Alignment Polish:** Polished text alignment using SFML's dynamic bounding box origin calculation (`setOrigin`) to create perfectly right-aligned columns pinned to the left edge of the animated loading treasure block. Upgraded typography globally across both loading states to `courier_bold.ttf`.

***

## Update: Weapon Data VFX Parsing (Phase 5)
* **Data Fields Expanded:** Updated `WeaponProfile` strictly following Absolute Encapsulation to privately store `m_bulletType` and `m_hitVFX`, exposing them via `const` getters.
* **JSON Hydration:** Upgraded `WeaponDataManager::LoadData` to successfully parse `bulletType` and `hitVFX` directly from the authentic `WEAPON_DATA.json`. Implemented fallback defaulting (e.g., assigning `""` if `hitVFX` is intentionally omitted like in the Whip) to prevent parsing crashes.
* **Next Steps:** The engine is now structurally prepared to route these strings into an active VFX mapping system (via `HITVFX_DATA.json`) during combat implementation.

***

## Update: Hit VFX Data Architecture & Rosetta Stone Mapping (Phase 6)
* **Created Data Containers:** Built `HitVfxProfile.h/.cpp` strictly following Absolute Encapsulation. Designed it to store parsed visual properties like `isTintFill`, `targetTint`, `hitFrameName`, `impactFrameName`, and `duration`.
* **JSON Hydration:** Built `HitVfxDataManager.h/.cpp` to parse the `assets/Data/HITVFX_DATA.json` file. It maps integer keys (e.g., `4`) directly to fully hydrated profiles. Added automatic `.png` extension stripping for frame names to guarantee compatibility with our global `TextureAtlas`.
* **The Rosetta Stone Mapping:** Solved the loss of the original C# Enum mappings by creating `HitVfxDataManager::GetVfxByName`. This acts as an internal translation layer, safely mapping string identifiers generated by `WeaponProfile` (like `"Dark"`, `"Blood"`) into their corresponding integer IDs in the JSON file.
* **Build Configuration:** Integrated the new modules into `CMakeLists.txt` and confirmed successful compilation. The engine's entire backend data-handling structure for visual effects is now completely ready to support rendering components.

***

## Update: VFX Rendering & Instance Polling (Phase 7)
* **VfxInstance Architecture:** Created a pure data-driven visual node (`VfxInstance`) adhering strictly to the Single Responsibility Principle. The node encapsulates an `sf::Sprite`, applies explicit center-of-mass origin shifting using `.getLocalBounds()`, translates 32-bit `targetTint` hex codes into `sf::Color` mapping, and independently decays its own duration timer translated into delta time.
* **VfxManager Implementation:** Created the centralized orchestrator (`VfxManager`) to manage the ephemeral rendering pool. To bypass the performance bottleneck of standard vector deletion, we implemented the C++ Erase-Remove idiom (`std::remove_if`) directly within the `Update` loop to guarantee O(N) linear time cleanup of expired effects.
* **Global Context Injection:** Safely propagated the `HitVfxDataManager` downwards through the engine architecture by injecting it into `StateContext` and initializing it cleanly in `main.cpp`, completely dodging the Singleton anti-pattern.
* **Validation Sandbox Integration:** Wired a temporary `PlayVfx` trigger directly into `GameState::HandleInput` bound to the Left Mouse Button. It correctly routes pixel coordinate conversions across the `m_worldView` and flawlessly spawns centered, accurately tinted visual effects from the `TextureAtlas` that decay cleanly after their JSON-defined duration.

***

## Update: VFX Animation & Scaling Polish (Phase 7.1)
* **Millisecond Precision:** Identified that original JSON `duration` fields represented milliseconds (from Phaser origin) rather than frames. Modified the `VfxInstance` conversion formula (`duration / 1000.0f`) to achieve the authentic ultra-fast 0.03s - 0.12s "pop" explosions.
* **Tween-Driven Scaling:** Fully integrated the engine's `Tweener` architecture into `VfxInstance` to animate the scaling of hit effects dynamically instead of hard-coding mathematical easing in the `Update` loop.
* **Dynamic Sprite Bounds Capping:** Developed a dynamic targeting algorithm using `.getLocalBounds()` to automatically calculate the maximum allowed `scaleX` and `scaleY`. Ensures that regardless of raw asset resolution, the VFX explosions explicitly cap at exactly `43x43` or `55x55` pixels, preventing screen clutter.
* **Ease-Out Cubic Polish:** Calibrated the Tweener to utilize MathUtils::EaseType::Cubic with EaseMode::Out, starting from 30% scale and aggressively easing into the target bounds, matching the original game's chaotic visual pop.

***

## Update: Weapon System Foundation & Whip Integration (Phases 1-3)
* **Completed:** Implemented the core weapon and projectile systems, allowing automatic, data-driven weapon firing.
* **Core Components:**
  * `Projectile` & `WhipProjectile`: Base class and subclass handling the position, scaling, and fading visual Tweens of slashes.
  * `ProjectileManager`: Orchestrates active projectiles and clears them up in $O(N)$ linear time when expired.
  * `Weapon` & `WhipWeapon`: Handles cooldown updates and spawning of horizontal strikes alternating left/right directions.
  * `WeaponInventory`: Component owned by the player that manages active weapons.
* **Integration:** Connected the player's starting weapon (like Whip) directly from character select profile data. Updated `GameState` to run the lifecycle updates and rendering.

***

## Update: Asset Reverse Engineering & Python Automation Tooling
* **Ghidra Assembly Analysis:** Successfully utilized Ghidra to decompile the original C# DLLs, allowing us to reverse-engineer hardcoded projectile physics and visual behaviors that were omitted from `WEAPON_DATA.json`. For example, we discovered the Magic Wand (`MagicMissileProjectile`) does not use a custom sprite for its trail; it programmatically spawns a generic Unity `Default-Particle` component tinted cyan.
* **Prefab & GUID Resolution:** Established a workflow to extract explicit sprite mappings by parsing Unity `.prefab` files and cross-referencing the `m_Sprite` GUIDs within the `SpriteRenderer` components.
* **Double-Jump Sprite Extractor Tool:** Engineered a robust Python utility (`extract_sprites_by_guid.py`) to navigate massive AssetRipper dumps. The script uses a two-step hop to cross-reference a target GUID to its serialized `.asset` metadata, locate the parent `Texture2D` GUID, automatically copy the master `.png` spritesheet, and generate a precise `_slice_info.txt` containing the exact pixel rect coordinates (`x`, `y`, `width`, `height`) needed to cut out the final sprite.
* **Validation:** Flawlessly verified the entire pipeline by dynamically extracting the Magic Wand's head sprite (`ProjectileHoly1`) and its coordinate map (`1098, 618, 27, 14`) directly from the master `vfx.png` spritesheet.

***

## Update: Magic Wand Particle System Overhaul
* **Particle Visuals Upgrade:** Completely overhauled the `MagicMissileProjectile` particle trail to simulate high-quality Unity-style particle effects natively in SFML. Transitioned from rigid `sf::CircleShape` instances to tiny `sf::RectangleShape` quads (`2x2` base size) utilizing `sf::BlendAdd` for authentic magical cyan glows.
* **Dynamic Spawning Logic:** Replaced static distance-based spawning with a time-based interval system (`m_particleSpawnTimer`). Particles now spawn in randomized clusters (1-3 particles per burst) to create an organic, dust-like texture rather than a uniform dashed line.
* **Organic Trajectory Physics:** Introduced randomized positional scatter and outward velocity drift. As particles age, they shrink in scale while drifting outward naturally, forming a perfectly flared, fading tail.
* **Weapon Scaling:** Adjusted the main projectile weapon sprite (`ProjectileHoly1`) to forcibly scale to a highly visible `45x29` dimension relative to its bounding box, ensuring visual parity with the authentic game aesthetic.

***

## Update: Authentic Enemy Hit Flash System & Component Architecture
* **Decoupled Architecture:** Created a dedicated `HitFlashComponent` using Composition over Inheritance. `EnemyBase` delegates all countdown and color-tinting logic to this component, rigidly avoiding God Class bloat.
* **Data-Driven Tints:** Extracted the `"tint"` value dynamically from `ENEMY_DATA.json` into `EnemyStats`, ensuring base enemy colors are fully data-driven rather than hard-coded.
* **Fragment Shader Integration:** Implemented a custom GLSL fragment shader directly inside `AnimatedEnemy` to correctly support `isTintFill` overrides. This perfectly overwrites sprite RGBs with pure white (or hit effects) while dynamically preserving native alpha thresholds.

***

## Update: Hit VFX Positioning, Delay Polish & Memory Safety
* **Impact Pinning:** Sampled and injected `originalPos` strictly prior to evaluating the knockback vector, meaning impact VFX flawlessly overlay the actual point of impact instead of shifting backward.
* **Authentic Tween Delay:** Overhauled `VfxInstance`'s scaling tweener to mimic the authentic game's delay. The hit sprite remains invisible (`alpha = 0`) for the first 60% of the flash duration and aggressively scales/rotates entirely within the final 40% of its lifetime.
* **Dangling Pointer Memory Fix:** Identified a major memory corruption defect where `std::vector` capacity reallocations invalidated `this` pointers captured inside the Tweener callback lambdas. Refactored `VfxManager` to use `std::vector<std::unique_ptr<VfxInstance>>`, guaranteeing absolute memory stability and eliminating overlapping visual anomalies.

***

## Update: Treasure Chest & Fireworks Architecture Discovery
* **State Machine Isolation:** Analyzed the `Gameplay.unity` file and discovered how treasure chest sequences are executed. The sequence pushes a dedicated `TreasureState` onto the state stack, freezing all player and enemy updates while keeping rendering active via an underlying blackout overlay.
* **FireworksController:** Discovered `FireworksController` acts as a specialized emitter for the chest-open sequence. It operates independently, spawning a sequence of particles (`VfxInstance`) mimicking parabolic physics using vertical velocity and gravity logic.

***

## Update: Magic Wand Tail Architecture Discovery
* **Tracer Prefab:** Solved the missing Magic Wand tail mystery. The tail is not a component of `MagicMissileProjectile.prefab`. Instead, it uses a dynamically instantiated, standalone Unity `TrailRenderer` component stored inside `Tracer.prefab`.
* **Trail Properties:** The `Tracer` uses highly optimized parameters (`m_Time: 0.7`, `m_MinVertexDistance: 0.001`, and `widthMultiplier: 0.2` with a curved fade-in/fade-out). The `ProjectileManager` dynamically attaches these tracers to specific projectiles at runtime, keeping the base prefabs incredibly lightweight and reusable.

***

## Update: Unity ProjectSettings & Performance Optimization Secrets
* **Physics Iterations (The 10k Enemy Secret):** Analyzed `Physics2DSettings.asset` and found that Vampire Survivors sets `m_VelocityIterations` and `m_PositionIterations` to exactly `1` (down from standard values like 8 and 3). This sacrifices realistic bouncing and stacking in exchange for blistering performance, allowing thousands of enemies to slide loosely over each other without tanking the CPU.
* **Transform Synchronization:** Discovered `m_AutoSyncTransforms: 0`, preventing physics calculations every time an object visually moves, saving massive overhead.
* **VFX Capacity:** Checked `VFXManager.asset` and noted a locked 60FPS update rate (`0.01666s`) for determinism, and an enormous `m_MaxCapacity: 100000000`, proving heavy reliance on massive object pools rather than on-the-fly garbage collection.
* **Collision Layer Matrix:** Revealed through `TagManager.asset` how the game manages physics categories (e.g., separating `Player`, `Enemies`, `Pickups`, `Projectiles`, `EnemiesPassThrough`, and `MagnetZone`). This demonstrates that the player's magnet is implemented as an actual invisible physics trigger circle, and enemies are layered meticulously to minimize cross-checking.

***

## Update: Il2Cpp Coroutine Reverse Engineering Discovery
* **Coroutine Architecture (IEnumerator):** Discovered that spawning methods like FireballSpawnerSpawnFireball are compiled into C# IEnumerator coroutines. The decompiled method allocates a state machine struct (e.g., _FireballSpawner.<SpawnFireball>d__16) and stores parameters into it, rather than executing the logic synchronously.
* **Garbage Collection Optimization:** Observed bitwise locking and unlocking loops around pointer assignments (e.g., DAT_1899efb60), confirming that the engine employs internal write barriers for Il2Cpp Garbage Collection safely handling pointers across threads/state machines.
* **Next Steps for Reversing:** To find the actual spawning logic and string resolutions for Coroutines, we need to inspect the MoveNext() method of these generated state machine classes (like FireballSpawner.<SpawnFireball>d__16MoveNext).

***

## Update: FireballSpawner Logic & Prefab Resolution
* **String Mapping Myth Busted:** Analyzed the MoveNext decompilation of FireballSpawner. Discovered that the game does **not** perform string concatenation (e.g., Projectile + FIREBALL) to load sprites at runtime.
* **Prefab Instantiation:** Instead, SpawnFireball takes a direct reference to a Unity Prefab (param_1) and calls UnityEngine.Object.Instantiate<GameObject>(). The reason we only found ProjectileFireball in the fx_atlas.json is because that sprite is hard-assigned to the SpriteRenderer/ParticleSystem directly inside the Unity Prefab asset via the Unity Editor, not in code!
* **State Machine Breakdown:**
  * **State 0:** Yields a WaitForSeconds delay (using the param_2 passed to the Spawner).
  * **State 1:** Instantiates the Prefab, calculates Vector3 positional offsets, and grabs the ParticleSystem component.
  * **State 2:** A mathematical Lerp loop that dynamically scales the fireball's size over time using delta time.
  * **State 3:** Calls Object.Destroy() and triggers FireballSpawnerFireballSequence, which likely handles the actual impact/explosion VFX.

***

## Update: Unity Asset Resolution & Python Mapping
* **AssetRipper Metadata Quirks:** Verified why fx.png.meta does not contain the sprite slice names (like ProjectileFireball). When the original game was extracted, the ripper separated every sub-sprite slice into its own .asset and .meta files, splitting them away from the master texture.
* **Automated Python Solution:** Created and executed a Python script (extract_sprite_mappings.py) to automate the Prefab-to-Sprite linkage. 
* **Next Steps for Reversing:** To find the actual spawning logic and string resolutions for Coroutines, we need to inspect the MoveNext() method of these generated state machine classes (like FireballSpawner.<SpawnFireball>d__16MoveNext).

***

## Update: FireballSpawner Logic & Prefab Resolution
* **String Mapping Myth Busted:** Analyzed the MoveNext decompilation of FireballSpawner. Discovered that the game does **not** perform string concatenation (e.g., Projectile + FIREBALL) to load sprites at runtime.
* **Prefab Instantiation:** Instead, SpawnFireball takes a direct reference to a Unity Prefab (param_1) and calls UnityEngine.Object.Instantiate<GameObject>(). The reason we only found ProjectileFireball in the  fx_atlas.json is because that sprite is hard-assigned to the SpriteRenderer/ParticleSystem directly inside the Unity Prefab asset via the Unity Editor, not in code!
* **State Machine Breakdown:**
  * **State 0:** Yields a WaitForSeconds delay (using the param_2 passed to the Spawner).
  * **State 1:** Instantiates the Prefab, calculates Vector3 positional offsets, and grabs the ParticleSystem component.
  * **State 2:** A mathematical Lerp loop that dynamically scales the fireball's size over time using delta time.
  * **State 3:** Calls Object.Destroy() and triggers FireballSpawnerFireballSequence, which likely handles the actual impact/explosion VFX.

***

## Update: Unity Asset Resolution & Python Mapping
* **AssetRipper Metadata Quirks:** Verified why  fx.png.meta does not contain the sprite slice names (like ProjectileFireball). When the original game was extracted, the ripper separated every sub-sprite slice into its own .asset and .meta files, splitting them away from the master texture.
* **Automated Python Solution:** Created and executed a Python script (extract_sprite_mappings.py) to automate the Prefab-to-Sprite linkage. 
  1. The script scanned all ripped .meta files to build a massive GUID -> SpriteName dictionary.
  2. It scanned all 700+ .prefab files in prefab_files to find their m_Sprite GUIDs.
  3. It cross-referenced the GUIDs to output a clean JSON map (prefab_sprite_map.json) linking PrefabName -> SpriteName (e.g. FireballProjectile.prefab -> ProjectileFireball).
* **Implementation Plan for Engine:** The C++ engine can now take  ulletType: FIREBALL, append Projectile, look up FireballProjectile.prefab in the generated JSON map, and instantly know to pull ProjectileFireball from the  fx_atlas.json without needing to parse Unity YAML files at runtime!

### Date:  - Weapon Targeting Architecture Overhaul
- Refactored Weapon base class and WeaponInventory to take EnemyPool reference instead of a static 	argetPosition.
- Implemented  irtual sf::Vector2f GetTargetPosition to allow weapons to select targets once per burst loop.
- Completed FireballWeapon with AimForRandomEnemy logic (choosing a random active enemy, falling back to facing direction if none are found).

***

## Update: Authentic Runetracer Implementation & Il2Cpp Physics Reversing
* **Runetracer Architecture**: Implemented `RunetracerWeapon` and `RunetracerProjectile` matching the original game's `WEAPON_DATA.json` config (bulletType: `DIAMOND`).
* **Physics Bouncing Math**: Discovered through IL2CPP decompilation that the original game delegates bouncing strictly to the Phaser/Unity physics engine (`setCollideWorldBounds(true)`). Replicated this perfectly in C++ by updating the `ProjectileManager` to enforce bounding-box view collision checks and reversing the velocity vectors cleanly upon impact.
* **Duration-Based Lifetime**: Confirmed the original game uses a strict duration timer (e.g. 2.25s for level 1) to destroy the Runetracer, completely bypassing bounce-count limits. Our C++ engine natively handles this via the `m_duration` logic in `Projectile.cpp`.
* **Visual FX Polish**: Verified the color cycling behavior of the original `DiamondProjectile` which loops a rainbow hue on its trail, while the base projectile itself remains white. Stripped away a false assumption about scaling pulses (which turned out to belong to a completely different bezier-curve projectile, `Rune1`) in favor of an authentic constant-scale rendering pass.
* **Infinite Penetration**: Overrode `OnHitEnemy` to explicitly prevent the projectile from destroying itself, and implemented a precise 500ms hit-delay per enemy as observed in the decompiled code, preventing instant-kill multi-hits while passing through targets.

### Runetracer & Tuning System Update
- **Runetracer Architecture**: Integrated RunetracerWeapon and RunetracerProjectile into the build system and weapon factory. Handles physics bounces via ProjectileManager screen-bounds check.
- **TrailRenderer**: Built a high-performance TrailRenderer component using sf::TriangleStrip to generate continuous ribbons with configurable fade-out starting points, fixed widths, and static RGB base colors.
- **UI Tuning Integration**: Hooked up the Runetracer live properties (diamond scale, trail width, fade start ratio, max lifetime, and color) to the in-game ParticleTuningUI by repurposing ParticleEmitterConfig.
- **Fade-Out State**: Runetracers now gracefully fade their alpha to 0 over 0.5s when their duration expires instead of popping out of existence.

