#include "Player.h"
#include "../Core/Data/PlayerProgressionManager.h"
#include "../Core/Data/PowerUpDataManager.h"
#include "../Core/Data/WeaponProfile.h"
#include <algorithm>
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

Player::Player(const CharacterProfile& profile, const sf::Texture& texture, const std::vector<sf::IntRect>& frames, int playerIndex)
    : m_animator(m_sprite),
      m_moveSpeedMultiplier(profile.GetStat("moveSpeed")),
      m_playerIndex(playerIndex)
{
    m_sprite.setTexture(texture);
    if(!frames.empty())
    {
        // Anchor pivot to middle-bottom (feet) of the character sprite
        m_sprite.setOrigin(frames[0].width / 2.0f, static_cast<float>(frames[0].height));
    }
    
    // Fallback if multiplier is somehow zero or missing
    if(m_moveSpeedMultiplier <= 0.0f)
    {
        m_moveSpeedMultiplier = 1.0f;
    }

    m_maxHealth = profile.GetStat("maxHp");
    if(m_maxHealth <= 0.0f)
    {
        m_maxHealth = profile.GetStat("maxHealth");
    }
    if(m_maxHealth <= 0.0f)
    {
        m_maxHealth = 100.0f;
    }
    m_baseMaxHealth = m_maxHealth;
    m_currentHealth = m_maxHealth;

    m_animator.Initialize(frames, ANIMATION_SPEED);
    SetCollisionRadius(8.0f);
}

void Player::ApplyGlobalBuffs(const PlayerProgressionManager& progression, const PowerUpDataManager& powerUpData)
{
    m_mightBuff = progression.GetGlobalStatBuff("power", powerUpData);
    m_areaBuff = progression.GetGlobalStatBuff("area", powerUpData);
    m_durationBuff = progression.GetGlobalStatBuff("duration", powerUpData);
    m_cooldownBuff = -progression.GetGlobalStatBuff("cooldown", powerUpData);
    m_amountBuff = static_cast<int>(progression.GetGlobalStatBuff("amount", powerUpData));
    float moveBuff = progression.GetGlobalStatBuff("moveSpeed", powerUpData);
    m_moveSpeedMultiplier += moveBuff;

    m_growthBuff = progression.GetGlobalStatBuff("growth", powerUpData);
    m_greedBuff = progression.GetGlobalStatBuff("greed", powerUpData);
    m_curseBuff = progression.GetGlobalStatBuff("curse", powerUpData);

    float magnetPowerUpBuff = progression.GetGlobalStatBuff("magnet", powerUpData);
    if (magnetPowerUpBuff > 0.0f)
    {
        AddAttractorbLevel(magnetPowerUpBuff);
    }
}

