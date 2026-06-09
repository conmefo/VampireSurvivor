#include "EnemyPool.h"

#include "EnemyFactory.h"

EnemyPool::EnemyPool(std::size_t initialCapacity)
{
    m_enemies.reserve(initialCapacity);

    for(std::size_t i = 0; i < initialCapacity; ++i)
    {
        m_enemies.push_back(EnemyFactory::Create(EnemyType::Basic));
    }
}

void EnemyPool::Prewarm(EnemyType type, std::size_t count)
{
    m_enemies.reserve(m_enemies.size() + count);

    for(std::size_t i = 0; i < count; ++i)
    {
        m_enemies.push_back(EnemyFactory::Create(type));
    }
}

EnemyBase& EnemyPool::Acquire(EnemyType type, const sf::Vector2f& position)
{
    return Acquire(type, position, EnemyFactory::GetDefaultStats(type));
}

EnemyBase& EnemyPool::Acquire(EnemyType type, const sf::Vector2f& position, const EnemyStats& stats)
{
    for(std::unique_ptr<EnemyBase>& enemy : m_enemies)
    {
        if(enemy->GetType() == type && !enemy->IsActive())
        {
            enemy->Activate(position, stats);
            return *enemy;
        }
    }

    EnemyBase& enemy = CreateEnemy(type);
    enemy.Activate(position, stats);
    return enemy;
}

void EnemyPool::DeactivateAll()
{
    for(std::unique_ptr<EnemyBase>& enemy : m_enemies)
    {
        enemy->Deactivate();
    }
}

void EnemyPool::Update(float dt, const sf::Vector2f& targetPosition)
{
    for(std::unique_ptr<EnemyBase>& enemy : m_enemies)
    {
        if(enemy->IsAlive())
        {
            enemy->SetTarget(targetPosition);
            enemy->Update(dt);
        }
    }
}

void EnemyPool::Draw(sf::RenderTarget& target)
{
    for(std::unique_ptr<EnemyBase>& enemy : m_enemies)
    {
        enemy->Draw(target);
    }
}

std::vector<EnemyBase*> EnemyPool::GetActiveEnemies()
{
    std::vector<EnemyBase*> activeEnemies;

    for(std::unique_ptr<EnemyBase>& enemy : m_enemies)
    {
        if(enemy->IsAlive())
        {
            activeEnemies.push_back(enemy.get());
        }
    }

    return activeEnemies;
}

std::size_t EnemyPool::GetActiveCount() const
{
    std::size_t count = 0;

    for(const std::unique_ptr<EnemyBase>& enemy : m_enemies)
    {
        if(enemy->IsAlive())
        {
            ++count;
        }
    }

    return count;
}

std::size_t EnemyPool::GetCapacity() const
{
    return m_enemies.size();
}

EnemyBase& EnemyPool::CreateEnemy(EnemyType type)
{
    m_enemies.push_back(EnemyFactory::Create(type));
    return *m_enemies.back();
}
