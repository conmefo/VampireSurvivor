#include "Skeleton.h"

#include <array>
#include <iostream>

namespace
{
    constexpr const char* EnemySheetPath = "assets/images/enemies/vs_enemies.png";

    const std::array<sf::IntRect, 4> SkeletonFrames = {{
        sf::IntRect(95, 1117, 28, 33),
        sf::IntRect(400, 1014, 29, 34),
        sf::IntRect(911, 880, 30, 36),
        sf::IntRect(400, 1014, 29, 34),
    }};

    sf::Texture& GetSkeletonTexture()
    {
        static sf::Texture texture;
        return texture;
    }

    bool LoadSkeletonTexture()
    {
        static bool attemptedLoad = false;
        static bool loaded = false;

        if(!attemptedLoad)
        {
            sf::Texture& texture = GetSkeletonTexture();
            loaded = texture.loadFromFile(EnemySheetPath);

            if(loaded)
            {
                texture.setSmooth(false);
            }
            else
            {
                std::cerr << "Failed to load Skeleton enemy sheet" << std::endl;
            }

            attemptedLoad = true;
        }

        return loaded;
    }
}

Skeleton::Skeleton()
    : EnemyBase(EnemyType::Skeleton),
      m_animationTimer(0.0f),
      m_frameDuration(0.16f),
      m_currentFrame(0),
      m_hasSprites(LoadSkeletonTexture())
{
    if(m_hasSprites)
    {
        ConfigureSprite();
    }
}

EnemyStats Skeleton::GetDefaultStats()
{
    EnemyStats stats;
    stats.maxHealth = 1.5f;
    stats.speed = 100.0f;
    stats.damage = 1.0f;
    stats.mass = 1.0f;
    stats.collisionRadius = 15.0f;
    stats.expYield = 2;
    return stats;
}

void Skeleton::Activate(const sf::Vector2f& position, const EnemyStats& stats)
{
    EnemyBase::Activate(position, stats);
    m_animationTimer = 0.0f;
    m_currentFrame = 0;

    if(m_hasSprites)
    {
        ConfigureSprite();
        SyncSpriteToPosition();
    }
}

void Skeleton::Update(float dt)
{
    EnemyBase::Update(dt);

    if(IsAlive() && m_hasSprites)
    {
        UpdateAnimation(dt);
        SyncSpriteToPosition();
    }
}

void Skeleton::Draw(sf::RenderTarget& target)
{
    if(!IsAlive())
    {
        return;
    }

    if(m_hasSprites)
    {
        target.draw(m_sprite);
        return;
    }

    EnemyBase::Draw(target);
}

void Skeleton::ConfigureSprite()
{
    m_sprite.setTexture(GetSkeletonTexture(), false);
    m_sprite.setTextureRect(SkeletonFrames[static_cast<std::size_t>(m_currentFrame)]);

    sf::FloatRect bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin(bounds.left + bounds.width / 2.0f,
                       bounds.top + bounds.height / 2.0f);
    m_sprite.setScale(1.5f, 1.5f);
}

void Skeleton::UpdateAnimation(float dt)
{
    m_animationTimer += dt;
    if(m_animationTimer < m_frameDuration)
    {
        return;
    }

    m_animationTimer = 0.0f;
    m_currentFrame = (m_currentFrame + 1) % static_cast<int>(SkeletonFrames.size());
    ConfigureSprite();
}

void Skeleton::SyncSpriteToPosition()
{
    m_sprite.setPosition(m_position);

    if(m_velocity.x < -0.01f)
    {
        m_sprite.setScale(-1.5f, 1.5f);
    }
    else if(m_velocity.x > 0.01f)
    {
        m_sprite.setScale(1.5f, 1.5f);
    }
}
