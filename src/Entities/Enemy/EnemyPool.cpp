#include "EnemyPool.h"

#include <algorithm>
#include <cmath>
#include <iostream>

namespace
{
constexpr float Pi = 3.14159265358979323846f;
constexpr float MaxSeparationForce = 8.0f;
constexpr int YBucketCount = 1080;

uint32_t QuickHash(uint32_t id)
{
    id = ((id >> 16) ^ id) * 0x45d9f3b;
    id = ((id >> 16) ^ id) * 0x45d9f3b;
    id = (id >> 16) ^ id;
    return id;
}
} // namespace

void EnemyHotSoA::Reserve(std::size_t capacity)
{
    x.resize(capacity, 0.0f);
    y.resize(capacity, 0.0f);
    vx.resize(capacity, 0.0f);
    vy.resize(capacity, 0.0f);
    hp.resize(capacity, 0.0f);
    maxHp.resize(capacity, 0.0f);
    speed.resize(capacity, 0.0f);
    damage.resize(capacity, 0.0f);
    radius.resize(capacity, 14.0f);
    expYield.resize(capacity, 1.0f);
    attackRange.resize(capacity, 0.0f);
    typeId.resize(capacity, 0);
    generation.resize(capacity, 0);
    flags.resize(capacity, 0);
    deathTimer.resize(capacity, 0.0f);
    flashTimer.resize(capacity, 0.0f);
}

EnemyPool::EnemyPool(const EnemyDatabase& enemyDatabase, std::size_t initialCapacity)
    : m_enemyDatabase(enemyDatabase), m_capacity(initialCapacity), m_grid(48.0f)
{
    m_hot.Reserve(m_capacity);
    m_activeIndices.resize(m_capacity, 0);
    m_sparseToDense.resize(m_capacity, UINT32_MAX);
    m_pushX.resize(m_capacity, 0.0f);
    m_pushY.resize(m_capacity, 0.0f);
    m_renderBuffer.reserve(m_capacity);
    m_yBuckets.resize(YBucketCount);

    m_freeIndices.reserve(m_capacity);
    for (std::size_t i = m_capacity; i > 0; --i)
    {
        m_freeIndices.push_back(static_cast<uint32_t>(i - 1));
    }

    // Prewarm legacy wrappers for backwards compatibility
    m_legacyPool.reserve(m_capacity);
    for (std::size_t i = 0; i < m_capacity; ++i)
    {
        m_legacyPool.push_back(std::make_unique<EnemyBase>("BASIC"));
    }
}

EnemyPool::~EnemyPool() = default;

uint16_t EnemyPool::GetOrCreateTypeId(const std::string& enemyId, const EnemyStats* customStats)
{
    auto it = m_typeMap.find(enemyId);
    if (it != m_typeMap.end() && !customStats)
    {
        return it->second;
    }

    EnemyTypeData data;
    data.id = enemyId;

    const EnemyDefinition* def = m_enemyDatabase.GetDefinition(enemyId);
    if (def)
    {
        data.maxHealth = customStats ? customStats->maxHealth : def->stats.maxHealth;
        data.speed = customStats ? customStats->speed : def->stats.speed;
        data.damage = customStats ? customStats->damage : def->stats.damage;
        data.mass = customStats ? customStats->mass : def->stats.mass;
        data.radius = customStats ? customStats->collisionRadius : def->stats.collisionRadius;
        data.expYield = customStats ? customStats->expYield : def->stats.expYield;
        data.attackRange = customStats ? customStats->attackRange : def->stats.attackRange;
        data.spriteScale = def->spriteScale;

        // Try extracting animation frames
        auto animIt = def->animations.find("WALK");
        if (animIt == def->animations.end() && !def->animations.empty())
        {
            animIt = def->animations.begin();
        }

        if (animIt != def->animations.end())
        {
            data.frameDuration = animIt->second.frameDuration > 0.001f ? animIt->second.frameDuration : 0.1f;
            for (const auto& frame : animIt->second.frames)
            {
                if (frame.texture)
                {
                    data.texture = frame.texture;
                    data.animFrames.push_back(frame.rect);
                }
            }
        }
    }
    else if (customStats)
    {
        data.maxHealth = customStats->maxHealth;
        data.speed = customStats->speed;
        data.damage = customStats->damage;
        data.mass = customStats->mass;
        data.radius = customStats->collisionRadius;
        data.expYield = customStats->expYield;
        data.attackRange = customStats->attackRange;
    }

    const uint16_t newId = static_cast<uint16_t>(m_typeCatalog.size());
    m_typeCatalog.push_back(data);
    m_typeMap[enemyId] = newId;
    return newId;
}

