# Vampire Survivors Clone

# Project Structure
VampireSurvivorsClone/
├── CMakeLists.txt                  # Cấu hình biên dịch (Build configuration)
├── data/                           # Dữ liệu tĩnh và file lưu game (Persistent Data)
│   ├── save_slots/                 # .dat files cho Mid-game state
│   └── configs/                    # JSON/CSV cho Weapon Synergy & Shop
├── assets/                         # Tài nguyên đồ họa/âm thanh (Static Assets)
│   ├── textures/                   # Spritesheets (Flyweight Pattern data)
│   ├── audio/                      # .wav (SFX) và .ogg (BGM)
│   └── fonts/                      
├── include/                        # Giao diện và khai báo (Header files / Interfaces)
│   ├── Core/                       # Nền tảng vật lý và thuật toán (Engine Core)
│   │   ├── Quadtree.hpp            # Phân hoạch không gian (Spatial Partitioning)
│   │   ├── PhysicsUtils.hpp        # Tính toán va chạm (AABB, Circle-Circle)
│   │   └── Events/                 # Hệ thống sự kiện (Observer Pattern)
│   │       ├── Observer.hpp
│   │       ├── Subject.hpp
│   │       └── EventData.hpp       
│   ├── Entities/                   # Lớp thực thể cốt lõi (Domain/Entity Layer)
│   │   ├── Agent.hpp               # Base class cho mọi thực thể động
│   │   ├── Player/
│   │   │   ├── PlayerBase.hpp
│   │   │   ├── CharacterFactory.hpp# Khởi tạo đa hình (Polymorphic initialization)
│   │   │   └── Classes/            # Các class cụ thể (Tank, Mage, Gambler)
│   │   └── Enemy/
│   │       ├── EnemyBase.hpp
│   │       ├── EnemySharedData.hpp # Flyweight object (Tiết kiệm RAM)
│   │       ├── Subclasses/         # Bat, Zombie, Boss
│   │       └── AI/                 # Hệ thống chiến thuật (Strategy Pattern)
│   │           ├── MovementStrategy.hpp
│   │           └── BossFSM.hpp     # Multi-phase Boss logic
│   ├── Weapons/                    # Hệ thống vũ khí (Arsenal Sub-system)
│   │   ├── WeaponBase.hpp
│   │   ├── Projectile.hpp          # Tách biệt (Decoupled) khỏi Weapon
│   │   ├── SynergyDict.hpp         # Từ điển tiến hóa (Weapon Evolution)
│   │   └── Implementations/        # MagicWand, Garlic, v.v.
│   ├── Systems/                    # Lớp điều khiển (System/Control Layer)
│   │   ├── GameManager.hpp         # Quản lý vòng lặp (Game Loop)
│   │   ├── ProfileManager.hpp      # Meta-progression Shop (Singleton Pattern)
│   │   ├── ObjectPool.hpp          # Tái sử dụng bộ nhớ (Object Pool Pattern)
│   │   ├── StatsManager.hpp        # Quản lý chỉ số (Data Aggregation)
│   │   └── AssetManager.hpp        # Quản lý tài nguyên (Resource Handler)
│   ├── States/                     # Quản lý luồng trò chơi (FSM State Pattern)
│   │   ├── GameState.hpp           # Abstract state
│   │   ├── StateMachine.hpp        # LIFO Stack controller
│   │   └── Contexts/               # PlayingState, PauseState, LevelUpState
│   └── UI/                         # Lớp giao diện (Boundary/Presentation Layer)
│       ├── UIManager.hpp           # Lắng nghe sự kiện (Event-driven UI)
│       └── Components/             # HealthBar, ExpBar, FloatingText
└── src/                            # Mã nguồn cài đặt (Source implementations)
    ├── Core/                       # Cấu trúc y hệt thư mục include/
    ├── Entities/
    ├── Weapons/
    ├── Systems/
    ├── States/
    ├── UI/
    └── main.cpp                    # Điểm khởi chạy (Entry point)