float g_PlayerSpeedMultiplier = 0.6f;

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

    if(m_recovery > 0.0f)
    {
        Heal(m_recovery * dt);
    }

    sf::Vector2f direction(0.0f, 0.0f);

    if(!m_isMultiplayer)
    {
        // Single Player mode: allow both WASD and Arrow keys
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
            m_sprite.setScale(-m_baseScaleX, m_baseScaleY);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            direction.x += 1.0f;
            m_sprite.setScale(m_baseScaleX, m_baseScaleY);
        }
    }
    else if(m_playerIndex == 0)
    {
        // Multiplayer P1: WASD controls
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            direction.y -= 1.0f;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            direction.y += 1.0f;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            direction.x -= 1.0f;
            m_sprite.setScale(-m_baseScaleX, m_baseScaleY);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            direction.x += 1.0f;
            m_sprite.setScale(m_baseScaleX, m_baseScaleY);
        }
    }
    else
    {
        // Multiplayer P2: Arrow key controls
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            direction.y -= 1.0f;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            direction.y += 1.0f;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            direction.x -= 1.0f;
            m_sprite.setScale(-m_baseScaleX, m_baseScaleY);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            direction.x += 1.0f;
            m_sprite.setScale(m_baseScaleX, m_baseScaleY);
        }
    }

    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if(length > 0.0f)
    {
        direction.x /= length;
        direction.y /= length;
        
        m_position += direction * BASE_MOVE_SPEED * m_moveSpeedMultiplier * g_PlayerSpeedMultiplier * dt;
        m_animator.Update(dt);
        m_currentDirection = direction;
        m_isMoving = true;
    }
    else
    {
        m_animator.Reset();
        m_isMoving = false;
        // Retain last facing horizontal direction (or default to Right if never moved)
        if (m_sprite.getScale().x < 0.0f) {
            m_currentDirection = sf::Vector2f(-1.0f, 0.0f);
        } else {
            m_currentDirection = sf::Vector2f(1.0f, 0.0f);
        }
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

    // Draw persistent weapon visual auras underneath the player
    m_weaponInventory.Draw(target);

    if (m_isMoving)
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

    const float effectiveDamage = std::max(0.0f, amount - m_armor);
    if(effectiveDamage <= 0.0f)
    {
        return;
    }

    m_currentHealth -= effectiveDamage;
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

void Player::GrantInvulnerability(float duration)
{
    if (duration > m_invulnTimer)
    {
        m_invulnTimer = duration;
    }
}

float Player::GetTargetExperience() const
{
    int lvl = m_level;
    if (lvl <= 1) return 5.0f;
    if (lvl < 20) return 5.0f + 10.0f * (lvl - 1);
    if (lvl == 20) return 5.0f + 10.0f * 19 + 600.0f; // 795.0f

    // Level 21..40
    float xp20 = 5.0f + 10.0f * 19 + 600.0f;
    if (lvl < 40) return xp20 + 13.0f * (lvl - 20);
    if (lvl == 40) return (xp20 + 13.0f * 20) + 2400.0f;

    // Level 41+
    float xp40 = (xp20 + 13.0f * 20) + 2400.0f;
    return xp40 + 16.0f * (lvl - 40);
}

float Player::GetExpProgressRatio() const
{
    float target = GetTargetExperience();
    if (target <= 0.0f) return 0.0f;
    float ratio = m_experience / target;
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;
    return ratio;
}

void Player::AddExperience(float amount)
{
    if(amount <= 0.0f)
    {
        return;
    }

    float growthMultiplier = GetGrowthMultiplier();
    if (m_level == 20 || m_level == 40)
    {
        growthMultiplier += 1.0f;
    }

    m_experience += amount * growthMultiplier;

    // Check for level-ups
    float targetExp = GetTargetExperience();
    while (m_experience >= targetExp)
    {
        m_experience -= targetExp;
        m_level++;
        if (m_onLevelUpCallback)
        {
            m_onLevelUpCallback();
        }
        targetExp = GetTargetExperience();
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

int Player::GetPassiveLevel(const std::string& passiveId) const
{
    const auto it = m_passiveLevels.find(passiveId);
    return it == m_passiveLevels.end() ? 0 : it->second;
}

int Player::GetPassiveCount() const
{
    return static_cast<int>(m_passiveLevels.size());
}

bool Player::LevelUpPassive(
    const std::string& passiveId,
    const WeaponProfile& profile,
    const WeaponLevelDelta* nextDelta)
{
    const int currentLevel = GetPassiveLevel(passiveId);
    if(currentLevel == 0)
    {
        ApplyPassiveStats(profile.GetSpecialStats());
    }
    else
    {
        if(!nextDelta)
        {
            return false;
        }
        ApplyPassiveStats(nextDelta->specialStats);
    }

    m_passiveLevels[passiveId] = currentLevel + 1;
    return true;
}

void Player::ApplyPassiveStats(const std::unordered_map<std::string, float>& stats)
{
    auto getStat = [&stats](const char* key) {
        const auto it = stats.find(key);
        return it == stats.end() ? 0.0f : it->second;
    };

    m_mightBuff += getStat("power");
    m_armor += getStat("armor");
    IncreaseMaxHealthPercent(getStat("maxHp"));
    m_recovery += getStat("regen");
    m_cooldownBuff -= getStat("cooldown");
    m_areaBuff += getStat("area");
    m_speedBuff += getStat("speed");
    m_durationBuff += getStat("duration");
    m_amountBuff += static_cast<int>(std::lround(getStat("amount")));
    m_moveSpeedMultiplier += getStat("moveSpeed");
    if(getStat("magnet") != 0.0f)
    {
        AddMagnetBonus(getStat("magnet"));
    }
    m_luckBuff += getStat("luck");
    m_growthBuff += getStat("growth");
    m_greedBuff += getStat("greed");
    m_curseBuff += getStat("curse");
    m_revivals += static_cast<int>(std::lround(getStat("revivals")));
}

void Player::IncreaseMaxHealthPercent(float amount)
{
    if(amount <= 0.0f)
    {
        return;
    }

    const float increase = m_baseMaxHealth * amount;
    m_maxHealth += increase;
    m_currentHealth = std::min(m_maxHealth, m_currentHealth + increase);
}

void Player::OnHpReachedZero()
{
    if(m_revivals > 0)
    {
        --m_revivals;
        m_isDead = false;
        m_currentHealth = m_maxHealth * 0.5f;
        m_invulnTimer = 0.75f;
        return;
    }

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

void Player::ResolveObstacleCollisions(const std::vector<sf::FloatRect>& obstacles)
{
    if (obstacles.empty() || m_isDead)
    {
        return;
    }

    const float radius = m_collisionRadius > 0.0f ? m_collisionRadius : 8.0f;
    // Player feet collision center (m_position is feet)
    const sf::Vector2f feetCenter = m_position - sf::Vector2f(0.0f, radius * 0.5f);

    for (const auto& obs : obstacles)
    {
        // Find closest point on obstacle AABB
        const float closestX = std::clamp(feetCenter.x, obs.left, obs.left + obs.width);
        const float closestY = std::clamp(feetCenter.y, obs.top, obs.top + obs.height);

        const float distX = feetCenter.x - closestX;
        const float distY = feetCenter.y - closestY;
        const float distSq = distX * distX + distY * distY;

        if (distSq < radius * radius)
        {
            if (distSq > 0.0001f)
            {
                const float dist = std::sqrt(distSq);
                const float overlap = radius - dist;
                m_position.x += (distX / dist) * overlap;
                m_position.y += (distY / dist) * overlap;
            }
            else
            {
                // Feet center is completely inside obstacle: push out along nearest boundary
                const float leftDist = std::abs(feetCenter.x - obs.left);
                const float rightDist = std::abs(feetCenter.x - (obs.left + obs.width));
                const float topDist = std::abs(feetCenter.y - obs.top);
                const float bottomDist = std::abs(feetCenter.y - (obs.top + obs.height));

                const float minXDist = std::min(leftDist, rightDist);
                const float minYDist = std::min(topDist, bottomDist);

                if (minXDist < minYDist)
                {
                    m_position.x += (leftDist < rightDist) ? -(leftDist + radius) : (rightDist + radius);
                }
                else
                {
                    m_position.y += (topDist < bottomDist) ? -(topDist + radius) : (bottomDist + radius);
                }
            }
            m_sprite.setPosition(m_position);
        }
    }
}
