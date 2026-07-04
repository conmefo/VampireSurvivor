# Vampire Survivors C++ Project — 2-Person Task Division

## 1. Project Goal

We are building a simple **Vampire Survivors-style game in C++**.

The game should include:

- A controllable player
- Auto-attacking weapons/projectiles
- Enemies that chase the player
- Enemy spawning over time
- EXP drops and level-up upgrades
- Basic UI:
  - HP bar
  - EXP bar
  - Timer
  - Level display
  - Game over screen

The project will be divided by **features**, not by UI/backend.

Each person owns the **whole UI and logic** of their feature.

---

#  Task Division

## Person 1 — Player, Weapon, EXP, Upgrade System

Person 1 owns everything related to the player and player combat.

### 1 Logic Responsibilities

Person 1 will implement:

- Player movement
- Player HP
- Player damage receiving
- Player level
- Player EXP
- Auto-attack weapon system
- Projectile creation
- Projectile movement
- Projectile collision with enemies
- Enemy damage from projectile
- EXP collection
- Level-up checking
- Upgrade selection logic

### 2 UI Responsibilities

Person 1 will implement:

- Player HP bar
- EXP bar
- Level text
- Upgrade selection menu
- Weapon information display
- Optional: damage number or hit effect

### 3 Classes Owned by Person 1

```txt
player/
├── Player.h
├── Player.cpp
├── Weapon.h
├── Weapon.cpp
├── Projectile.h
├── Projectile.cpp
├── ExperienceOrb.h
├── ExperienceOrb.cpp
├── Upgrade.h
├── Upgrade.cpp
├── UpgradeMenu.h
└── UpgradeMenu.cpp

ui/
├── PlayerHUD.h
└── PlayerHUD.cpp
```

### 4 Main Classes

```cpp
class Player;
class Weapon;
class Projectile;
class ExperienceOrb;
class Upgrade;
class UpgradeMenu;
class PlayerHUD;
```

---

## Person 2 — Enemy, Wave, Spawning, Game State System

Person 2 owns everything related to enemies and game survival pressure.

### 3.1 Logic Responsibilities

Person 2 will implement:

- Enemy movement toward player
- Enemy HP
- Enemy damage
- Enemy collision with player
- Enemy death
- Enemy spawning around the player
- Spawn rate increasing over time
- Different enemy types
- Wave/difficulty scaling
- Game timer
- Win condition
- Lose condition
- Pause/restart logic
- Enemy drop logic after death

### 3.2 UI Responsibilities

Person 2 will implement:

- Game timer display
- Wave/stage display
- Enemy HP bar, optional
- Pause menu
- Game over screen
- Victory screen
- Restart instruction text

### 3.3 Classes Owned by Person 2

```txt
enemy/
├── Enemy.h
├── Enemy.cpp
├── EnemySpawner.h
├── EnemySpawner.cpp
├── WaveManager.h
├── WaveManager.cpp
├── DropManager.h
└── DropManager.cpp

game/
├── GameStateManager.h
├── GameStateManager.cpp
├── GameTimer.h
└── GameTimer.cpp

ui/
├── GameHUD.h
├── GameHUD.cpp
├── GameOverScreen.h
├── GameOverScreen.cpp
├── VictoryScreen.h
└── VictoryScreen.cpp
```

### 3.4 Main Classes

```cpp
class Enemy;
class EnemySpawner;
class WaveManager;
class DropManager;
class GameStateManager;
class GameTimer;
class GameHUD;
class GameOverScreen;
class VictoryScreen;
```

---

# 5. Shared Code Between Both People

Both people can use these shared files.

No one should randomly change shared files without telling the other person first.

```txt
common/
├── Vector2.h
├── Rect.h
├── Collision.h
├── Constants.h
└── Entity.h

game/
├── Game.h
└── Game.cpp
```

---

## 5.1 Vector2

Used for position, movement direction, and velocity.

