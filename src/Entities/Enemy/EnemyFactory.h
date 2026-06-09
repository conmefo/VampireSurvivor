#pragma once

#include "EnemyBase.h"
#include "EnemyType.h"

#include <memory>

class EnemyFactory
{
public:
    static std::unique_ptr<EnemyBase> Create(EnemyType type);
    static EnemyStats GetDefaultStats(EnemyType type);
};