void EnemyPool::Prewarm(const std::string& enemyId, std::size_t count)
{
    GetOrCreateTypeId(enemyId);
}

EnemyBase* EnemyPool::Acquire(const std::string& enemyId, const sf::Vector2f& position)
{
    return Acquire(enemyId, position, EnemyStats{});
}

EnemyBase* EnemyPool::Acquire(const std::string& enemyId, const sf::Vector2f& position, const EnemyStats& stats)
{
    if (m_freeIndices.empty() || m_activeCount >= m_capacity)
    {
        return nullptr;
    }

    const uint32_t sparseIndex = m_freeIndices.back();
    m_freeIndices.pop_back();

    const uint32_t denseIndex = m_activeCount++;
    m_activeIndices[denseIndex] = sparseIndex;
    m_sparseToDense[sparseIndex] = denseIndex;

    const bool hasCustomStats = (stats.maxHealth > 0.0f || stats.speed > 0.0f);
    const uint16_t typeId = GetOrCreateTypeId(enemyId, hasCustomStats ? &stats : nullptr);
    const EnemyTypeData& typeData = m_typeCatalog[typeId];

    m_hot.x[sparseIndex] = position.x;
    m_hot.y[sparseIndex] = position.y;
    m_hot.vx[sparseIndex] = 0.0f;
    m_hot.vy[sparseIndex] = 0.0f;
    m_hot.maxHp[sparseIndex] = hasCustomStats ? stats.maxHealth : typeData.maxHealth;
    m_hot.hp[sparseIndex] = m_hot.maxHp[sparseIndex];
    m_hot.speed[sparseIndex] = hasCustomStats ? stats.speed : typeData.speed;
    m_hot.damage[sparseIndex] = hasCustomStats ? stats.damage : typeData.damage;
    m_hot.radius[sparseIndex] = hasCustomStats ? stats.collisionRadius : typeData.radius;
    m_hot.expYield[sparseIndex] = hasCustomStats ? stats.expYield : typeData.expYield;
    m_hot.attackRange[sparseIndex] = hasCustomStats ? stats.attackRange : typeData.attackRange;
    m_hot.typeId[sparseIndex] = typeId;
    m_hot.generation[sparseIndex]++;
    m_hot.flags[sparseIndex] = 1; // Active bit
    m_hot.deathTimer[sparseIndex] = 0.0f;
    m_hot.flashTimer[sparseIndex] = 0.0f;

    // Sync legacy wrapper
    if (sparseIndex < m_legacyPool.size())
    {
        EnemyBase* legacy = m_legacyPool[sparseIndex].get();
        EnemyStats s = stats;
        if (!hasCustomStats)
        {
            s.maxHealth = typeData.maxHealth;
            s.speed = typeData.speed;
            s.damage = typeData.damage;
            s.mass = typeData.mass;
            s.collisionRadius = typeData.radius;
            s.expYield = typeData.expYield;
        }
        legacy->Activate(position, s);
        return legacy;
    }

    return nullptr;
}

void EnemyPool::DeactivateIndex(uint32_t denseIdx)
{
    if (denseIdx >= m_activeCount)
    {
        return;
    }

    const uint32_t deadSparse = m_activeIndices[denseIdx];
    const uint32_t lastDense = m_activeCount - 1;
    const uint32_t lastSparse = m_activeIndices[lastDense];

    m_activeIndices[denseIdx] = lastSparse;
    m_sparseToDense[lastSparse] = denseIdx;
    m_sparseToDense[deadSparse] = UINT32_MAX;

    m_activeCount--;
    m_freeIndices.push_back(deadSparse);
    m_hot.flags[deadSparse] = 0;

    if (deadSparse < m_legacyPool.size())
    {
        m_legacyPool[deadSparse]->Deactivate();
    }
}

