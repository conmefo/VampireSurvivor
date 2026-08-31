#include "AnimatedEnemy.h"

#include <algorithm>

AnimatedEnemy::AnimatedEnemy(const EnemyDefinition& definition)
    : EnemyBase(definition.id),
      m_definition(definition),
      m_currentAnimation(nullptr),
      m_animationTimer(0.0f),
      m_currentFrame(0),
      m_deathAnimationTimer(0.0f),
      m_deathFrame(0),
      m_facingSign(1.0f)
{
    auto found = m_definition.animations.find("idle");
    if(found != m_definition.animations.end())
    {
        m_currentAnimation = &found->second;
        ApplyFrame();
    }
}

void AnimatedEnemy::Activate(const sf::Vector2f& position, const EnemyStats& stats)
{
    EnemyBase::Activate(position, stats);
    m_animationTimer = 0.0f;
    m_currentFrame = 0;
    m_deathAnimationTimer = 0.0f;
    m_deathFrame = 0;
    m_facingSign = 1.0f;
    ApplyFrame();
    SyncSpriteToPosition();
}

void AnimatedEnemy::Update(float dt)
{
    EnemyBase::Update(dt);

    if(IsAlive() && m_currentAnimation)
    {
        UpdateAnimation(dt);
        SyncSpriteToPosition();
    }
    else if(IsDying())
    {
        SyncSpriteToPosition();
    }
}

void AnimatedEnemy::Draw(sf::RenderTarget& target)
{
    if(!IsAlive() && !IsDying())
    {
        return;
    }

    const bool hasActiveSprite =
        (m_currentAnimation && !m_currentAnimation->frames.empty()) ||
        (IsDying() && !m_definition.deathAnimation.frames.empty());

    if(hasActiveSprite)
    {
        m_sprite.setColor(GetRenderColor());
        target.draw(m_sprite);
        return;
    }

    EnemyBase::Draw(target);
}

void AnimatedEnemy::ApplyFrame()
{
    if(!m_currentAnimation || m_currentAnimation->frames.empty())
    {
        return;
    }

    if(m_currentFrame >= static_cast<int>(m_currentAnimation->frames.size()))
    {
        m_currentFrame = 0;
    }

    ApplyAnimationFrame(*m_currentAnimation, m_currentFrame);
}

void AnimatedEnemy::ApplyDeathFrame()
{
    ApplyAnimationFrame(m_definition.deathAnimation, m_deathFrame);
}

void AnimatedEnemy::ApplyAnimationFrame(const EnemyAnimationDefinition& animation, int frameIndex)
{
    if(animation.frames.empty())
    {
        return;
    }

    if(frameIndex < 0 || frameIndex >= static_cast<int>(animation.frames.size()))
    {
        return;
    }

    const EnemyAnimationFrame& frame = animation.frames[static_cast<std::size_t>(frameIndex)];
    if(!frame.texture)
    {
        return;
    }

    m_sprite.setTexture(*frame.texture, false);
    m_sprite.setTextureRect(frame.rect);

    sf::FloatRect bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin(bounds.left + bounds.width / 2.0f,
                       bounds.top + bounds.height);
    // Note: scale is intentionally NOT set here.
    // SyncSpriteToPosition() owns the scale and runs after every frame update.
}

void AnimatedEnemy::UpdateAnimation(float dt)
{
    if(!m_currentAnimation || m_currentAnimation->frames.empty())
    {
        return;
    }

    m_animationTimer += dt;
    if(m_animationTimer < m_currentAnimation->frameDuration)
    {
        return;
    }

    m_animationTimer = 0.0f;
    ++m_currentFrame;

    if(m_currentFrame >= static_cast<int>(m_currentAnimation->frames.size()))
    {
        m_currentFrame = m_currentAnimation->isLooping
            ? 0
            : static_cast<int>(m_currentAnimation->frames.size()) - 1;
    }

    ApplyFrame();
}

void AnimatedEnemy::UpdateDeath(float dt)
{
    if(m_definition.deathAnimation.frames.empty())
    {
        EnemyBase::UpdateDeath(dt);
        return;
    }

    if(m_deathFrame == 0 && m_deathAnimationTimer == 0.0f)
    {
        ApplyDeathFrame();
    }

    const float frameDuration = std::max(m_definition.deathAnimation.frameDuration, 0.001f);
    m_deathAnimationTimer += dt;
    while(m_deathAnimationTimer >= frameDuration)
    {
        m_deathAnimationTimer -= frameDuration;
        ++m_deathFrame;

        if(m_deathFrame >= static_cast<int>(m_definition.deathAnimation.frames.size()))
        {
            Deactivate();
            return;
        }

        ApplyDeathFrame();
    }

    m_position += m_velocity * dt;
    m_velocity *= std::max(0.0f, 1.0f - dt * 6.0f);
    SyncSpriteToPosition();
    SyncBodyToPosition();
}

void AnimatedEnemy::SyncSpriteToPosition()
{
    m_sprite.setPosition(m_position);

    if(m_velocity.x < -0.01f)
    {
        m_facingSign = 1.0f;
    }
    else if(m_velocity.x > 0.01f)
    {
        m_facingSign = -1.0f;
    }
    // Always apply the last-known facing so the sprite never snaps when idle
    m_sprite.setScale(m_facingSign * m_definition.spriteScale, m_definition.spriteScale);
}
