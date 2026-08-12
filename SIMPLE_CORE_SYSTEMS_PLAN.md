# Simple Core Systems Implementation Plan

This plan covers a small, playable version of three missing systems:

1. XP and level-ups
2. Pickups and a simple run economy
3. More varied enemy behavior, bosses, hazards, and a basic end encounter

The goal is to make the current run loop feel complete without trying to reproduce every system from the real game. Keep the implementation data-driven where the current code already supports it, and avoid adding evolutions, Arcana, co-op, advanced pathfinding, or complex boss state machines in this pass.

## Current integration points

- `Player` already stores experience, health, weapons, and global weapon buffs.
- `ExperienceGemManager` already handles XP gems, magnet behavior, and collection.
- `GameState` already handles enemy deaths, XP gem spawning, chest rewards, stage timing, and player contact damage.
- `EnemyBase` currently moves enemies directly toward the player.
- `EnemyPool` already owns active enemies and updates/draws them.
- `TreasureChestManager` and `TreasureRewardView` already provide a simple boss reward flow.

## 1. XP level-up system

### Desired behavior

- Start every run at level 1.
- Collecting XP gems increases the current XP total.
- When XP reaches the threshold, the player levels up.
- Extra XP carries over to the next level.
- If one pickup crosses several thresholds, queue all level-ups instead of losing any XP.
- Pause normal gameplay while the player chooses one simple upgrade.

### Recommended progression

Use a small formula instead of a large data table:

```text
XP required = 20 + (current level * 10)
```

Example: level 1 requires 30 XP, level 2 requires 40 XP, level 3 requires 50 XP. The exact numbers can be tuned after a short playtest.

### Player changes

Update `src/Entities/Player/Player.h` and `Player.cpp`:

- Add `int m_level = 1`.
- Keep `m_experience`, but treat it as XP toward the next level.
- Add `float m_experienceToNextLevel` or calculate the threshold from the level.
- Add getters for level, current XP, and the next-level threshold.
- Change `AddExperience(float amount)` so it loops while XP is above the threshold and returns, or records, the number of levels gained.
- Add a pending-level-up count if the level-up view is controlled by `GameState`.
- Reset level, XP, and pending level-ups when a new run starts.

Suggested behavior:

```text
AddExperience(amount)
    experience += amount
    while experience >= required XP
        experience -= required XP
        level += 1
        pending level-ups += 1
```

### Level-up choice

Create a small view in:

- `src/States/Game/LevelUpView.h`
- `src/States/Game/LevelUpView.cpp`

The first version only needs three choices:

- Upgrade the player's current weapon.
- Add one supported weapon if there is an available weapon slot.
- Increase a simple passive stat such as max health, move speed, or pickup radius.

Use the existing `WeaponInventory::LevelUpWeapon` and weapon factory paths. Do not implement weapon evolutions or unions yet. If the player has no weapon slot or a choice is invalid, replace it with another upgrade choice.

### GameState flow

Update `src/States/Game/GameState.h` and `GameState.cpp`:

1. Let `ExperienceGemManager::Update` collect gems and add XP.
2. Check for pending level-ups immediately afterward.
3. Show `LevelUpView` for the first pending level-up.
4. Pause enemy, projectile, timer, and pickup updates while the view is open.
5. Apply the selected upgrade.
6. Remove one pending level-up and show the next one, if any.
7. Resume the run when the queue is empty.

Do not pause rendering. The player should still see the frozen game behind the overlay.

### HUD changes

Update `src/States/Game/PlayerHUD.h` and `PlayerHUD.cpp`:

- Show `Level N`.
- Add a small XP progress bar.
- Update the bar after every gem collection and level-up.
- Keep the existing health and gold displays.

### XP acceptance checks

- The first collected gems can reach level 2.
- XP above the threshold carries over correctly.
- A large XP reward can produce multiple queued level-ups.
- Gameplay is paused while a choice is open.
- Choosing a weapon upgrade visibly changes the weapon.
- Starting a new run resets level and XP.

## 2. Pickups and simple economy

### Desired behavior

Keep XP gems as they are and add only two simple pickup types:

