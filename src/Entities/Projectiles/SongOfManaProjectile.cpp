#include "SongOfManaProjectile.h"
#include "../Player.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <iostream>

// Global Tuning Variable Definitions for Song of Mana Visual Parameters
float g_SongOfManaDuration = 0.5f;
float g_SongOfManaParticleCount = 120.0f;
float g_SongOfManaColumnWidth = 16.0f;
float g_SongOfManaColumnHeight = 490.9f; // Visible screen view height (1080.0f / 2.2f WorldZoom)
float g_SongOfManaSpeedX = 985.0f;
float g_SongOfManaSpeedY = 370.0f;
float g_SongOfManaDecel = 0.5f;
float g_SongOfManaFadeTime = 1.2f;
float g_SongOfManaDelay = 1.5f; // 2000ms interval - 500ms duration = 1500ms delay
float g_SongOfManaParticleScale = 0.50f;
float g_SongOfManaParticleAlpha = 0.90f;
float g_SongOfManaBounceDampening = 0.85f;

SongOfManaProjectile::SongOfManaProjectile(const sf::Texture& texture, const sf::IntRect& textureRect,
                                           sf::Vector2f initialPos, float activeDuration, float fadeTime, float power,
                                           float areaMultiplier, const std::string& hitVfxName)
    : Projectile(texture, textureRect, initialPos, sf::Vector2f(0.0f, 0.0f), activeDuration + fadeTime, power, areaMultiplier, hitVfxName)
    , m_areaMultiplier(areaMultiplier)
    , m_activeTimer(std::max(0.1f, activeDuration))
{
    m_particleSprite.setTexture(texture);
    // Uses 2Spell particle frames from items_atlas
    m_particleSprite.setTextureRect(textureRect);
    m_particleSprite.setOrigin(textureRect.width * 0.5f, textureRect.height * 0.5f);

    m_beamSprite = m_sprite;

    InitBobs();
}

void SongOfManaProjectile::SetOwnerPlayer(const Player* player)
{
    m_ownerPlayer = player;
}

void SongOfManaProjectile::SetHitBoxDelay(float hitBoxDelayMs)
{
    (void)hitBoxDelayMs;
}

void SongOfManaProjectile::SetRepeatInterval(float repeatIntervalMs)
{
    m_repeatIntervalMs = repeatIntervalMs;
}

void SongOfManaProjectile::SetParticleAtlasFrames(const std::vector<AssetTextureData>& particleFrames)
{
    m_particleFrames = particleFrames;
}

void SongOfManaProjectile::InitBobs()
{
    int totalParticles = static_cast<int>(g_SongOfManaParticleCount * m_areaMultiplier);
    int bobsPerHalf = static_cast<int>(totalParticles * 0.5f);

    m_topBobs.resize(bobsPerHalf);
    m_bottomBobs.resize(bobsPerHalf);

    auto getRandomFloat = [](float minVal, float maxVal) {
        float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        return minVal + r * (maxVal - minVal);
    };

    const float PI = 3.1415926535f;
    float effectiveSpeedX = g_SongOfManaSpeedX * (g_SongOfManaColumnWidth / 80.0f);

    for (int i = 0; i < bobsPerHalf; ++i)
    {
        float speedScalar = 0.35f + 0.65f * (static_cast<float>(i % 5) / 4.0f);

        // Top half: random 3D latitude angle theta [-PI/2, 0] and longitude angle phi [0, 2*PI]
        float thetaTop = getRandomFloat(-PI * 0.5f, 0.0f);
        float phiTop = getRandomFloat(0.0f, 2.0f * PI);
        m_topBobs[i].position = sf::Vector2f(0.0f, 0.0f);
        m_topBobs[i].velocity = sf::Vector2f(
            speedScalar * effectiveSpeedX * std::cos(thetaTop) * std::cos(phiTop),
            speedScalar * g_SongOfManaSpeedY * std::sin(thetaTop)
        );
        m_topBobs[i].dampening = g_SongOfManaBounceDampening;
        m_topBobs[i].alpha = g_SongOfManaParticleAlpha;
        m_topBobs[i].spriteIndex = std::rand() % 5;

        // Bottom half: random 3D latitude angle theta [0, PI/2] and longitude angle phi [0, 2*PI]
        float thetaBottom = getRandomFloat(0.0f, PI * 0.5f);
        float phiBottom = getRandomFloat(0.0f, 2.0f * PI);
        m_bottomBobs[i].position = sf::Vector2f(0.0f, 0.0f);
        m_bottomBobs[i].velocity = sf::Vector2f(
            speedScalar * effectiveSpeedX * std::cos(thetaBottom) * std::cos(phiBottom),
            speedScalar * g_SongOfManaSpeedY * std::sin(thetaBottom)
        );
        m_bottomBobs[i].dampening = g_SongOfManaBounceDampening;
        m_bottomBobs[i].alpha = g_SongOfManaParticleAlpha;
        m_bottomBobs[i].spriteIndex = std::rand() % 5;
    }
}