void EnemyPool::Clear()
{
    DeactivateAll();
}

void EnemyPool::DeactivateAll()
{
    for (uint32_t i = 0; i < m_activeCount; ++i)
    {
        const uint32_t sparse = m_activeIndices[i];
        m_sparseToDense[sparse] = UINT32_MAX;
        m_hot.flags[sparse] = 0;
        if (sparse < m_legacyPool.size())
        {
            m_legacyPool[sparse]->Deactivate();
        }
    }

    m_activeCount = 0;
    m_freeIndices.clear();
    for (std::size_t i = m_capacity; i > 0; --i)
    {
        m_freeIndices.push_back(static_cast<uint32_t>(i - 1));
    }
}

void EnemyPool::Update(float dt, const sf::Vector2f& targetPosition)
{
    m_globalTime += dt;
    if (m_activeCount == 0)
    {
        return;
    }

    // Step 1: High-Performance Movement towards target
    float minX = 1e9f, minY = 1e9f, maxX = -1e9f, maxY = -1e9f;
    for (uint32_t i = 0; i < m_activeCount; ++i)
    {
        const uint32_t idx = m_activeIndices[i];
        if (!(m_hot.flags[idx] & 1))
        {
            continue;
        }

        // Dying state check
        if (m_hot.flags[idx] & 2)
        {
            m_hot.deathTimer[idx] += dt;
            if (m_hot.deathTimer[idx] >= 0.35f)
            {
                DeactivateIndex(i);
                --i;
                continue;
            }
            continue;
        }

        // Flashing damage timer
        if (m_hot.flashTimer[idx] > 0.0f)
        {
            m_hot.flashTimer[idx] = std::max(0.0f, m_hot.flashTimer[idx] - dt);
        }

        const float dx = targetPosition.x - m_hot.x[idx];
        const float dy = targetPosition.y - m_hot.y[idx];
        const float distSq = dx * dx + dy * dy;

        const float attackRange = m_hot.attackRange[idx];
        const bool holdingAttackRange = attackRange > 0.0f && distSq <= attackRange * attackRange;

        if (distSq > 0.001f && !holdingAttackRange)
        {
            const float invDist = 1.0f / std::sqrt(distSq);
            const float speed = m_hot.speed[idx];
            m_hot.vx[idx] = (dx * invDist) * speed;
            m_hot.vy[idx] = (dy * invDist) * speed;
        }
        else
        {
            m_hot.vx[idx] = 0.0f;
            m_hot.vy[idx] = 0.0f;
        }

        minX = std::min(minX, m_hot.x[idx]);
        minY = std::min(minY, m_hot.y[idx]);
        maxX = std::max(maxX, m_hot.x[idx]);
        maxY = std::max(maxY, m_hot.y[idx]);
    }

    if (m_activeCount == 0)
    {
        return;
    }

    // Expand bounds for spatial grid
    const sf::FloatRect worldBounds(minX - 100.0f, minY - 100.0f, (maxX - minX) + 200.0f, (maxY - minY) + 200.0f);

    // Step 2: Rebuild Broadphase Uniform Spatial Grid (Count -> Prefix Sum -> Scatter)
    std::vector<float> activeX(m_activeCount);
    std::vector<float> activeY(m_activeCount);
    for (uint32_t i = 0; i < m_activeCount; ++i)
    {
        const uint32_t idx = m_activeIndices[i];
        activeX[i] = m_hot.x[idx];
        activeY[i] = m_hot.y[idx];
    }
    m_grid.Rebuild(activeX.data(), activeY.data(), m_activeCount, worldBounds);

    // Step 3: Two-Pass Crowd Separation Solver
    std::fill(m_pushX.begin(), m_pushX.end(), 0.0f);
    std::fill(m_pushY.begin(), m_pushY.end(), 0.0f);

    const int gridW = m_grid.GetGridWidth();
    const int gridH = m_grid.GetGridHeight();
    const auto& entityIndices = m_grid.GetEntityIndices();

    for (int cy = 0; cy < gridH; ++cy)
    {
        for (int cx = 0; cx < gridW; ++cx)
        {
            const GridCell cellA = m_grid.GetCell(cx, cy);
            if (cellA.count == 0)
            {
                continue;
            }

            // Same cell comparisons (j > i)
            for (uint32_t i = 0; i < cellA.count; ++i)
            {
                const uint32_t denseA = entityIndices[cellA.begin + i];
                const uint32_t sparseA = m_activeIndices[denseA];
                const float radA = m_hot.radius[sparseA];

                for (uint32_t j = i + 1; j < cellA.count; ++j)
                {
                    const uint32_t denseB = entityIndices[cellA.begin + j];
                    const uint32_t sparseB = m_activeIndices[denseB];

                    const float dx = m_hot.x[sparseB] - m_hot.x[sparseA];
                    const float dy = m_hot.y[sparseB] - m_hot.y[sparseA];
                    const float minDistance = radA + m_hot.radius[sparseB];
                    const float distSq = dx * dx + dy * dy;

                    if (distSq < minDistance * minDistance && distSq > 0.0001f)
                    {
                        const float dist = std::sqrt(distSq);
                        const float overlap = (minDistance - dist) * 0.5f;
                        const float pushX = (dx / dist) * overlap;
                        const float pushY = (dy / dist) * overlap;

                        m_pushX[sparseA] -= pushX;
                        m_pushY[sparseA] -= pushY;
                        m_pushX[sparseB] += pushX;
                        m_pushY[sparseB] += pushY;
                    }
                }
            }

            // Neighbor cells (Check 4 directions to avoid processing pairs twice: right, bottom-left, bottom, bottom-right)
            static const int neighborOffsets[4][2] = {{1, 0}, {-1, 1}, {0, 1}, {1, 1}};
            for (const auto& offset : neighborOffsets)
            {
                const int ncx = cx + offset[0];
                const int ncy = cy + offset[1];
                if (ncx < 0 || ncx >= gridW || ncy < 0 || ncy >= gridH)
                {
                    continue;
                }

                const GridCell cellB = m_grid.GetCell(ncx, ncy);
                for (uint32_t i = 0; i < cellA.count; ++i)
                {
                    const uint32_t denseA = entityIndices[cellA.begin + i];
                    const uint32_t sparseA = m_activeIndices[denseA];
                    const float radA = m_hot.radius[sparseA];

                    for (uint32_t j = 0; j < cellB.count; ++j)
                    {
                        const uint32_t denseB = entityIndices[cellB.begin + j];
                        const uint32_t sparseB = m_activeIndices[denseB];

                        const float dx = m_hot.x[sparseB] - m_hot.x[sparseA];
                        const float dy = m_hot.y[sparseB] - m_hot.y[sparseA];
                        const float minDistance = radA + m_hot.radius[sparseB];
                        const float distSq = dx * dx + dy * dy;

                        if (distSq < minDistance * minDistance && distSq > 0.0001f)
                        {
                            const float dist = std::sqrt(distSq);
                            const float overlap = (minDistance - dist) * 0.5f;
                            const float pushX = (dx / dist) * overlap;
                            const float pushY = (dy / dist) * overlap;

                            m_pushX[sparseA] -= pushX;
                            m_pushY[sparseA] -= pushY;
                            m_pushX[sparseB] += pushX;
                            m_pushY[sparseB] += pushY;
                        }
                    }
                }
            }
        }
    }

    // Step 4: Apply position updates and clamped crowd push forces
    for (uint32_t i = 0; i < m_activeCount; ++i)
    {
        const uint32_t idx = m_activeIndices[i];
        if (!(m_hot.flags[idx] & 1) || (m_hot.flags[idx] & 2))
        {
            continue;
        }

        float pX = std::clamp(m_pushX[idx], -MaxSeparationForce, MaxSeparationForce);
        float pY = std::clamp(m_pushY[idx], -MaxSeparationForce, MaxSeparationForce);

        m_hot.x[idx] += (m_hot.vx[idx] * dt) + pX;
        m_hot.y[idx] += (m_hot.vy[idx] * dt) + pY;
    }

    // Process queued damage and death events
    ProcessDamageEvents();
    ProcessDeathEvents();
    SyncLegacyEnemies();
}

