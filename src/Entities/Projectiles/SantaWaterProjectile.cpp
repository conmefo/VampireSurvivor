#include "SantaWaterProjectile.h"
#include "SantaWaterZone.h"
#include <cmath>
#include <algorithm>

float g_SantaWaterDropHeight = 250.0f;
float g_SantaWaterFlightTime = 0.75f;
float g_SantaWaterSpinSpeed = -360.0f;
float g_SantaWaterBottleScale = 1.0f;
float g_SantaWaterXSpread = 50.0f;
float g_SantaWaterYSpread = 0.0f;

SantaWaterProjectile::SantaWaterProjectile(ProjectileManager* projManager, TextureAtlas* atlas, const sf::Texture& texture, const sf::IntRect& textureRect, sf::Vector2f playerPosition, sf::Vector2f targetPosition, float duration, float power, float areaMultiplier, float hitBoxDelay, const std::string& hitVfxName)
    : Projectile(texture, textureRect, playerPosition, sf::Vector2f(0.0f, 0.0f), 10.0f, power, areaMultiplier, hitVfxName, -1)
    , m_projManager(projManager)
    , m_atlas(atlas)
    , m_targetPosition(targetPosition)
    , m_flightTimer(0.0f)
    , m_zoneDuration(duration)
    , m_areaMultiplier(areaMultiplier)
    , m_hitBoxDelay(hitBoxDelay)
    , m_hasLanded(false)
{
    // Fixed Y coordinate at upper screen edge, randomized horizontal X coordinate in closed range around player X
    float randomXRatio = ((static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f);

    float startX = playerPosition.x + randomXRatio * g_SantaWaterXSpread;
    float startY = playerPosition.y - g_SantaWaterDropHeight;

    m_startPosition = sf::Vector2f(startX, startY);
    m_sprite.setPosition(m_startPosition);
    m_sprite.setScale(g_SantaWaterBottleScale, g_SantaWaterBottleScale);
}

void SantaWaterProjectile::Update(float dt)
{
    if(m_hasLanded)
    {
        return;
    }

    m_sprite.setScale(g_SantaWaterBottleScale, g_SantaWaterBottleScale);

    m_flightTimer += dt;

    float flightTime = std::max(0.05f, g_SantaWaterFlightTime);
    float t = std::min(1.0f, m_flightTimer / flightTime);

    // Pure straight line trajectory (no parabolic arc)
    sf::Vector2f currentPos = m_startPosition + (m_targetPosition - m_startPosition) * t;
    m_sprite.setPosition(currentPos);

    float currentRotation = g_SantaWaterSpinSpeed * (m_flightTimer / flightTime);
    m_sprite.setRotation(currentRotation);

    if(t >= 1.0f && !m_hasLanded)
    {
        m_hasLanded = true;

        if(m_projManager && m_atlas)
        {
            AssetTextureData data = m_atlas->GetTextureData("HolyWater");
            if(!data.texture || data.rect.width == 0 || data.rect.height == 0)
            {
                data = m_atlas->GetTextureData("ProjectileBottle");
            }

            if(data.texture)
            {
                auto groundZone = std::make_unique<SantaWaterZone>(
                    m_atlas,
                    *data.texture,
                    data.rect,
                    m_targetPosition,
                    m_zoneDuration,
                    m_power,
                    m_areaMultiplier,
                    m_hitBoxDelay,
                    m_hitVfxName
                );

                m_projManager->AddProjectile(std::move(groundZone));
            }

            // Spawn HitSmokeE / HitSmokeE2 break visual pop effect
            // Spawn HitCloud1 and HitCloud2 break visual pop effects synchronized together
            AssetTextureData cloud1Data = m_atlas->GetTextureData("HitCloud1");
            AssetTextureData cloud2Data = m_atlas->GetTextureData("HitCloud2");

            float targetScale = 0.75f * m_areaMultiplier;

            if(cloud1Data.texture && cloud1Data.rect.width > 0)
            {
                auto cloud1Effect = std::make_unique<Projectile>(
                    *cloud1Data.texture,
                    cloud1Data.rect,
                    m_targetPosition,
                    sf::Vector2f(0.0f, 0.0f),
                    0.12f,
                    0.0f,
                    m_areaMultiplier,
                    ""
                );
                cloud1Effect->SetColor(sf::Color(180, 210, 255, 140));
                cloud1Effect->SetScaleTween(sf::Vector2f(targetScale, targetScale), 0.08f);
                m_projManager->AddProjectile(std::move(cloud1Effect));
            }

            if(cloud2Data.texture && cloud2Data.rect.width > 0)
            {
                auto cloud2Effect = std::make_unique<Projectile>(
                    *cloud2Data.texture,
                    cloud2Data.rect,
                    m_targetPosition,
                    sf::Vector2f(0.0f, 0.0f),
                    0.12f,
                    0.0f,
                    m_areaMultiplier,
                    ""
                );
                cloud2Effect->SetColor(sf::Color(180, 210, 255, 140));
                cloud2Effect->SetScaleTween(sf::Vector2f(targetScale, targetScale), 0.08f);
                m_projManager->AddProjectile(std::move(cloud2Effect));
            }
        }

        m_duration = 0.0f; // Expire airborne bottle projectile
    }
}