```cpp
struct Vector2 {
    float x;
    float y;

    Vector2();
    Vector2(float x, float y);

    Vector2 operator+(const Vector2& other) const;
    Vector2 operator-(const Vector2& other) const;
    Vector2 operator*(float scalar) const;

    float length() const;
    Vector2 normalized() const;
};
```

### Used by:

- Player position
- Enemy position
- Projectile position
- Movement direction
- Spawn position

---

## 5.2 Rect

Used for collision.

```cpp
struct Rect {
    float x;
    float y;
    float width;
    float height;
};
```

### Used by:

- Player collision box
- Enemy collision box
- Projectile collision box
- EXP orb collision box

---

## 5.3 Collision

Used to check if two objects touch each other.

```cpp
class Collision {
public:
    static bool checkAABB(const Rect& a, const Rect& b);
};
```

### Used by:

- Projectile hits enemy
- Enemy touches player
- Player collects EXP orb

---

## 5.4 Entity Interface

All game objects should follow this basic interface.

```cpp
class Entity {
public:
    virtual ~Entity() = default;

    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;

    virtual Rect getBounds() const = 0;
    virtual bool isAlive() const = 0;
};
```

### Used by:

- Player
- Enemy
- Projectile
- ExperienceOrb

---

# 6. Important Interfaces Between Both People

This section defines how Person 1 and Person 2 communicate through code.

The goal is to avoid messy direct access.

---

## 6.1 Player Interface

Owned by: **Person 1**

Used by: **Person 2**

Person 2 needs to know:

- Where the player is
- Whether the player is alive
- How to damage the player

```cpp
class Player : public Entity {
public:
    void update(float deltaTime) override;
    void render() override;

    Vector2 getPosition() const;
    Rect getBounds() const override;

    void takeDamage(int damage);
    bool isAlive() const override;

    int getHP() const;
    int getMaxHP() const;

    int getLevel() const;
    int getEXP() const;
    int getEXPToNextLevel() const;

    void addEXP(int amount);
};
```

### Who calls Player?

```txt
Game
 └── calls Player.update()

Enemy
 └── needs Player.getPosition()

Enemy / EnemySystem
 └── calls Player.takeDamage()

PlayerHUD
 └── calls Player.getHP()
 └── calls Player.getEXP()
 └── calls Player.getLevel()
```

---

## 6.2 Enemy Interface

Owned by: **Person 2**

Used by: **Person 1**

Person 1 needs to know:

- Enemy position
- Enemy collision box
- Whether enemy is alive
- How to damage enemy

```cpp
class Enemy : public Entity {
public:
    void update(float deltaTime, const Player& player);
    void render() override;

    Vector2 getPosition() const;
    Rect getBounds() const override;

    void takeDamage(int damage);
    bool isAlive() const override;

    int getHP() const;
    int getMaxHP() const;

    int getEXPDropAmount() const;
};
```

### Who calls Enemy?

```txt
WaveManager / EnemySpawner
 └── creates Enemy

Game
 └── calls Enemy.update(deltaTime, player)

Projectile / CombatSystem
 └── calls Enemy.takeDamage()

DropManager
 └── calls Enemy.getEXPDropAmount()
```

---

## 6.3 Projectile Interface

Owned by: **Person 1**

Used by: **Game / Combat checking**

```cpp
class Projectile : public Entity {
public:
    void update(float deltaTime) override;
    void render() override;

    Rect getBounds() const override;
    bool isAlive() const override;

    int getDamage() const;
    void destroy();
};
```

### Who calls Projectile?

```txt
Weapon
 └── creates Projectile

Game
 └── calls Projectile.update()

Game / CombatSystem
 └── checks Projectile bounds with Enemy bounds

Projectile
 └── calls Enemy.takeDamage() indirectly through CombatSystem
```

---

## 6.4 ExperienceOrb Interface

Owned by: **Person 1**

Created by: **Person 2 DropManager**