void EnemyPool::QueueDamage(uint32_t index, float damage, sf::Vector2f direction)
{
    if (index >= m_capacity || !(m_hot.flags[index] & 1) || (m_hot.flags[index] & 2))
    {
        return;
    }
    m_damageEvents.push_back({index, m_hot.generation[index], damage, direction});
}

void EnemyPool::QueueDamageByPointer(EnemyBase* enemyPtr, float damage, sf::Vector2f direction)
{
    if (!enemyPtr)
    {
        return;
    }
    for (uint32_t i = 0; i < m_activeCount; ++i)
    {
        const uint32_t sparse = m_activeIndices[i];
        if (m_legacyPool[sparse].get() == enemyPtr)
        {
            QueueDamage(sparse, damage, direction);
            return;
        }
    }
}

bool EnemyPool::ApplyDamageByPointer(EnemyBase* enemyPtr, float damage, sf::Vector2f direction, float knockbackForce)
{
    if (!enemyPtr)
    {
        return false;
    }

    for (uint32_t i = 0; i < m_activeCount; ++i)
    {
        const uint32_t sparse = m_activeIndices[i];
        if (m_legacyPool[sparse].get() == enemyPtr)
        {
            return ApplyDamageAtIndex(sparse, damage, direction, knockbackForce);
        }
    }

    return false;
}