- Gold coins for run currency.
- A small healing pickup for occasional recovery.

The existing boss chest remains the large boss reward. There is no need to build a full item inventory or shop yet.

### Recommended implementation

Create a separate manager so XP and gold keep different meanings:

- `src/Entities/Pickups/CoinPickupManager.h`
- `src/Entities/Pickups/CoinPickupManager.cpp`

Use a small pickup struct containing:

- Position
- Value
- Active state
- A simple sprite or circle fallback

Reuse the same basic collection rules as XP gems:

- A short collection radius.
- A larger magnet radius.
- Movement toward the player when magnetized.
- Deactivation after collection.

Avoid duplicating complicated physics. The existing XP gem behavior is a suitable template.

### Drop rules

Add a small drop rule in the enemy-death path in `GameState.cpp`:

- Normal enemy: low chance to drop 1-5 gold.
- Elite or tougher enemy: higher chance to drop 5-15 gold.
- Boss: always drop a larger coin bundle, such as 25-100 gold.
- Healing pickup: rare chance from normal enemies, or one guaranteed drop from selected bosses.

Keep the random values configurable in one place so they can be tuned without changing collision code.

### Economy flow

When a coin is collected:

1. Call the existing `GameState::AddRunGold` path.
2. Update the HUD immediately.
3. Keep the coins in run currency until the run ends.
4. Let the existing run-bank logic save the final amount once.

Do not write save data for every coin. This avoids unnecessary disk writes and keeps the run economy easy to reason about.

For healing pickups:

- Add a small `Player::Heal(amount)` method if one does not already exist.
- Clamp health to max health.
- Do not allow a pickup to revive a dead player.

### GameState integration

Add the coin manager to the same lifecycle as the XP gem manager:

- Initialize it with the existing asset resources.
- Update it once per frame while gameplay is active.
- Draw it before enemies or UI, depending on the current render order.
- Clear it when loading a stage or starting a new run.
- Pass a callback or reference to `AddRunGold` when a coin is collected.

Keep `TreasureChestManager` and `TreasureRewardView` as the boss reward presentation. A boss can award both a chest and a coin bundle, but do not create a second reward screen.

### Economy acceptance checks

- Some enemy deaths can produce visible coins.
- Touching or magnetizing a coin increases run gold exactly once.
- Bosses provide a noticeably larger reward.
- The HUD changes immediately after collection.
- Healing never exceeds max health.
- Run gold is banked once at run completion, not once per pickup.
- Pickups are cleared correctly when a run ends or a stage reloads.

## 3. Enemy behavior, bosses, hazards, and end encounter

### Keep the normal enemy behavior

The current direct chase is a good default. Do not replace it with pathfinding. First make the existing runtime path active:

- Change `DebugStaticTargetsMode` in `GameState.cpp` to a real debug option that defaults to `false`.
- Ensure stage spawning, stage events, and `EnemyPool::Update` run during a normal game.
- Keep contact damage handled by the existing `ApplyEnemyContactDamage` flow.

### Add one ranged enemy

Add optional ranged fields to the enemy definition/stats:

- `isRanged`
- `attackRange`
- `attackCooldown`
- `projectileSpeed`
- `projectileDamage`

The ranged behavior can stay very small:

```text
if distance > attackRange
    move toward player
else
    stop or move slowly
    when cooldown reaches zero, fire one projectile toward player
```

Create a lightweight enemy attack manager, for example:

- `src/Entities/Enemy/EnemyAttackManager.h`
- `src/Entities/Enemy/EnemyAttackManager.cpp`

Each attack only needs position, velocity, damage, lifetime, radius, and active state. Update movement, remove expired/off-screen attacks, draw a simple shape, and apply damage once when it touches the player.

Do not reuse player projectile damage code unless the ownership and collision rules remain clear. Enemy attacks must never damage enemies or grant XP.

### Add one simple boss type

Use the existing boss spawn data and `m_bossEnemies` tracking. Add an `isBoss` flag or boss stats to the enemy definition instead of creating a large class hierarchy.

Basic boss rules:

- Large sprite or scale.
- 8-12 times normal health.
- Slower movement than normal enemies.
- Higher contact damage.
- A visible health bar or boss name when active.
- One attack every few seconds.

