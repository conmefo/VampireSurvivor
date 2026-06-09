#include "Bat1.h"

#include <array>
#include <iostream>

namespace
{
    constexpr const char* BatFrameOnePath = "assets/images/enemies/bat1_1.png";
    constexpr const char* BatFrameTwoPath = "assets/images/enemies/bat1_2.png";

    std::array<sf::Texture, 2>& GetBatTextures()
    {
        static std::array<sf::Texture, 2> textures;
        return textures;
    }

    bool LoadBatTextures()
    {
        static bool attemptedLoad = false;
        static bool loaded = false;

        if(!attemptedLoad)
        {
            std::array<sf::Texture, 2>& textures = GetBatTextures();
            loaded = textures[0].loadFromFile(BatFrameOnePath) &&
                     textures[1].loadFromFile(BatFrameTwoPath);

            if(loaded)
            {
                textures[0].setSmooth(false);
                textures[1].setSmooth(false);
            }
            else
            {
                std::cerr << "Failed to load Bat1 enemy frames" << std::endl;
            }

            attemptedLoad = true;
        }

        return loaded;
    }
}

Bat1::Bat1()
    : EnemyBase(EnemyType::Bat1),
      m_animationTimer(0.0f),
      m_frameDuration(0.16f),
      m_currentFrame(0),
      m_hasSprites(LoadBatTextures())
{
    if(m_hasSprites)
    {
        ConfigureSprite();
    }
}

EnemyStats Bat1::GetDefaultStats()
{
    EnemyStats stats;
    stats.maxHealth = 8.0f;
    stats.speed = 95.0f;
    stats.damage = 1.0f;
    stats.mass = 0.7f;
    stats.collisionRadius = 13.0f;
    stats.expYield = 1;
    return stats;
}

void Bat1::Activate(const sf::Vector2f& position, const EnemyStats& stats)
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

void Bat1::Update(float dt)
{
    EnemyBase::Update(dt);

    if(IsAlive() && m_hasSprites)
    {
        UpdateAnimation(dt);
        SyncSpriteToPosition();
    }
}

void Bat1::Draw(sf::RenderTarget& target)
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

void Bat1::ConfigureSprite()
{
    std::array<sf::Texture, 2>& textures = GetBatTextures();
    m_sprite.setTexture(textures[static_cast<std::size_t>(m_currentFrame)], true);

    sf::FloatRect bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin(bounds.left + bounds.width / 2.0f,
                       bounds.top + bounds.height / 2.0f);
    m_sprite.setScale(1.5f, 1.5f);
}

void Bat1::UpdateAnimation(float dt)
{
    m_animationTimer += dt;
    if(m_animationTimer < m_frameDuration)
    {
        return;
    }

    m_animationTimer = 0.0f;
    m_currentFrame = (m_currentFrame + 1) % 2;
    ConfigureSprite();
}

void Bat1::SyncSpriteToPosition()
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