void EnemyPool::SetOnEnemyDeath(std::function<void(EnemyBase*, const sf::Vector2f&, float)> callback)
{
    m_onEnemyDeath = std::move(callback);
}

bool EnemyPool::ApplyDamageAtIndex(uint32_t index, float damage, const sf::Vector2f& direction, float knockbackForce)
{
    if (index >= m_capacity || !(m_hot.flags[index] & 1) || (m_hot.flags[index] & 2) ||
        index >= m_legacyPool.size())
    {
        return false;
    }

    EnemyBase* legacy = m_legacyPool[index].get();
    const bool killed = legacy->TakeDamage(damage, direction);
    m_hot.hp[index] = legacy->GetHealth();
    m_hot.flashTimer[index] = 0.12f;

    if (!killed)
    {
        const float directionLengthSquared = direction.x * direction.x + direction.y * direction.y;
        if (directionLengthSquared > 0.001f && knockbackForce > 0.0f)
        {
            const float inverseLength = 1.0f / std::sqrt(directionLengthSquared);
            legacy->ApplyKnockback(direction * (inverseLength * knockbackForce));
            const sf::Vector2f position = legacy->GetPosition();
            m_hot.x[index] = position.x;
            m_hot.y[index] = position.y;
        }
        return false;
    }

    m_hot.flags[index] |= 2;
    const sf::Vector2f deathPosition = legacy->GetPosition();
    m_hot.x[index] = deathPosition.x;
    m_hot.y[index] = deathPosition.y;
    m_deathEvents.push_back({index, deathPosition, m_hot.expYield[index], false});
    if (m_onEnemyDeath)
    {
        m_onEnemyDeath(legacy, deathPosition, m_hot.expYield[index]);
    }
    return true;
}

void EnemyPool::ProcessDamageEvents()
{
    if (m_damageEvents.empty())
    {
        return;
    }

    for (const auto& evt : m_damageEvents)
    {
        const uint32_t idx = evt.index;
        if (idx >= m_capacity || !(m_hot.flags[idx] & 1) || m_hot.generation[idx] != evt.generation)
        {
            continue;
        }

        ApplyDamageAtIndex(idx, evt.damage, evt.hitDirection, 8.0f);
    }
    m_damageEvents.clear();
}

void EnemyPool::ProcessDeathEvents()
{
    m_deathEvents.clear();
}

void EnemyPool::SyncLegacyEnemies()
{
    for (uint32_t i = 0; i < m_activeCount; ++i)
    {
        const uint32_t sparse = m_activeIndices[i];
        if (sparse < m_legacyPool.size())
        {
            EnemyBase* legacy = m_legacyPool[sparse].get();
            legacy->SetPosition(sf::Vector2f(m_hot.x[sparse], m_hot.y[sparse]));
        }
    }
}