This is shared because enemy death creates EXP, but the player collects EXP.

```cpp
class ExperienceOrb : public Entity {
public:
    ExperienceOrb(Vector2 position, int amount);

    void update(float deltaTime) override;
    void render() override;

    Rect getBounds() const override;
    bool isAlive() const override;

    int getAmount() const;
    void collect();
};
```

### Who calls ExperienceOrb?

```txt
DropManager
 └── creates ExperienceOrb when enemy dies

Game
 └── checks Player collision with ExperienceOrb

Player
 └── receives EXP using Player.addEXP()
```

---

## 6.5 Weapon Interface

Owned by: **Person 1**

```cpp
class Weapon {
public:
    void update(float deltaTime, const Vector2& playerPosition);
    void render();

    std::vector<Projectile>& getProjectiles();

    void increaseDamage(int amount);
    void reduceCooldown(float amount);
};
```

### Who calls Weapon?

```txt
Player
 └── owns Weapon

Player.update()
 └── calls Weapon.update()

Game / CombatSystem
 └── gets projectiles from Weapon
```

---

## 6.6 EnemySpawner Interface

Owned by: **Person 2**

```cpp
class EnemySpawner {
public:
    void update(float deltaTime, const Player& player);
    std::vector<Enemy>& getEnemies();

private:
    void spawnEnemyAroundPlayer(const Player& player);
};
```

### Who calls EnemySpawner?

```txt
Game
 └── calls EnemySpawner.update(deltaTime, player)

EnemySpawner
 └── creates Enemy around Player position
```

---

## 6.7 WaveManager Interface

Owned by: **Person 2**

```cpp
class WaveManager {
public:
    void update(float deltaTime);

    int getCurrentWave() const;
    float getSpawnRateMultiplier() const;
    bool shouldSpawnBoss() const;
};
```

### Who calls WaveManager?

```txt
Game
 └── calls WaveManager.update()

EnemySpawner
 └── uses WaveManager spawn rate/difficulty
```

---

## 6.8 GameStateManager Interface

Owned by: **Person 2**

```cpp
enum class GameState {
    Playing,
    Paused,
    GameOver,
    Victory
};

class GameStateManager {
public:
    void update(const Player& player, float gameTime);

    GameState getState() const;

    void pause();
    void resume();
    void restart();

    bool isPlaying() const;
    bool isGameOver() const;
    bool isVictory() const;
};
```

### Who calls GameStateManager?

```txt
Game
 └── calls GameStateManager.update(player, gameTime)

Game
 └── checks GameState before updating objects

GameOverScreen
 └── shows UI if state is GameOver

VictoryScreen
 └── shows UI if state is Victory
```

---

# 7. Main Call Flow

The main game loop should look like this:

```cpp
while (window.isOpen()) {
    float deltaTime = clock.restart();

    game.handleInput();
    game.update(deltaTime);
    game.render();
}
```

Inside `Game::update()`:

```cpp
void Game::update(float deltaTime) {
    if (!gameStateManager.isPlaying()) {
        return;
    }

    gameTimer.update(deltaTime);

    player.update(deltaTime);

    waveManager.update(deltaTime);
    enemySpawner.update(deltaTime, player);

    for (Enemy& enemy : enemySpawner.getEnemies()) {
        enemy.update(deltaTime, player);
    }

    checkProjectileEnemyCollisions();
    checkEnemyPlayerCollisions();
    checkPlayerEXPOrbCollisions();

    gameStateManager.update(player, gameTimer.getTime());
}
```

Inside `Game::render()`:

```cpp
void Game::render() {
    player.render();

    for (Enemy& enemy : enemySpawner.getEnemies()) {
        enemy.render();
    }

    for (Projectile& projectile : player.getWeapon().getProjectiles()) {
        projectile.render();
    }

    for (ExperienceOrb& orb : experienceOrbs) {
        orb.render();
    }

    playerHUD.render(player);
    gameHUD.render(gameTimer, waveManager);

    if (gameStateManager.isGameOver()) {
        gameOverScreen.render();
    }

    if (gameStateManager.isVictory()) {
        victoryScreen.render();
    }
}
```

