#include "Player.h"
#include <cmath>

Player::Player(const CharacterProfile& profile, const sf::Texture& texture, const std::vector<sf::IntRect>& frames)
    : m_animator(m_sprite),
      m_moveSpeedMultiplier(profile.GetStat("moveSpeed"))
{
    m_sprite.setTexture(texture);
    if(!frames.empty())
    {
        m_sprite.setOrigin(frames[0].width / 2.0f, frames[0].height / 2.0f);
        m_baseScaleX = 76.0f / static_cast<float>(frames[0].width);
        m_baseScaleY = 76.0f / static_cast<float>(frames[0].height);
        m_sprite.setScale(m_baseScaleX, m_baseScaleY);
    }
    
    // Fallback if multiplier is somehow zero or missing
    if(m_moveSpeedMultiplier <= 0.0f)
    {
        m_moveSpeedMultiplier = 1.0f;
    }

    m_animator.Initialize(frames, ANIMATION_SPEED);
}

void Player::Update(float dt)
{
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
    if (m_currentDirection.x != 0.0f || m_currentDirection.y != 0.0f)
    {
        const int numShadows = 5;
        const float shadowSpacing = 3.0f;

        for (int i = numShadows; i >= 1; --i)
        {
            sf::Sprite shadowSprite = m_sprite;
            sf::Vector2f offset = -m_currentDirection * (shadowSpacing * static_cast<float>(i));
            shadowSprite.move(offset);

            sf::Uint8 alpha = static_cast<sf::Uint8>(140 - i * 25);
            shadowSprite.setColor(sf::Color(30, 30, 80, alpha));

            target.draw(shadowSprite);
        }
    }
    
    target.draw(m_sprite);
}
