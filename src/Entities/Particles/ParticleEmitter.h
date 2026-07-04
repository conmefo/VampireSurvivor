#pragma once

#include "Particle.h"
#include <vector>
#include <string>

namespace vs
{

struct ParticleEmitterConfig
{
    float startLifetime = 1.0f;
    float startSpeed = 50.0f;
    float emissionRate = 10.0f;
    float minSize = 0.1f;
    float maxSize = 1.0f;
    float endSize = 1.0f; // Only used if you want to shrink/grow over time, otherwise keep it same as size
    bool randomRotation = false;
    
    sf::Color startColor = sf::Color::White;
    sf::Color endColor = sf::Color(255, 255, 255, 0); // Fade to transparent

    float colorR = 100.0f;
    float colorG = 255.0f;
    float colorB = 255.0f;
    float colorA = 255.0f;
    float weaponScaleX = 1.0f;
    float weaponScaleY = 1.0f;
    
    sf::Vector2f emissionDirection = sf::Vector2f(0.0f, 0.0f);
    float emitterOffset = 0.0f;
    
    // Trail specific tuning (used by TrailRenderer)
    float trailWidth = 15.0f;
    float trailFadeStart = 0.5f;
    float trailLength = 0.8f;
    
    float shapeRadius = 0.0f;
    float shapeAngle = 360.0f; // Spread angle in degrees
    
    bool looping = true;
    float duration = 1.0f; // Total duration of the emitter if not looping
    
    std::string textureId = ""; // e.g. "HitStarWhite1"

    int burstCount = 0;
    float gravityModifier = 0.0f;
    float damping = 0.0f;
    sf::BlendMode blendMode = sf::BlendAlpha;
    
    bool overrideColor = false;
};

class ParticleEmitter
{
public:
    ParticleEmitter(const ParticleEmitterConfig& config, const sf::Vector2f& position, const sf::IntRect& textureRect);

    void Update(float dt);
    
    bool IsFinished() const;
    void SetPosition(const sf::Vector2f& pos) 
    { 
        if (m_isFirstPositionUpdate) {
            m_previousPosition = pos;
            m_position = pos;
            m_isFirstPositionUpdate = false;
        } else {
            m_previousPosition = m_position;
            m_position = pos;
        }
    }
    
    const std::vector<Particle>& GetParticles() const { return m_particles; }

    void Stop() 
    {
        m_config.looping = false;
        m_elapsedTime = m_config.duration; // Force emission to stop immediately
    }

    // Dynamic config tuning support
    ParticleEmitterConfig& GetConfig() { return m_config; }

private:
    void EmitParticle(const sf::Vector2f& spawnPos);

    ParticleEmitterConfig m_config;
    sf::Vector2f m_position;
    sf::Vector2f m_previousPosition;
    bool m_isFirstPositionUpdate = true;
    bool m_hasBursted = false;
    sf::IntRect m_textureRect;
    
    std::vector<Particle> m_particles;
    
    float m_elapsedTime = 0.0f;
    float m_emissionTimer = 0.0f;
};

} // namespace vs
