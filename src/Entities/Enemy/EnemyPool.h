#pragma once

#include "EnemyBase.h"
#include "EnemyDatabase.h"
#include "../../Core/Physics/UniformSpatialGrid.h"

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Damage and Death Event structures for deferred event pipeline
struct DamageEvent
{
    uint32_t index = 0;
    uint32_t generation = 0;
    float damage = 0.0f;
    sf::Vector2f hitDirection = {0.0f, 0.0f};
};

struct DeathEvent
{
    uint32_t index = 0;
    sf::Vector2f position = {0.0f, 0.0f};
    float expYield = 1.0f;
    bool isBoss = false;
};

// Flyweight data stored once per enemy type
struct EnemyTypeData
{
    std::string id;
    float maxHealth = 10.0f;
    float speed = 60.0f;
    float damage = 1.0f;
    float mass = 1.0f;
    float radius = 14.0f;
    float expYield = 1.0f;
    float attackRange = 0.0f;
    float spriteScale = 1.0f;
    const sf::Texture* texture = nullptr;
    std::vector<sf::IntRect> animFrames;
    float frameDuration = 0.1f;
};

// High-performance Struct-of-Arrays (SoA) layout
struct EnemyHotSoA
{
    std::vector<float> x;
    std::vector<float> y;
    std::vector<float> vx;
    std::vector<float> vy;
    std::vector<float> hp;
    std::vector<float> maxHp;
    std::vector<float> speed;
    std::vector<float> damage;
    std::vector<float> radius;
    std::vector<float> expYield;
    std::vector<float> attackRange;
    std::vector<uint16_t> typeId;
    std::vector<uint32_t> generation;
    std::vector<uint8_t> flags; // Bit 0: Active, Bit 1: Dying, Bit 2: Boss
    std::vector<float> deathTimer;
    std::vector<float> flashTimer;

    void Reserve(std::size_t capacity);
};

// Extracted Render Struct for O(N) Bucket Y-Sorting
struct RenderEnemy
{
    float x;
    float y;
    float radius;
    uint16_t typeId;
    uint8_t frame;
    bool isFlashing;
    bool isDying;
    float deathAlpha;
};

class EnemyPool
{
public:
    explicit EnemyPool(const EnemyDatabase& enemyDatabase, std::size_t initialCapacity = 2000);
    ~EnemyPool();

    void Prewarm(const std::string& enemyId, std::size_t count);

    // Spawn / Acquire methods
    EnemyBase* Acquire(const std::string& enemyId, const sf::Vector2f& position);
    EnemyBase* Acquire(const std::string& enemyId, const sf::Vector2f& position, const EnemyStats& stats);
    void Clear();
    void DeactivateAll();

    // High-performance SoA Update & Batched Render
    void Update(float dt, const sf::Vector2f& targetPosition);
    void Draw(sf::RenderTarget& target);
    void DrawDebug(sf::RenderTarget& target);

    // Queries
    std::vector<EnemyBase*> GetActiveEnemies();
    std::size_t GetActiveCount() const { return m_activeCount; }
    std::size_t GetCapacity() const { return m_capacity; }

    // Collision Resolvers
    void ResolveEnemyCollisions();
    void ResolveObstacleCollisions(const std::vector<sf::FloatRect>& obstacles);

    // High-performance event-based damage & grid API
    void QueueDamage(uint32_t index, float damage, sf::Vector2f direction);
    void QueueDamageByPointer(EnemyBase* enemyPtr, float damage, sf::Vector2f direction);
    bool ApplyDamageByPointer(EnemyBase* enemyPtr, float damage, sf::Vector2f direction = {0.0f, 0.0f}, float knockbackForce = 8.0f);
    void SetOnEnemyDeath(std::function<void(EnemyBase*, const sf::Vector2f&, float)> callback);
    void ProcessDamageEvents();
    void ProcessDeathEvents();

    const UniformSpatialGrid& GetGrid() const { return m_grid; }
    const EnemyHotSoA& GetSoA() const { return m_hot; }
    uint32_t GetActiveIndexAt(std::size_t idx) const { return m_activeIndices[idx]; }

    // Synchronize legacy wrapper objects if accessed by traditional OOP code
    void SyncLegacyEnemies();

private:
    uint16_t GetOrCreateTypeId(const std::string& enemyId, const EnemyStats* customStats = nullptr);
    void DeactivateIndex(uint32_t denseIdx);
    bool ApplyDamageAtIndex(uint32_t index, float damage, const sf::Vector2f& direction, float knockbackForce);

    const EnemyDatabase& m_enemyDatabase;
    std::size_t m_capacity = 0;
    uint32_t m_activeCount = 0;

    // SoA Memory
    EnemyHotSoA m_hot;
    std::vector<uint32_t> m_activeIndices;
    std::vector<uint32_t> m_freeIndices;
    std::vector<uint32_t> m_sparseToDense;

    // Flyweight catalog
    std::vector<EnemyTypeData> m_typeCatalog;
    std::unordered_map<std::string, uint16_t> m_typeMap;

    // Broadphase spatial grid & crowd separation scratch buffers
    UniformSpatialGrid m_grid;
    std::vector<float> m_pushX;
    std::vector<float> m_pushY;

    // Event Buffers
    std::vector<DamageEvent> m_damageEvents;
    std::vector<DeathEvent> m_deathEvents;
    std::function<void(EnemyBase*, const sf::Vector2f&, float)> m_onEnemyDeath;

    // Rendering Extraction & Bucket Y-Sorting
    std::vector<RenderEnemy> m_renderBuffer;
    std::vector<std::vector<uint32_t>> m_yBuckets;
    sf::VertexArray m_batchedVertices;

    // Legacy wrappers for backwards compatibility
    std::vector<std::unique_ptr<EnemyBase>> m_legacyPool;
    std::vector<EnemyBase*> m_legacyActiveCache;
    float m_globalTime = 0.0f;
};