Use one simple boss attack pattern first:

- Aim one projectile at the player; or
- Fire a small radial burst of 6-8 projectiles.

The boss can alternate between chasing and attacking. A tiny state set is enough:

```text
Chase -> Telegraph briefly -> Attack -> Cooldown -> Chase
```

Do not add multiple phases, invulnerability windows, summons, or procedural bullet patterns in this pass.

### Boss rewards

When a boss dies:

- Spawn a large XP reward.
- Spawn a guaranteed coin bundle.
- Keep the existing treasure chest reward if it is already part of the current flow.
- Clear the boss health bar and boss tracking state.

### Add one stage hazard

Create a minimal hazard system, either as a small manager or as a `StageHazard` list owned by `GameState`:

- Circular damage zone.
- Position and radius.
- Active duration.
- Damage-per-second value.
- Simple translucent red/orange drawing.

Spawn one hazard from a stage event or a boss attack. When the player overlaps it, apply damage over time with a short damage interval so the player is not damaged once per render frame. Remove the hazard when its duration ends.

This is enough to introduce area denial without implementing traps, moving platforms, environmental physics, or a full stage hazard database.

### Basic end encounter

At the current run time limit, replace immediate completion with a simple final-boss step:

1. Stop normal enemy spawning.
2. Spawn one oversized, killable end boss at the edge of the play area.
3. Keep existing enemies active until they die or leave the arena.
4. Show a boss name and health bar.
5. Complete the run only after the end boss is defeated.
6. Use the existing completion and run-gold banking flow.

This is intentionally a simple Reaper/Ender-style encounter, not an exact recreation. If it makes the first playtest too difficult, give the player a short preparation window or use a lower boss health multiplier.

### Enemy acceptance checks

- Normal enemies spawn and chase during a real run.
- A ranged enemy fires toward the player and its projectile can be dodged.
- Enemy projectiles expire and cannot damage the player repeatedly every frame.
- Bosses have substantially more health and use one readable attack pattern.
- Boss deaths give XP, gold, and the existing chest reward.
- At the time limit, the final boss appears and normal spawning stops.
- The run completes only after the final boss is defeated.
- A hazard visibly damages the player over time and then disappears.

## Suggested implementation order

1. Disable static-target debug mode and verify normal waves, enemy updates, and stage events.
2. Add player level, XP thresholds, and the level-up queue.
3. Add the level-up overlay and the level/XP HUD.
4. Add coin and healing pickups, then connect enemy death drops and run gold.
5. Add enemy projectiles and one ranged enemy definition.
6. Add boss stats, the boss attack, health bar, and boss rewards.
7. Add one temporary damage-zone hazard.
8. Replace time-limit completion with the simple final boss encounter.
9. Run a full playtest and tune XP, drop rates, enemy damage, projectile speed, and boss health.

## Files likely to change

### New files

- `src/States/Game/LevelUpView.h/.cpp`
- `src/Entities/Pickups/CoinPickupManager.h/.cpp`
- `src/Entities/Enemy/EnemyAttackManager.h/.cpp`
- Optional: `src/Entities/Stage/StageHazardManager.h/.cpp`

### Existing files

- `src/Entities/Player/Player.h/.cpp`
- `src/States/Game/PlayerHUD.h/.cpp`
- `src/States/Game/GameState.h/.cpp`
- `src/Entities/Enemy/EnemyBase.h/.cpp`
- `src/Entities/Enemy/EnemyPool.h/.cpp`
- `src/Entities/Enemy/EnemyDatabase.h/.cpp`
- `assets/Data/enemies/*.json`
- Build or asset-registration files if new source files require them

## Definition of done

The run should now have this loop:

```text
Kill enemies
    -> collect XP and occasional gold/healing pickups
    -> level up and choose a simple upgrade
    -> survive ranged enemies, hazards, and bosses
    -> defeat the final boss
    -> bank run gold and finish the run
```

Once this works reliably, the next layer can add richer weapon choices, passive items, evolutions, more enemy patterns, stage-specific hazards, and more accurate end encounters.
