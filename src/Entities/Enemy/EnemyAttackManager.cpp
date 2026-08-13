#include "EnemyAttackManager.h"

#include "EnemyBase.h"
#include "../Player.h"
#include "../../Core/Resources/TextureAtlas.h"

#include <array>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>

namespace
{
constexpr float Pi = 3.14159265358979323846f;
constexpr float DefaultBossAttackRange = 420.0f;
constexpr float DefaultBossCooldown = 3.4f;
constexpr float DefaultBossTelegraph = 0.9f;
constexpr int BossProjectileCount = 8;
constexpr float BossProjectileDamageMultiplier = 0.6f;
}

void EnemyAttackManager::Initialize(const TextureAtlas& atlas)
{
    const std::array<const char*, 4> authenticEnemySheetPaths = {
        "VS_Assets/VS_Assets/ExportedProject/Assets/Resources/spritesheets/enemies.png",
        "../VS_Assets/VS_Assets/ExportedProject/Assets/Resources/spritesheets/enemies.png",
        "assets/images/enemies/vs_enemies.png",
        "Assets/images/enemies/vs_enemies.png"};

    for(const char* path : authenticEnemySheetPaths)
    {
        if(m_authenticEnemyProjectileTexture.loadFromFile(path))
        {
            m_authenticEnemyProjectileTexture.setSmooth(false);
            m_authenticEnemyProjectileTextureReady = true;
            std::cout << "EnemyAttackManager: using authentic VS projectile asset Bullet1_0 from "
                      << path << "\n";
            break;
        }
    }

    if(m_authenticEnemyProjectileTextureReady)
    {
        m_normalProjectileTexture = &m_authenticEnemyProjectileTexture;
        m_normalProjectileRect = m_authenticEnemyProjectileRect;
        m_bossProjectileTexture = &m_authenticEnemyProjectileTexture;
        m_bossProjectileRect = m_authenticEnemyProjectileRect;
        return;
    }

    AssetTextureData normalProjectile = atlas.GetTextureData("BulletAqua");
    if(!normalProjectile.texture)
    {
        normalProjectile = atlas.GetTextureData("ProjectileBullet");
    }

    AssetTextureData bossProjectile = atlas.GetTextureData("BulletRed");
    if(!bossProjectile.texture)
    {
        bossProjectile = atlas.GetTextureData("ProjectileBullet");
    }

    m_normalProjectileTexture = normalProjectile.texture;
    m_normalProjectileRect = normalProjectile.rect;
    m_bossProjectileTexture = bossProjectile.texture;
    m_bossProjectileRect = bossProjectile.rect;
}

float EnemyAttackManager::Length(const sf::Vector2f& value)
{
    return std::sqrt(value.x * value.x + value.y * value.y);
}

sf::Vector2f EnemyAttackManager::Normalize(const sf::Vector2f& value)
{
    const float length = Length(value);
    if(length <= 0.0001f)
    {
        return sf::Vector2f(0.0f, 0.0f);
    }

    return value / length;
}

Player* EnemyAttackManager::FindNearestPlayer(
    const sf::Vector2f& position,
    const std::vector<std::unique_ptr<Player>>& players)
{
    Player* nearest = nullptr;
    float nearestDistanceSquared = std::numeric_limits<float>::max();

    for(const auto& player : players)
    {
        if(!player || player->IsDead())
        {
            continue;
        }

        const sf::Vector2f difference = player->GetCenterPosition() - position;
        const float distanceSquared = difference.x * difference.x + difference.y * difference.y;
        if(distanceSquared < nearestDistanceSquared)
        {
            nearestDistanceSquared = distanceSquared;
            nearest = player.get();
        }
    }

    return nearest;
}

void EnemyAttackManager::Clear()
{
    m_attackStates.clear();
    m_projectiles.clear();
}