---

# 8. Who Calls Who?

## 8.1 Simple Overview

```txt
main.cpp
 └── Game

Game
 ├── Player
 ├── Weapon
 ├── Projectile
 ├── EnemySpawner
 ├── Enemy
 ├── WaveManager
 ├── DropManager
 ├── GameStateManager
 ├── PlayerHUD
 ├── GameHUD
 ├── GameOverScreen
 └── VictoryScreen
```

---

## 8.2 Person 1 Calls Person 2 Code

Person 1's code needs to interact with enemy code when projectiles hit enemies.

```txt
Projectile / CombatSystem
 └── checks collision with Enemy
 └── calls Enemy.takeDamage()
 └── checks Enemy.isAlive()
```

Example:

```cpp
if (Collision::checkAABB(projectile.getBounds(), enemy.getBounds())) {
    enemy.takeDamage(projectile.getDamage());
    projectile.destroy();
}
```

---

## 8.3 Person 2 Calls Person 1 Code

Person 2's code needs to interact with player code when enemies chase or damage the player.

```txt
Enemy
 └── calls Player.getPosition()

Enemy collision system
 └── calls Player.takeDamage()
```

Example:

```cpp
Vector2 direction = player.getPosition() - enemy.getPosition();
enemy.move(direction.normalized());

if (Collision::checkAABB(enemy.getBounds(), player.getBounds())) {
    player.takeDamage(enemy.getDamage());
}
```

---

# 9. Shared Data Ownership Rules

## 9.1 Player Data

Owned by: **Person 1**

Other code can read player data through getters only.

Allowed:

```cpp
player.getPosition();
player.getHP();
player.getLevel();
```

Not allowed:

```cpp
player.hp = 100;
player.position.x = 500;
```

---

## 9.2 Enemy Data

Owned by: **Person 2**

Other code can damage enemy through function calls only.

Allowed:

```cpp
enemy.takeDamage(10);
enemy.isAlive();
```

Not allowed:

```cpp
enemy.hp -= 10;
enemy.alive = false;
```

---

## 9.3 EXP Data

Owned by: **Person 1**

Enemy system can create EXP orbs, but only Player handles adding EXP.

Allowed:

```cpp
player.addEXP(orb.getAmount());
orb.collect();
```

Not allowed:

```cpp
player.exp += 10;
```

---

# 10. Suggested Folder Structure

```txt
VampireSurvivorsCPP/
├── README.md
├── CMakeLists.txt
├── assets/
│   ├── fonts/
│   ├── images/
│   └── sounds/
│
├── src/
│   ├── main.cpp
│   │
│   ├── common/
│   │   ├── Vector2.h
│   │   ├── Rect.h
│   │   ├── Collision.h
│   │   ├── Constants.h
│   │   └── Entity.h
│   │
│   ├── game/
│   │   ├── Game.h
│   │   ├── Game.cpp
│   │   ├── GameTimer.h
│   │   ├── GameTimer.cpp
│   │   ├── GameStateManager.h
│   │   └── GameStateManager.cpp
│   │
│   ├── player/
│   │   ├── Player.h
│   │   ├── Player.cpp
│   │   ├── Weapon.h
│   │   ├── Weapon.cpp
│   │   ├── Projectile.h
│   │   ├── Projectile.cpp
│   │   ├── ExperienceOrb.h
│   │   ├── ExperienceOrb.cpp
│   │   ├── Upgrade.h
│   │   ├── Upgrade.cpp
│   │   ├── UpgradeMenu.h
│   │   └── UpgradeMenu.cpp
│   │
│   ├── enemy/
│   │   ├── Enemy.h
│   │   ├── Enemy.cpp
│   │   ├── EnemySpawner.h
│   │   ├── EnemySpawner.cpp
│   │   ├── WaveManager.h
│   │   ├── WaveManager.cpp
│   │   ├── DropManager.h
│   │   └── DropManager.cpp
│   │
│   └── ui/
│       ├── PlayerHUD.h
│       ├── PlayerHUD.cpp
│       ├── GameHUD.h
│       ├── GameHUD.cpp
│       ├── GameOverScreen.h
│       ├── GameOverScreen.cpp
│       ├── VictoryScreen.h
│       └── VictoryScreen.cpp
```

