#pragma once

#include "EnemyBase.h"
#include "EnemyType.h"

#include <memory>
#include <vector>

class EnemyPool
{
public:
    explicit EnemyPool(std::size_t initialCapacity = 0);
    ~EnemyPool() = default;

    void Prewarm(EnemyType type, std::size_t count);
    EnemyBase& Acquire(EnemyType type, const sf::Vector2f& position);
    EnemyBase& Acquire(EnemyType type, const sf::Vector2f& position, const EnemyStats& stats);
    void DeactivateAll();

    void Update(float dt, const sf::Vector2f& targetPosition);
    void Draw(sf::RenderTarget& target);

    std::vector<EnemyBase*> GetActiveEnemies();
    std::size_t GetActiveCount() const;
    std::size_t GetCapacity() const;

private:
    EnemyBase& CreateEnemy(EnemyType type);

    std::vector<std::unique_ptr<EnemyBase>> m_enemies;
};