void EnemyAttackManager::Update(
    float dt,
    const std::vector<EnemyBase*>& enemies,
    const std::vector<std::unique_ptr<Player>>& players,
    const std::unordered_set<EnemyBase*>& bossEnemies)
{
    dt = std::max(0.0f, dt);

    std::unordered_set<EnemyBase*> activeAttackers;
    activeAttackers.reserve(enemies.size());

    for(EnemyBase* enemy : enemies)
    {
        if(!enemy || !enemy->IsAlive())
        {
            continue;
        }

        const bool isBoss = bossEnemies.find(enemy) != bossEnemies.end();
        const bool hasRangedAttack = enemy->HasRangedAttack();
        if(!hasRangedAttack)
        {
            continue;
        }

        Player* target = FindNearestPlayer(enemy->GetCollisionCenter(), players);
        if(!target)
        {
            continue;
        }

        activeAttackers.insert(enemy);
        AttackState& state = m_attackStates[enemy];
        state.cooldown = std::max(0.0f, state.cooldown - dt);

        if(state.telegraphRemaining > 0.0f)
        {
            state.telegraphRemaining = std::max(0.0f, state.telegraphRemaining - dt);
            if(state.telegraphRemaining <= 0.0f)
            {
                if(state.bossBurst)
                {
                    FireBossBurst(*enemy);
                }
                else
                {
                    FireSingleProjectile(*enemy, state);
                }

                state.cooldown = isBoss
                    ? (enemy->GetAttackCooldown() > 0.0f ? enemy->GetAttackCooldown() : DefaultBossCooldown)
                    : enemy->GetAttackCooldown();
            }
            continue;
        }

        const sf::Vector2f enemyPosition = enemy->GetCollisionCenter();
        const sf::Vector2f targetPosition = target->GetCenterPosition();
        const float distance = Length(targetPosition - enemyPosition);
        const float attackRange = isBoss
            ? std::max(DefaultBossAttackRange, enemy->GetAttackRange())
            : enemy->GetAttackRange();

        if(distance > attackRange || state.cooldown > 0.0f)
        {
            continue;
        }

        state.aimPosition = targetPosition;
        state.bossBurst = isBoss;
        state.telegraphRemaining = isBoss
            ? std::max(DefaultBossTelegraph, enemy->GetAttackTelegraph())
            : std::max(0.15f, enemy->GetAttackTelegraph());
    }

    for(auto it = m_attackStates.begin(); it != m_attackStates.end();)
    {
        if(activeAttackers.find(it->first) == activeAttackers.end())
        {
            it = m_attackStates.erase(it);
        }
        else
        {
            ++it;
        }
    }

    for(Projectile& projectile : m_projectiles)
    {
        projectile.age += dt;
        projectile.position += projectile.velocity * dt;
        projectile.visual.setPosition(projectile.position);
        projectile.sprite.setPosition(projectile.position);

        if(projectile.age >= projectile.lifetime)
        {
            projectile.lifetime = 0.0f;
            continue;
        }

        for(const auto& player : players)
        {
            if(!player || player->IsDead())
            {
                continue;
            }

            const sf::Vector2f difference = player->GetCenterPosition() - projectile.position;
            const float hitDistance = player->GetCollisionRadius() + projectile.radius;
            if(difference.x * difference.x + difference.y * difference.y <= hitDistance * hitDistance)
            {
                player->TakeDamage(projectile.damage);
                projectile.lifetime = 0.0f;
                break;
            }
        }
    }

    m_projectiles.erase(
        std::remove_if(
            m_projectiles.begin(),
            m_projectiles.end(),
            [](const Projectile& projectile) { return projectile.lifetime <= 0.0f; }),
        m_projectiles.end());
}

void EnemyAttackManager::FireSingleProjectile(const EnemyBase& enemy, const AttackState& state)
{
    const sf::Vector2f origin = enemy.GetCollisionCenter();
    const sf::Vector2f direction = Normalize(state.aimPosition - origin);
    if(Length(direction) <= 0.0f)
    {
        return;
    }

    SpawnProjectile(
        origin,
        direction * enemy.GetProjectileSpeed(),
        enemy.GetProjectileDamage(),
        enemy.GetProjectileRadius(),
        enemy.GetProjectileLifetime(),
        sf::Color(255, 170, 70, 245),
        m_normalProjectileTexture,
        m_normalProjectileRect);
}

void EnemyAttackManager::FireBossBurst(const EnemyBase& enemy)
{
    const sf::Vector2f origin = enemy.GetCollisionCenter();
    const float speed = std::max(160.0f, enemy.GetProjectileSpeed());
    const float damage = enemy.GetProjectileDamage() * BossProjectileDamageMultiplier;

    for(int index = 0; index < BossProjectileCount; ++index)
    {
        const float angle = (2.0f * Pi * static_cast<float>(index)) /
                            static_cast<float>(BossProjectileCount);
        const sf::Vector2f direction(std::cos(angle), std::sin(angle));
        SpawnProjectile(
            origin,
            direction * speed,
            damage,
            std::max(8.0f, enemy.GetProjectileRadius()),
            std::max(5.0f, enemy.GetProjectileLifetime()),
            sf::Color(255, 70, 100, 245),
            m_bossProjectileTexture,
            m_bossProjectileRect);
    }
}

void EnemyAttackManager::SpawnProjectile(
    const sf::Vector2f& position,
    const sf::Vector2f& velocity,
    float damage,
    float radius,
    float lifetime,
    const sf::Color& color,
    const sf::Texture* texture,
    const sf::IntRect& textureRect)
{
    Projectile projectile;
    projectile.position = position;
    projectile.velocity = velocity;
    projectile.damage = std::max(0.0f, damage);
    projectile.radius = std::max(2.0f, radius);
    projectile.lifetime = std::max(0.1f, lifetime);
    projectile.visual.setRadius(projectile.radius);
    projectile.visual.setOrigin(projectile.radius, projectile.radius);
    projectile.visual.setFillColor(color);
    projectile.visual.setOutlineColor(sf::Color(80, 20, 20, 230));
    projectile.visual.setOutlineThickness(1.0f);
    projectile.visual.setPosition(position);

    if(texture && textureRect.width > 0 && textureRect.height > 0)
    {
        projectile.sprite.setTexture(*texture);
        projectile.sprite.setTextureRect(textureRect);
        const sf::FloatRect bounds = projectile.sprite.getLocalBounds();
        projectile.sprite.setOrigin(
            bounds.left + bounds.width * 0.5f,
            bounds.top + bounds.height * 0.5f);

        const float largestDimension = std::max(bounds.width, bounds.height);
        if(largestDimension > 0.0f)
        {
            const float targetSize = projectile.radius * 2.5f;
            const float scale = targetSize / largestDimension;
            projectile.sprite.setScale(scale, scale);
        }

        projectile.sprite.setRotation(
            std::atan2(velocity.y, velocity.x) * 180.0f / Pi);
        projectile.sprite.setPosition(position);
        projectile.usesSprite = true;
    }

    m_projectiles.push_back(std::move(projectile));
}

void EnemyAttackManager::Draw(sf::RenderTarget& target) const
{
    for(const Projectile& projectile : m_projectiles)
    {
        if(projectile.usesSprite)
        {
            target.draw(projectile.sprite);
        }
        else
        {
            target.draw(projectile.visual);
        }
    }
}
