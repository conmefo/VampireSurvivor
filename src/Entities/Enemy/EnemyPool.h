#pragma once

#include "EnemyBase.h"
#include "EnemyDatabase.h"

#include <memory>
#include <string>
#include <vector>

class EnemyPool {
public:
  explicit EnemyPool(const EnemyDatabase &enemyDatabase,
                     std::size_t initialCapacity = 0);
  ~EnemyPool() = default;

  void Prewarm(const std::string &enemyId, std::size_t count);
  EnemyBase *Acquire(const std::string &enemyId, const sf::Vector2f &position);
  EnemyBase *Acquire(const std::string &enemyId, const sf::Vector2f &position,
                     const EnemyStats &stats);
  void DeactivateAll();

  void Update(float dt, const sf::Vector2f &targetPosition);
  void Draw(sf::RenderTarget &target);

  std::vector<EnemyBase *> GetActiveEnemies();
  std::size_t GetActiveCount() const;
  std::size_t GetCapacity() const;

  void ResolveEnemyCollisions();

private:
  EnemyBase *CreateEnemy(const std::string &enemyId);

  const EnemyDatabase &m_enemyDatabase;
  std::vector<std::unique_ptr<EnemyBase>> m_enemies;
};
