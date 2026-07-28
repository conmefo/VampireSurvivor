#include "../Player.h"
#include "GarlicWeapon.h"
#include "../Projectiles/GarlicProjectile.h"
#include <cmath>

extern float g_PlayerSpeedMultiplier;

GarlicWeapon::GarlicWeapon(const WeaponProfile& profile)
    : Weapon(profile)
{
}

void GarlicWeapon::Update(float dt, ProjectileManager& projManager, TextureAtlas& atlas, Player& player, EnemyPool& enemyPool)
{
    // 1. Initialize persistent visual sprite
    if (!m_visualInitialized)
    {
        AssetTextureData data = atlas.GetTextureData("Rings3");
        if (data.texture)
        {
            m_visualSprite.setTexture(*data.texture);
            m_visualSprite.setTextureRect(data.rect);
            m_visualSprite.setOrigin(data.rect.width / 2.0f, data.rect.height / 2.0f);
            m_visualInitialized = true;
        }
    }

    // 2. Accumulate ticks (Garlic ticks faster when player is actively walking/moving)
    float baseTimeMs = dt * 1000.f;
    float movementTimeMs = 0.0f;

    // Retain facing logic when stopped but check active motion using length of movement keys
    sf::Vector2f rawMovementDir(0.f, 0.f);
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) rawMovementDir.y -= 1.0f;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) rawMovementDir.y += 1.0f;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) rawMovementDir.x -= 1.0f;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) rawMovementDir.x += 1.0f;

    float isMoving = (rawMovementDir.x != 0.0f || rawMovementDir.y != 0.0f);
    if (isMoving)
    {
        // Speed scaling (base speed 150 * dt * multiplier)
        float walkSpeed = 150.f * g_PlayerSpeedMultiplier;
        float distWalked = walkSpeed * dt;
        // The original game walks time accumulation faster based on distance walked:
        // walk_distance * 100.0f * (dt_ms / 166.666f)
        movementTimeMs = distWalked * 100.f * (baseTimeMs / 166.6667f);
    }

    m_accumulatorMs += baseTimeMs + movementTimeMs;

    // 3. Trigger pulse hit tick when interval is reached
    float intervalMs = static_cast<float>(m_profile.GetInterval());
    if (intervalMs <= 0.0f) intervalMs = 1000.0f;

    if (m_accumulatorMs >= intervalMs)
    {
        m_accumulatorMs -= intervalMs;
        
        // Spawn/Fire circular pulse trigger logic
        FireOne(projManager, atlas, player, player.GetPosition(), 0);
    }

    // 4. Update visual animations (endless rotation + alpha transparency pulsing)
    if (m_visualInitialized)
    {
        // Slow rotation (30 degrees per second)
        m_rotationAngle += 30.0f * dt;
        if (m_rotationAngle >= 360.0f) m_rotationAngle -= 360.0f;
        m_visualSprite.setRotation(m_rotationAngle);

        // Continuous alpha transparency pulsing (Sine wave between 0.1f and 0.3f opacity)
        m_alphaTimer += dt * 1.5708f; // Complete pulse cycle every 4 seconds (pi / 2 rad per second)
        float pulseFactor = 0.2f + 0.1f * std::sin(m_alphaTimer);
        sf::Uint8 alpha = static_cast<sf::Uint8>(std::max(0.0f, std::min(1.0f, pulseFactor)) * 255.f);
        m_visualSprite.setColor(sf::Color(255, 255, 255, alpha));

        // Lock visual sprite position to player's middle bottom position (feet center)
        m_visualSprite.setPosition(player.GetBottomPosition());

        // Update area scale multiplier (multiplied by 2.0f matching the original game's 0x40000000 float scale parameter)
        float areaMultiplier = 2.0f * m_profile.GetArea();
        m_visualSprite.setScale(areaMultiplier, areaMultiplier);
    }

    // Keep temporary compiler warning flags clean
    (void)enemyPool;
}

void GarlicWeapon::Draw(sf::RenderTarget& target) const
{
    if (m_visualInitialized)
    {
        target.draw(m_visualSprite);
    }
}

void GarlicWeapon::FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex)
{
    AssetTextureData data = atlas.GetTextureData("Rings3");
    if (!data.texture) return;

    // Apply the 2.0f base area scale parameter matching original game
    float areaMultiplier = 2.0f * m_profile.GetArea();
    
    // Garlic pulse lasts for a transient tick time (e.g. 0.15s duration)
    float duration = 0.15f; 
    float power = m_profile.GetPower();

    // Create Garlic projectile centered on player's feet
    auto proj = std::make_unique<GarlicProjectile>(*data.texture, data.rect, player.GetBottomPosition(), duration, power, areaMultiplier, m_profile.GetHitVFX(), m_profile.GetPenetrating());
    
    // Connect tracking so the projectile moves locked with the player's feet
    proj->SetCenterPosition(player.GetBottomPosition());

    projManager.AddProjectile(std::move(proj));

    (void)targetPosition;
    (void)projectileIndex;
}
