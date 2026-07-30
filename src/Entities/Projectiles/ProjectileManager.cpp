#include "ProjectileManager.h"
#include <algorithm>

ProjectileManager::ProjectileManager()
{
}

void ProjectileManager::Update(float dt)
{
    size_t count = m_projectiles.size();
    for (size_t i = 0; i < count; ++i)
    {
        if (i < m_projectiles.size() && m_projectiles[i])
        {
            m_projectiles[i]->Update(dt);
        }
    }

    m_projectiles.erase(
        std::remove_if(m_projectiles.begin(), m_projectiles.end(),
            [](const std::unique_ptr<Projectile>& p) { return !p || p->IsExpired(); }),
        m_projectiles.end()
    );

    size_t actionCount = m_delayedActions.size();
    for (size_t i = 0; i < actionCount; ++i)
    {
        if (i < m_delayedActions.size())
        {
            m_delayedActions[i].timer -= dt;
            if (m_delayedActions[i].timer <= 0.0f && m_delayedActions[i].action)
            {
                m_delayedActions[i].action();
            }
        }
    }

    m_delayedActions.erase(
        std::remove_if(m_delayedActions.begin(), m_delayedActions.end(),
            [](const DelayedAction& a) { return a.timer <= 0.0f; }),
        m_delayedActions.end()
    );
}

void ProjectileManager::QueueDelayedAction(float delay, std::function<void()> action)
{
    if (action)
    {
        m_delayedActions.push_back({std::max(0.0f, delay), action});
    }
}

void ProjectileManager::Draw(sf::RenderTarget& target) const
{
    for(const auto& proj : m_projectiles)
    {
        proj->Draw(target);
    }
}

void ProjectileManager::SpawnProjectile(TextureAtlas& atlas, const std::string& frameName, sf::Vector2f position, sf::Vector2f velocity, float duration, float power, float areaMultiplier, const std::string& hitVfxName)
{
    AssetTextureData data = atlas.GetTextureData(frameName);
    if(data.texture)
    {
        m_projectiles.push_back(std::make_unique<Projectile>(*data.texture, data.rect, position, velocity, duration, power, areaMultiplier, hitVfxName));
    }
}

void ProjectileManager::AddProjectile(std::unique_ptr<Projectile> projectile)
{
    if (projectile) {
        projectile->SetParticleManager(m_particleManager);
        m_projectiles.push_back(std::move(projectile));
    }
}

std::vector<std::pair<Projectile*, void*>> ProjectileManager::CheckCollisions(const std::vector<CollisionTarget>& targets)
{
    std::vector<std::pair<Projectile*, void*>> hits;

    for(auto& proj : m_projectiles)
    {
        if(proj->IsExpired())
        {
            continue;
        }

        sf::FloatRect projBounds = proj->GetGlobalBounds();

        for(const auto& target : targets)
        {
            if(proj->HasHitEnemy(target.entityId))
            {
                continue;
            }

            if(projBounds.intersects(target.bounds))
            {
                proj->OnHitEnemy(target.entityId);
                hits.push_back({proj.get(), target.entityId});

                if(proj->IsExpired())
                {
                    break;
                }
            }
        }
    }

    return hits;
}