std::vector<EnemyBase*> EnemyPool::GetActiveEnemies()
{
    m_legacyActiveCache.clear();
    m_legacyActiveCache.reserve(m_activeCount);
    for (uint32_t i = 0; i < m_activeCount; ++i)
    {
        const uint32_t sparse = m_activeIndices[i];
        if (sparse < m_legacyPool.size() && (m_hot.flags[sparse] & 1) && !(m_hot.flags[sparse] & 2))
        {
            m_legacyActiveCache.push_back(m_legacyPool[sparse].get());
        }
    }
    return m_legacyActiveCache;
}

void EnemyPool::ResolveEnemyCollisions()
{
    // Handled in high-performance Update pass
}

void EnemyPool::ResolveObstacleCollisions(const std::vector<sf::FloatRect>& obstacles)
{
    if (obstacles.empty() || m_activeCount == 0)
    {
        return;
    }

    for (uint32_t i = 0; i < m_activeCount; ++i)
    {
        const uint32_t idx = m_activeIndices[i];
        if (!(m_hot.flags[idx] & 1))
        {
            continue;
        }

        const float r = m_hot.radius[idx];
        const sf::FloatRect bounds(m_hot.x[idx] - r, m_hot.y[idx] - r, r * 2.0f, r * 2.0f);

        for (const auto& obs : obstacles)
        {
            if (bounds.intersects(obs))
            {
                const float centerX = m_hot.x[idx];
                const float centerY = m_hot.y[idx];
                const float obsCenterX = obs.left + obs.width * 0.5f;
                const float obsCenterY = obs.top + obs.height * 0.5f;

                const float dx = centerX - obsCenterX;
                const float dy = centerY - obsCenterY;

                if (std::abs(dx) > std::abs(dy))
                {
                    m_hot.x[idx] += (dx > 0.0f ? 4.0f : -4.0f);
                }
                else
                {
                    m_hot.y[idx] += (dy > 0.0f ? 4.0f : -4.0f);
                }
            }
        }
    }
}