void SongOfManaProjectile::Update(float dt)
{
    Projectile::Update(dt);

    if (m_activeTimer > 0.0f)
    {
        m_activeTimer -= dt;

        // Tick repeat interval to re-damage enemies during active phase
        m_repeatTimerMs += dt * 1000.0f;
        if (m_repeatTimerMs >= m_repeatIntervalMs)
        {
            m_repeatTimerMs -= m_repeatIntervalMs;
            m_hitEnemies.clear();
        }
    }

    // Dynamic horizontal & vertical anchoring to player
    if (m_ownerPlayer)
    {
        sf::Vector2f playerPos = m_ownerPlayer->GetPosition();
        m_sprite.setPosition(playerPos);
    }

    UpdateBlitterBounce(dt);
}

void SongOfManaProjectile::UpdateBlitterBounce(float dt)
{
    float halfWidth = m_areaMultiplier * g_SongOfManaColumnWidth;
    float halfHeight = g_SongOfManaColumnHeight * 0.5f;

    float xMin = -halfWidth;
    float xMax = halfWidth;
    float yMin = -halfHeight;
    float yMax = halfHeight;

    bool isFadingOut = (m_activeTimer <= 0.0f);
    float fadeRate = (g_SongOfManaFadeTime > 0.0f) ? (g_SongOfManaParticleAlpha / g_SongOfManaFadeTime) : 4.0f;
    float decelFactor = std::max(0.0f, 1.0f - g_SongOfManaDecel * dt);

    auto updateParticle = [&](SongBob& bob) {
        bob.position += bob.velocity * dt;

        if (isFadingOut)
        {
            // Left & right boundaries disappear, decelerate and fade out
            bob.velocity *= decelFactor;
            bob.alpha = std::max(0.0f, bob.alpha - fadeRate * dt);

            // Upper & lower edges remain active: bounce particles vertically
            if (bob.position.y > yMax) {
                bob.position.y = yMax;
                bob.velocity.y = -bob.velocity.y;
            } else if (bob.position.y < yMin) {
                bob.position.y = yMin;
                bob.velocity.y = -bob.velocity.y;
            }
        }
        else
        {
            // Specular light reflection bouncing off column edges (with dampening loss)
            if (bob.position.x > xMax) {
                bob.position.x = xMax;
                bob.velocity.x = -bob.dampening * bob.velocity.x;
            } else if (bob.position.x < xMin) {
                bob.position.x = xMin;
                bob.velocity.x = -bob.dampening * bob.velocity.x;
            }

            if (bob.position.y > yMax) {
                bob.position.y = yMax;
                bob.velocity.y = -bob.dampening * bob.velocity.y;
            } else if (bob.position.y < yMin) {
                bob.position.y = yMin;
                bob.velocity.y = -bob.dampening * bob.velocity.y;
            }
        }
    };

    for (auto& bob : m_topBobs)
    {
        updateParticle(bob);
    }

    for (auto& bob : m_bottomBobs)
    {
        updateParticle(bob);
    }
}

void SongOfManaProjectile::Draw(sf::RenderTarget& target) const
{
    sf::Vector2f basePos = GetPosition();
    sf::RectangleShape particleDot(sf::Vector2f(3.5f, 3.5f));
    particleDot.setOrigin(1.75f, 1.75f);

    // Helper lambda to draw a particle bob
    auto drawBob = [&](const SongBob& bob, sf::Color fallbackColor) {
        if (!m_particleFrames.empty())
        {
            size_t idx = bob.spriteIndex % m_particleFrames.size();
            const auto& frameData = m_particleFrames[idx];
            if (frameData.texture)
            {
                sf::Sprite spr(*frameData.texture, frameData.rect);
                spr.setOrigin(frameData.rect.width * 0.5f, frameData.rect.height * 0.5f);
                spr.setPosition(basePos + bob.position);
                spr.setScale(g_SongOfManaParticleScale, g_SongOfManaParticleScale);
                spr.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(bob.alpha * 255)));
                target.draw(spr);
                return;
            }
        }
        particleDot.setPosition(basePos + bob.position);
        particleDot.setFillColor(fallbackColor);
        target.draw(particleDot);
    };

    // Bottom half (Z Depth behind player)
    for (const auto& bob : m_bottomBobs)
    {
        drawBob(bob, sf::Color(200, 240, 255, static_cast<sf::Uint8>(bob.alpha * 255)));
    }

    // Top half (Z Depth in front of player)
    for (const auto& bob : m_topBobs)
    {
        drawBob(bob, sf::Color(220, 245, 255, static_cast<sf::Uint8>(bob.alpha * 255)));
    }
}

sf::FloatRect SongOfManaProjectile::GetGlobalBounds() const
{
    if (m_activeTimer <= 0.0f)
    {
        return sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f);
    }

    float halfWidth = m_areaMultiplier * g_SongOfManaColumnWidth;
    float halfHeight = g_SongOfManaColumnHeight * 0.5f;
    sf::Vector2f pos = GetPosition();
    return sf::FloatRect(pos.x - halfWidth, pos.y - halfHeight, halfWidth * 2.0f, halfHeight * 2.0f);
}
