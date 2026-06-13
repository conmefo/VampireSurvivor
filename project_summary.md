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
