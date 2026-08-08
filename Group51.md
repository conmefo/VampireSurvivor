# Weekly Report

## General Information

*   **Group ID:** Group51
*   **Group Name:** Group51
*   **Project Name:** Vampire Survivors Clone
*   **Date range:** 2026-08-02 – 2026-08-08

## Tasks Completed This Week

**Vo Ba Thong - 25125036**

*   Architected the modular Treasure Chest reward sequence using a State Pattern (`ITreasurePhaseState`, `TreasurePhase1IdleState`, `TreasurePhase2OpeningState`), decoupling phase transitions, slot reel animations, and reward presentation into isolated state handlers.
*   Built `TreasureRewardView` and interactive visual color tools (`ReelColorTuningUI`) to orchestrate multi-layered reward presentation, item card reveals, and dynamic color palette shifting during chest unlocks.
*   Engineered `ProjectileShadowTrailComponent` utilizing real-time LERP transform history for smooth sub-frame afterimage trails on parabolic projectiles like `AxeWeapon`.
*   Implemented `GarlicWeapon` persistent pulsing aura anchored to player feet pivot coordinates, integrating movement-based tick acceleration and enemy status resistance debuffs.
*   Refactored projectile lifetime and memory safety loops in `ProjectileManager` to safe index-based traversal, preventing iterator invalidation crashes during asynchronous deletion.

**Ho Quang Minh - 25125060**

*   Expanded enemy wave database configurations and calibrated stage difficulty scaling curves over extended survival timers.
*   Integrated multi-channel audio events for chest opening sequences, slot reel tick SFX, and reward fanfare playback.
*   Optimized spatial enemy spawner placement algorithms around letterboxed viewport boundaries to prevent out-of-bounds spawning.

## AI Usage Declaration

This week, we utilized AI (Gemini) primarily as a pair-programming technical advisor for state machine architecture, motion trail interpolation, and safe container traversal in C++.

**How we used AI:**

**Challenge 1: State-Machine Driven Treasure Chest Sequence & Reel Animations**

1.  **Identifying the Problem:** The treasure chest reveal requires multi-stage visual phase transitions (idle, opening, reel spin, item reveal, reward collection). Managing all timing, animations, and input locking inside a single view class created tightly coupled spaghetti code.
2.  **My Suggestion & Prompting:** I designed a modular State Machine pattern dividing the sequence into explicit phase state objects (`ITreasurePhaseState`). I prompted the AI to help structure the abstract base interface and state context lifecycle methods.
3.  **AI Assistance:** The AI verified the state transition flow, generated clean interface boilerplate for the state classes, and assisted in deriving smooth easing functions for the slot reel spin decrescendo effect without hardcoding frame counts.

**Challenge 2: Parabolic Physics & Sub-frame Shadow Trails**

1.  **Identifying the Problem:** High-velocity parabolic projectiles (like Axes) exhibited visual jitter between rendered frames, and standard static trails created disjointed line segments.
2.  **My Suggestion & Prompting:** I formulated the physical launch vector ($625.0\,\text{f}$ gravity acceleration) and designed `ProjectileShadowTrailComponent` using a ring buffer of historical spatial transforms interpolated via LERP.
3.  **AI Assistance:** The AI reviewed the ring buffer implementation, helping ensure safe boundary checks and computing sub-frame alpha decay curves that create smooth, continuous afterimage motion trails.

## Tasks Planned for Next Week

*   Implement weapon evolution merging mechanics when max-level base weapons pair with specific passive items.
*   Build the Stage Select and Character Result recap screen interfaces.
*   Perform comprehensive gameplay balance tuning and memory allocation audits for final release build.

## Issues

*   **Issue:** Modifying active projectile collections inside update loops caused vector iterator invalidation and random runtime crashes during high-density combat bursts.
*   **Resolution:** Replaced raw iterator loops in `ProjectileManager` with safe index-based traversal and asynchronous delayed action queues (`QueueDelayedAction`).