void EnemyPool::Draw(sf::RenderTarget& target)
{
    if (m_activeCount == 0)
    {
        return;
    }

    // Step 1: Extract Render Data
    m_renderBuffer.clear();
    m_renderBuffer.reserve(m_activeCount);

    for (uint32_t i = 0; i < m_activeCount; ++i)
    {
        const uint32_t idx = m_activeIndices[i];
        if (!(m_hot.flags[idx] & 1))
        {
            continue;
        }

        const uint16_t typeId = m_hot.typeId[idx];
        const EnemyTypeData& typeData = m_typeCatalog[typeId];
        const int frameCount = static_cast<int>(typeData.animFrames.size());
        uint8_t frameIdx = 0;

        if (frameCount > 1)
        {
            const uint32_t hashVal = QuickHash(idx);
            const int globalTick = static_cast<int>(m_globalTime / typeData.frameDuration);
            frameIdx = static_cast<uint8_t>((globalTick + hashVal) % frameCount);
        }

        RenderEnemy ren;
        ren.x = m_hot.x[idx];
        ren.y = m_hot.y[idx];
        ren.radius = m_hot.radius[idx];
        ren.typeId = typeId;
        ren.frame = frameIdx;
        ren.isFlashing = (m_hot.flashTimer[idx] > 0.0f);
        ren.isDying = ((m_hot.flags[idx] & 2) != 0);
        ren.deathAlpha = ren.isDying ? std::max(0.0f, 1.0f - (m_hot.deathTimer[idx] / 0.35f)) : 1.0f;
        m_renderBuffer.push_back(ren);
    }

    if (m_renderBuffer.empty())
    {
        return;
    }

    // Step 2: O(N) Bucket Y-Sorting into integer vertical buckets
    for (auto& bucket : m_yBuckets)
    {
        bucket.clear();
    }

    const sf::View currentView = target.getView();
    const sf::Vector2f viewCenter = currentView.getCenter();
    const sf::Vector2f viewSize = currentView.getSize();
    const float viewTop = viewCenter.y - viewSize.y * 0.5f;
    const float viewHeight = viewSize.y;

    for (std::size_t i = 0; i < m_renderBuffer.size(); ++i)
    {
        const float normY = (m_renderBuffer[i].y - viewTop) / viewHeight;
        const int bucketIdx = std::clamp(static_cast<int>(normY * YBucketCount), 0, YBucketCount - 1);
        m_yBuckets[bucketIdx].push_back(static_cast<uint32_t>(i));
    }

    // Step 3: Pack into Batched sf::VertexArray (Quads) per texture atlas
    m_batchedVertices.setPrimitiveType(sf::Quads);
    m_batchedVertices.clear();
    m_batchedVertices.resize(m_renderBuffer.size() * 4);

    std::size_t vertexIndex = 0;
    const sf::Texture* activeTexture = nullptr;

    for (int b = 0; b < YBucketCount; ++b)
    {
        for (uint32_t renIdx : m_yBuckets[b])
        {
            const RenderEnemy& ren = m_renderBuffer[renIdx];
            const EnemyTypeData& typeData = m_typeCatalog[ren.typeId];

            if (!typeData.texture)
            {
                continue;
            }

            activeTexture = typeData.texture;
            sf::IntRect texRect;
            if (!typeData.animFrames.empty() && ren.frame < typeData.animFrames.size())
            {
                texRect = typeData.animFrames[ren.frame];
            }
            else
            {
                texRect = sf::IntRect(0, 0, static_cast<int>(typeData.radius * 2.0f), static_cast<int>(typeData.radius * 2.0f));
            }

            const float halfW = (texRect.width * typeData.spriteScale) * 0.5f;
            const float halfH = (texRect.height * typeData.spriteScale) * 0.5f;

            sf::Color renderColor = sf::Color::White;
            if (ren.isFlashing)
            {
                renderColor = sf::Color(255, 100, 100, static_cast<sf::Uint8>(255 * ren.deathAlpha));
            }
            else
            {
                renderColor.a = static_cast<sf::Uint8>(255 * ren.deathAlpha);
            }

            const float left = ren.x - halfW;
            const float top = ren.y - halfH;
            const float right = ren.x + halfW;
            const float bottom = ren.y + halfH;

            const float texL = static_cast<float>(texRect.left);
            const float texT = static_cast<float>(texRect.top);
            const float texR = static_cast<float>(texRect.left + texRect.width);
            const float texB = static_cast<float>(texRect.top + texRect.height);

            m_batchedVertices[vertexIndex + 0] = sf::Vertex(sf::Vector2f(left, top), renderColor, sf::Vector2f(texL, texT));
            m_batchedVertices[vertexIndex + 1] = sf::Vertex(sf::Vector2f(right, top), renderColor, sf::Vector2f(texR, texT));
            m_batchedVertices[vertexIndex + 2] = sf::Vertex(sf::Vector2f(right, bottom), renderColor, sf::Vector2f(texR, texB));
            m_batchedVertices[vertexIndex + 3] = sf::Vertex(sf::Vector2f(left, bottom), renderColor, sf::Vector2f(texL, texB));

            vertexIndex += 4;
        }
    }

    if (vertexIndex > 0 && activeTexture)
    {
        target.draw(&m_batchedVertices[0], vertexIndex, sf::Quads, sf::RenderStates(activeTexture));
    }
}

void EnemyPool::DrawDebug(sf::RenderTarget& target)
{
    sf::CircleShape circle;
    circle.setFillColor(sf::Color::Transparent);
    circle.setOutlineColor(sf::Color::Red);
    circle.setOutlineThickness(1.0f);

    for (uint32_t i = 0; i < m_activeCount; ++i)
    {
        const uint32_t idx = m_activeIndices[i];
        if (!(m_hot.flags[idx] & 1))
        {
            continue;
        }

        const float r = m_hot.radius[idx];
        circle.setRadius(r);
        circle.setOrigin(r, r);
        circle.setPosition(m_hot.x[idx], m_hot.y[idx]);
        target.draw(circle);
    }
}
