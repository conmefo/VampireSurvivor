#include "Player.h"
#include <cmath>

namespace
{
    const char* HIT_FLASH_FRAG = R"(
        uniform sampler2D texture;
        uniform vec4 flashColor;
        uniform bool isOverride;

        void main()
        {
            vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
            if (pixel.a > 0.0)
            {
                if (isOverride)
                    gl_FragColor = vec4(flashColor.rgb, pixel.a * flashColor.a);
                else
                    gl_FragColor = vec4(pixel.rgb * flashColor.rgb, pixel.a * flashColor.a);
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

Player::Player(const CharacterProfile& profile, const sf::Texture& texture, const std::vector<sf::IntRect>& frames)
    : m_animator(m_sprite),
      m_moveSpeedMultiplier(profile.GetStat("moveSpeed"))
{
    m_sprite.setTexture(texture);
    if(!frames.empty())
    {
        m_sprite.setOrigin(frames[0].width / 2.0f, frames[0].height / 2.0f);
        // m_baseScaleX = 76.0f / static_cast<float>(frames[0].width);
        // m_baseScaleY = 76.0f / static_cast<float>(frames[0].height);
        // m_sprite.setScale(m_baseScaleX, m_baseScaleY);
    }
    
    // Fallback if multiplier is somehow zero or missing
    if(m_moveSpeedMultiplier <= 0.0f)
    {
        m_moveSpeedMultiplier = 1.0f;
    }

    m_maxHealth = profile.GetStat("maxHealth");
    if(m_maxHealth <= 0.0f) 
    {
        m_maxHealth = 100.0f;
    }
    m_currentHealth = m_maxHealth;

    m_animator.Initialize(frames, ANIMATION_SPEED);
}

void Player::Update(float dt)
{
    if (m_isDead)
    {
        m_deathScaleXTweener.Update(dt);
        m_deathScaleYTweener.Update(dt);
        m_deathColorTweener.Update(dt);
        
        float signX = m_sprite.getScale().x >= 0 ? 1.0f : -1.0f;
        float scaleY = m_deathScaleYTweener.GetValue();
        m_sprite.setScale(signX * m_baseScaleX * m_deathScaleXTweener.GetValue(), m_baseScaleY * scaleY);
        
        sf::FloatRect bounds = m_sprite.getLocalBounds();
        float yOffset = (1.0f - scaleY) * (bounds.height / 2.0f) * m_baseScaleY;
        m_sprite.setPosition(m_position.x, m_position.y + yOffset);
        
        float colorVal = m_deathColorTweener.GetValue();
        m_sprite.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(255 * colorVal)));
        return;
    }

    if (m_invulnTimer > 0.0f)
    {
        m_invulnTimer -= dt;
        if (m_invulnTimer < 0.0f) m_invulnTimer = 0.0f;
    }
    if (m_flashTimer > 0.0f)
    {
        m_flashTimer -= dt;
        if (m_flashTimer < 0.0f) m_flashTimer = 0.0f;
    }

    sf::Vector2f direction(0.0f, 0.0f);

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        direction.y -= 1.0f;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        direction.y += 1.0f;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        direction.x -= 1.0f;
        m_sprite.setScale(-m_baseScaleX, m_baseScaleY); // Flip sprite left
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        direction.x += 1.0f;
        m_sprite.setScale(m_baseScaleX, m_baseScaleY); // Flip sprite right
    }

    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if(length > 0.0f)
    {
        direction.x /= length;
        direction.y /= length;
        
        m_position += direction * BASE_MOVE_SPEED * m_moveSpeedMultiplier * dt;
        m_animator.Update(dt);
        m_currentDirection = direction;
    }
    else
    {
        m_animator.Reset();
        m_currentDirection = sf::Vector2f(0.0f, 0.0f);
    }

    m_sprite.setPosition(m_position);
}