---

# 11. Git Branch Suggestion

Each person should work on a separate branch.

```txt
main
├── feature/player-combat
└── feature/enemy-wave
```

Person 1 branch:

```txt
feature/player-combat
```

Person 2 branch:

```txt
feature/enemy-wave
```

When a feature is stable, merge into `main`.

---

# 12. Commit Rules

Each commit should be small and clear.

Good commit examples:

```txt
add player movement
add player hp and damage function
add projectile class
add enemy movement toward player
add enemy spawner
add player hud
add game over screen
```

Bad commit examples:

```txt
update
fix
final
stuff
```

---

# 1 Integration Plan

## Step 1 — Agree on Shared Interfaces

Before coding big features, both people should finish:

```txt
Vector2
Rect
Collision
Entity
Game
```

This avoids conflicts later.

---

## Step 2 — Person 1 Builds Player Alone

Person 1 should test:

- Player can move
- Player HP works
- Weapon shoots projectiles
- Projectiles move
- EXP bar works
- Level-up menu appears

---

## Step 3 — Person 2 Builds Enemy Alone

Person 2 should test:

- Enemy can spawn
- Enemy moves toward player
- Enemy has HP
- Wave timer works
- Game over screen works
- Enemy difficulty increases over time

---

## Step 4 — Combine Combat

After both parts work separately:

- Projectiles hit enemies
- Enemies lose HP
- Enemies die
- Enemies drop EXP
- Player collects EXP
- Player levels up

---

## Step 5 — Polish

Final polish tasks:

- Better sprites
- Better UI layout
- Sound effects
- Start menu
- Pause menu
- Game over screen
- Victory screen
- Balance enemy speed/damage/spawn rate

---

# 13. Minimum Version We Must Finish

If time is short, finish this first:

## Person 1 Minimum

- Player movement
- Player HP
- Auto weapon
- Projectile
- Projectile hits enemy
- EXP bar
- Level display

## Person 2 Minimum

- Enemy movement toward player
- Enemy spawning
- Enemy damages player
- Timer
- Game over screen

---

# 15. Final Responsibility Table

| Area | Owner | Other Person Can Use Through |
|---|---|---|
| Player | Person 1 | `Player` public functions |
| Weapon | Person 1 | `Weapon` public functions |
| Projectile | Person 1 | `Projectile` public functions |
| EXP | Person 1 | `ExperienceOrb`, `Player.addEXP()` |
| Upgrade menu | Person 1 | `UpgradeMenu` public functions |
| Enemy | Person 2 | `Enemy` public functions |
| Enemy spawning | Person 2 | `EnemySpawner` public functions |
| Wave system | Person 2 | `WaveManager` public functions |
| Game state | Person 2 | `GameStateManager` public functions |
| Timer UI | Person 2 | `GameHUD` public functions |
| Player UI | Person 1 | `PlayerHUD` public functions |
| Collision | Shared | `Collision::checkAABB()` |
| Vector math | Shared | `Vector2` |
| Base object interface | Shared | `Entity` |

---

# 16. Final Notes

The most important rule is:

```txt
Each person owns a complete feature: logic + UI.
```

Person 1 should not directly modify enemy internal variables.

Person 2 should not directly modify player internal variables.

Both people should communicate through clean public functions.

This keeps the project easier to merge, easier to debug, and easier to explain in a presentation.
