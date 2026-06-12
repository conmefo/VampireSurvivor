# Vampire Survivors Clone

## Project Structure

```
VampireSurvivorsClone/
├── CMakeLists.txt                  # Cấu hình biên dịch (Build configuration)
├── data/                           # Dữ liệu tĩnh và file lưu game (Persistent Data)
│   ├── save_slots/                 # .dat files cho Mid-game state
│   └── configs/                    # JSON/CSV cho Weapon Synergy & Shop
├── assets/                         # Tài nguyên đồ họa/âm thanh (Static Assets)
│   ├── textures/                   # Spritesheets (Flyweight Pattern data)
│   ├── audio/                      # .wav (SFX) và .ogg (BGM)
│   └── fonts/                      
├── src/                            # Mã nguồn dự án (Headers & Sources)
│   ├── main.cpp                    # Điểm khởi chạy (Entry point)
│   ├── Core/                       # Nền tảng vật lý và thuật toán (Engine Core)
│   │   ├── Quadtree.h              # Phân hoạch không gian (Spatial Partitioning)
│   │   ├── Quadtree.cpp
│   │   ├── PhysicsUtils.h          # Tính toán va chạm (AABB, Circle-Circle)
│   │   ├── PhysicsUtils.cpp    
│   │   └── Events/                 # Hệ thống sự kiện (Observer Pattern)
│   │       ├── Observer.h
│   │       ├── Subject.h
│   │       └── EventData.h
│   ├── Entities/                   # Lớp thực thể cốt lõi (Domain/Entity Layer)
│   │   ├── Agent.h                 # Base class cho mọi thực thể động
│   │   ├── Agent.cpp
│   │   ├── Player/
│   │   │   ├── PlayerBase.h
│   │   │   ├── PlayerBase.cpp
│   │   │   ├── CharacterFactory.h  # Khởi tạo đa hình (Polymorphic initialization)
│   │   │   ├── CharacterFactory.cpp
│   │   │   └── Classes/            # Các class cụ thể (Tank, Mage, Gambler)
│   │   └── Enemy/
│   │       ├── EnemyBase.h
│   │       ├── EnemyBase.cpp
│   │       ├── EnemySharedData.h   # Flyweight object (Tiết kiệm RAM)
│   │       ├── EnemySharedData.cpp
│   │       ├── Subclasses/         # Bat, Zombie, Boss
│   │       └── AI/                 # Hệ thống chiến thuật (Strategy Pattern)
│   │           ├── MovementStrategy.h
│   │           ├── MovementStrategy.cpp
│   │           ├── BossFSM.h       # Multi-phase Boss logic
│   │           └── BossFSM.cpp
│   ├── Weapons/                    # Hệ thống vũ khí (Arsenal Sub-system)
│   │   ├── WeaponBase.h
│   │   ├── WeaponBase.cpp
│   │   ├── Projectile.h            # Tách biệt (Decoupled) khỏi Weapon
│   │   ├── Projectile.cpp
│   │   ├── SynergyDict.h           # Từ điển tiến hóa (Weapon Evolution)
│   │   └── Implementations/        # MagicWand, Garlic, v.v.
│   ├── Systems/                    # Lớp điều khiển (System/Control Layer)
│   │   ├── GameManager.h           # Quản lý vòng lặp (Game Loop)
│   │   ├── GameManager.cpp
│   │   ├── ProfileManager.h        # Meta-progression Shop (Singleton Pattern)
│   │   ├── ProfileManager.cpp
│   │   ├── ObjectPool.h            # Tái sử dụng bộ nhớ (Object Pool Pattern)
│   │   ├── StatsManager.h          # Quản lý chỉ số (Data Aggregation)
│   │   ├── StatsManager.cpp
│   │   ├── AssetManager.h          # Quản lý tài nguyên (Resource Handler)
│   │   └── AssetManager.cpp
│   ├── States/                     # Quản lý luồng trò chơi (FSM State Pattern)
│   │   ├── GameState.h             # Abstract state
│   │   ├── StateMachine.h          # LIFO Stack controller
│   │   ├── StateMachine.cpp
│   │   └── Contexts/               # PlayingState, PauseState, LevelUpState
│   └── UI/                         # Lớp giao diện (Boundary/Presentation Layer)
│       ├── UIManager.h             # Lắng nghe sự kiện (Event-driven UI)
│       ├── UIManager.cpp
│       └── Components/             # HealthBar, ExpBar, FloatingText
```