void Player::Draw(sf::RenderTarget& target)
{
    if (m_isDead)
    {
        if (sf::Shader::isAvailable())
        {
            float colorVal = m_deathColorTweener.GetValue();
            sf::Shader* shader = GetHitFlashShader();
            shader->setUniform("texture", sf::Shader::CurrentTexture);
            shader->setUniform("flashColor", sf::Glsl::Vec4(1.0f, 0.0f, 0.0f, colorVal));
            shader->setUniform("isOverride", true);
            
            // Temporarily set alpha to 255 because shader handles alpha
            sf::Color oldColor = m_sprite.getColor();
            m_sprite.setColor(sf::Color(255, 255, 255, 255));
            target.draw(m_sprite, shader);
            m_sprite.setColor(oldColor);
        }
        else
        {
            target.draw(m_sprite);
        }
        return;
    }

    if (m_currentDirection.x != 0.0f || m_currentDirection.y != 0.0f)
    {
        const int numShadows = 4;
        const float shadowSpacing = 1.3f;

        for (int i = numShadows; i >= 1; --i)
        {
            sf::Sprite shadowSprite = m_sprite;
            sf::Vector2f offset = -m_currentDirection * (shadowSpacing * static_cast<float>(i));
            shadowSprite.move(offset);

            sf::Uint8 alpha = static_cast<sf::Uint8>(180 - i * 40);
            shadowSprite.setColor(sf::Color(128, 128, 128, alpha));

            target.draw(shadowSprite);
        }
    }
    
    if (m_flashTimer > 0.0f && sf::Shader::isAvailable())
    {
        sf::Shader* shader = GetHitFlashShader();
        shader->setUniform("texture", sf::Shader::CurrentTexture);
        shader->setUniform("flashColor", sf::Glsl::Vec4(1.0f, 0.0f, 0.0f, 1.0f)); // Pure Red
        shader->setUniform("isOverride", false);
        target.draw(m_sprite, shader);
    }
    else if (m_flashTimer > 0.0f)
    {
        m_sprite.setColor(sf::Color(255, 0, 0));
        target.draw(m_sprite);
    }
    else
    {
        m_sprite.setColor(sf::Color::White);
        target.draw(m_sprite);
    }
}

void Player::TakeDamage(float amount)
{
    if (m_isDead || m_invulnTimer > 0.0f) return;

    m_currentHealth -= amount;
    if (m_currentHealth <= 0.0f)
    {
        m_currentHealth = 0.0f;
        OnHpReachedZero();
        return;
    }

    m_invulnTimer = I_FRAME_DURATION;
    m_flashTimer = HIT_FLASH_DURATION;

    if (m_onHitVfxCallback)
    {
        m_onHitVfxCallback("Blood", m_position);
    }
}

void Player::Heal(float amount)
{
    if (m_isDead) return;

    m_currentHealth += amount;
    if (m_currentHealth > m_maxHealth)
    {
        m_currentHealth = m_maxHealth;
    }
}

void Player::Revive()
{
    m_isDead = false;
    m_currentHealth = m_maxHealth;
    m_sprite.setScale(m_baseScaleX, m_baseScaleY);
    m_sprite.setColor(sf::Color::White);
    
    // Stop tweens if they are running
    m_deathScaleXTweener.Stop();
    m_deathScaleYTweener.Stop();
    m_deathColorTweener.Stop();
}

void Player::OnHpReachedZero()
{
    m_isDead = true;

    m_deathScaleXTweener.SetStartValue(1.0f);
    m_deathScaleXTweener.SetEndValue(3.5f);
    m_deathScaleXTweener.SetDuration(1.25f);
    m_deathScaleXTweener.Start();

    m_deathScaleYTweener.SetStartValue(1.0f);
    m_deathScaleYTweener.SetEndValue(0.15f);
    m_deathScaleYTweener.SetDuration(1.25f);
    m_deathScaleYTweener.Start();

    m_deathColorTweener.SetStartValue(1.0f);
    m_deathColorTweener.SetEndValue(0.0f);
    m_deathColorTweener.SetDuration(1.5f);
    m_deathColorTweener.Start();
}
