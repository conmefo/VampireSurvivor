#include "AnimatedEnemy.h"

namespace
{
    const char* HIT_FLASH_FRAG = R"(
        uniform sampler2D texture;
        uniform vec4 flashColor;

        void main()
        {
            vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
            if (pixel.a > 0.0)
            {
                gl_FragColor = vec4(flashColor.rgb, pixel.a * flashColor.a);
            }
            else
            {
                gl_FragColor = vec4(0.0);
            }
        }
    )";

    sf::Shader* GetHitFlashShader()
    {
        static sf::Shader shader;
        static bool loaded = false;
        if (!loaded)
        {
            if(sf::Shader::isAvailable())
            {
                shader.loadFromMemory(HIT_FLASH_FRAG, sf::Shader::Fragment);
            }
            loaded = true;
        }
        return &shader;
    }
}

AnimatedEnemy::AnimatedEnemy(const EnemyDefinition& definition)
    : EnemyBase(definition.id),
      m_definition(definition),
      m_currentAnimation(nullptr),
      m_animationTimer(0.0f),
      m_currentFrame(0)
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
}

void AnimatedEnemy::Draw(sf::RenderTarget& target)
{
    if(!IsAlive())
    {
        return;
    }

    if(m_currentAnimation && !m_currentAnimation->frames.empty())
    {
        if(m_hitFlash.IsFlashing() && m_hitFlash.UseTintFill() && sf::Shader::isAvailable())
        {
            sf::Shader* shader = GetHitFlashShader();
            shader->setUniform("texture", sf::Shader::CurrentTexture);
            sf::Color c = m_hitFlash.GetCurrentColor();
            shader->setUniform("flashColor", sf::Glsl::Vec4(c.r / 255.f, c.g / 255.f, c.b / 255.f, c.a / 255.f));
            target.draw(m_sprite, shader);
        }
        else
        {
            m_sprite.setColor(m_hitFlash.GetCurrentColor());
            target.draw(m_sprite);
        }
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

    const EnemyAnimationFrame& frame = m_currentAnimation->frames[static_cast<std::size_t>(m_currentFrame)];
    if(!frame.texture)
    {
        return;
    }

    m_sprite.setTexture(*frame.texture, false);
    m_sprite.setTextureRect(frame.rect);

    sf::FloatRect bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin(bounds.left + bounds.width / 2.0f,
                       bounds.top + bounds.height / 2.0f);
    m_sprite.setScale(m_definition.spriteScale, m_definition.spriteScale);
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

void AnimatedEnemy::SyncSpriteToPosition()
{
    m_sprite.setPosition(m_position);

    const float scale = m_definition.spriteScale;
    if(m_velocity.x < -0.01f)
    {
        m_sprite.setScale(-scale, scale);
    }
    else if(m_velocity.x > 0.01f)
    {
        m_sprite.setScale(scale, scale);
    }
}
