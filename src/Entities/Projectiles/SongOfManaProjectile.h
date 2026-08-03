#pragma once

#include "Projectile.h"
#include <SFML/Graphics.hpp>
#include <vector>

struct SongBob
{
    sf::Vector2f position;
    sf::Vector2f velocity;
    float dampening = 1.0f;
    float alpha = 0.95f;
    size_t spriteIndex = 0;
};

// Global Tuning Variables for Song of Mana Visual Parameters
extern float g_SongOfManaDuration;
extern float g_SongOfManaParticleCount;
extern float g_SongOfManaColumnWidth;
extern float g_SongOfManaColumnHeight;
extern float g_SongOfManaSpeedX;
extern float g_SongOfManaSpeedY;
extern float g_SongOfManaDecel;
extern float g_SongOfManaFadeTime;
extern float g_SongOfManaDelay;
extern float g_SongOfManaParticleScale;
extern float g_SongOfManaParticleAlpha;
extern float g_SongOfManaBounceDampening;

class Player;

class SongOfManaProjectile : public Projectile
{
public:
    SongOfManaProjectile(const sf::Texture& texture, const sf::IntRect& textureRect,
                         sf::Vector2f initialPos, float activeDuration, float fadeTime, float power,
                         float areaMultiplier, const std::string& hitVfxName = "Prism");
    ~SongOfManaProjectile() override = default;

    void Update(float dt) override;
    void Draw(sf::RenderTarget& target) const override;
    sf::FloatRect GetGlobalBounds() const override;

    void SetOwnerPlayer(const Player* player);
    void SetHitBoxDelay(float hitBoxDelayMs);
    void SetRepeatInterval(float repeatIntervalMs);
    void SetParticleAtlasFrames(const std::vector<AssetTextureData>& particleFrames);

private:
    void InitBobs();
    void UpdateBlitterBounce(float dt);

    const Player* m_ownerPlayer = nullptr;

    float m_areaMultiplier = 1.0f;
    float m_repeatIntervalMs = 500.0f;
    float m_repeatTimerMs = 0.0f;
    float m_activeTimer = 0.0f;

    std::vector<AssetTextureData> m_particleFrames;

    // Dual particle system (Top & Bottom halves)
    std::vector<SongBob> m_topBobs;
    std::vector<SongBob> m_bottomBobs;

    sf::Sprite m_particleSprite;
    sf::Sprite m_beamSprite;
};
