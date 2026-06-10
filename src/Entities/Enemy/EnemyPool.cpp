#include "EnemyPool.h"

#include "../../Core/Physics/Collision.h"
#include "AnimatedEnemy.h"

EnemyPool::EnemyPool(const EnemyDatabase &enemyDatabase,
                     std::size_t initialCapacity)
    : m_enemyDatabase(enemyDatabase) {
  m_enemies.reserve(initialCapacity);
}

void EnemyPool::Prewarm(const std::string &enemyId, std::size_t count) {
  m_enemies.reserve(m_enemies.size() + count);

  for (std::size_t i = 0; i < count; ++i) {
    CreateEnemy(enemyId);
  }
}

EnemyBase *EnemyPool::Acquire(const std::string &enemyId,
                              const sf::Vector2f &position) {
  const EnemyDefinition *definition = m_enemyDatabase.GetDefinition(enemyId);
  if (!definition) {
    return nullptr;
  }

  return Acquire(enemyId, position, definition->stats);
}

EnemyBase *EnemyPool::Acquire(const std::string &enemyId,
                              const sf::Vector2f &position,
                              const EnemyStats &stats) {
  for (std::unique_ptr<EnemyBase> &enemy : m_enemies) {
    if (enemy->GetDefinitionId() == enemyId && !enemy->IsActive()) {
      enemy->Activate(position, stats);
      return enemy.get();
    }
  }

  EnemyBase *enemy = CreateEnemy(enemyId);
  if (enemy) {
    enemy->Activate(position, stats);
  }

  return enemy;
}

void EnemyPool::DeactivateAll() {
  for (std::unique_ptr<EnemyBase> &enemy : m_enemies) {
    enemy->Deactivate();
  }
}

void EnemyPool::Update(float dt, const sf::Vector2f &targetPosition) {
  for (std::unique_ptr<EnemyBase> &enemy : m_enemies) {
    if (enemy->IsAlive()) {
      enemy->SetTarget(targetPosition);
      enemy->Update(dt);
    }
  }
}

void EnemyPool::Draw(sf::RenderTarget &target) {
  for (std::unique_ptr<EnemyBase> &enemy : m_enemies) {
    enemy->Draw(target);
  }
}

std::vector<EnemyBase *> EnemyPool::GetActiveEnemies() {
  std::vector<EnemyBase *> activeEnemies;

  for (std::unique_ptr<EnemyBase> &enemy : m_enemies) {
    if (enemy->IsAlive()) {
      activeEnemies.push_back(enemy.get());
    }
  }

  return activeEnemies;
}

std::size_t EnemyPool::GetActiveCount() const {
  std::size_t count = 0;

  for (const std::unique_ptr<EnemyBase> &enemy : m_enemies) {
    if (enemy->IsAlive()) {
      ++count;
    }
  }

  return count;
}

std::size_t EnemyPool::GetCapacity() const { return m_enemies.size(); }

void EnemyPool::ResolveEnemyCollisions() {
  std::vector<EnemyBase *> enemies = GetActiveEnemies();

  for (std::size_t i = 0; i < enemies.size(); ++i) {
    for (std::size_t j = i + 1; j < enemies.size(); ++j) {
      EnemyBase *a = enemies[i];
      EnemyBase *b = enemies[j];

      const sf::Vector2f separation = Collision::GetCircleSeparation(
          a->GetPosition(), a->GetCollisionRadius(), b->GetPosition(),
          b->GetCollisionRadius());
      if (separation.x == 0.0f && separation.y == 0.0f) {
        continue;
      }

      a->ApplyKnockback(separation * 0.5f);
      b->ApplyKnockback(-separation * 0.5f);
    }
  }
}

EnemyBase *EnemyPool::CreateEnemy(const std::string &enemyId) {
  const EnemyDefinition *definition = m_enemyDatabase.GetDefinition(enemyId);
  if (!definition) {
    return nullptr;
  }

  m_enemies.push_back(std::make_unique<AnimatedEnemy>(*definition));
  return m_enemies.back().get();
}
