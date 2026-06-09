#include "EnemyFactory.h"

#include "Bat1.h"

std::unique_ptr<EnemyBase> EnemyFactory::Create(EnemyType type)
{
    switch(type)
    {
    case EnemyType::Bat1:
        return std::make_unique<Bat1>();
    case EnemyType::Basic:
    default:
        return std::make_unique<EnemyBase>();
    }
}

EnemyStats EnemyFactory::GetDefaultStats(EnemyType type)
{
    switch(type)
    {
    case EnemyType::Bat1:
        return Bat1::GetDefaultStats();
    case EnemyType::Basic:
    default:
        return EnemyStats();
    }
}
