#include "VfxInstance.h"
#include "../Core/Math/MathUtils.h"
VfxInstance::VfxInstance(const HitVfxProfile& profile, const sf::Vector2f& position, TextureAtlas& atlas)
    : m_profile(&profile)
{
    // The original game was built in Phaser (Javascript), where Tweens use Milliseconds!
    // So a duration of 120 means 120ms (0.12 seconds), and 30 means 30ms (0.03 seconds).
    m_maxTimer = static_cast<float>(profile.GetDuration()) / 1000.0f; 
    m_timer = m_maxTimer;
    m_hasImpact = false;

    // Load Hit Frame
    AssetTextureData hitData = atlas.GetTextureData(profile.GetHitFrameName());
    if(hitData.texture)
    {
        m_hitSprite.setTexture(*hitData.texture);
        m_hitSprite.setTextureRect(hitData.rect);
        
        sf::FloatRect bounds = m_hitSprite.getLocalBounds();
        m_hitSprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    }
    m_hitSprite.setPosition(position.x + HIT_SPRITE_OFFSET_X, position.y + HIT_SPRITE_OFFSET_Y);

    // Load Impact Frame
    std::string impactName = profile.GetImpactFrameName();
    if(!impactName.empty() && impactName != "NoDraw")
    {
        AssetTextureData impactData = atlas.GetTextureData(impactName);
        if(impactData.texture)
        {
            m_hasImpact = true;
            m_impactSprite.setTexture(*impactData.texture);
            m_impactSprite.setTextureRect(impactData.rect);
            
            sf::FloatRect bounds = m_impactSprite.getLocalBounds();
            m_impactSprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
        }
    }
    m_impactSprite.setPosition(position);

    // The targetTint is NOT for the VFX sprite! It's to flash the ENEMY!
    // So we just render the VFX sprite perfectly normally.
    m_hitSprite.setColor(sf::Color(255, 255, 255, 255));
    if(m_hasImpact) m_impactSprite.setColor(sf::Color(255, 255, 255, 255));

    // Calculate independent X/Y scale for Hit Sprite
    sf::FloatRect hitBounds = m_hitSprite.getLocalBounds();
    sf::Vector2f hitScale(HIT_SPRITE_SIZE_X / hitBounds.width, HIT_SPRITE_SIZE_Y / hitBounds.height);
    
    if(hitScale.x <= 0.0f || hitScale.x > LIMIT_SCALE) hitScale.x = MAX_SCALE;
    if(hitScale.y <= 0.0f || hitScale.y > LIMIT_SCALE) hitScale.y = MAX_SCALE;

    // Calculate independent X/Y scale for Impact Sprite
    sf::Vector2f impactScale(MAX_SCALE, MAX_SCALE);
    if(m_hasImpact)
    {
        sf::FloatRect impactBounds = m_impactSprite.getLocalBounds();
        impactScale.x = IMPACT_SPRITE_SIZE_X / impactBounds.width;
        impactScale.y = IMPACT_SPRITE_SIZE_Y / impactBounds.height;
        
        if(impactScale.x <= 0.0f || impactScale.x > LIMIT_SCALE) impactScale.x = MAX_SCALE;
        if(impactScale.y <= 0.0f || impactScale.y > LIMIT_SCALE) impactScale.y = MAX_SCALE;
    }

    // Setup Tweener for scaling. Delay scaling so it only appears near the end of the tint effect!
    m_scaleTweener.SetStartValue(MIN_SCALE);
    m_scaleTweener.SetEndValue(MAX_SCALE);
    m_scaleTweener.SetDuration(m_maxTimer * 0.4f); // Scale over the remaining 40%
    m_scaleTweener.SetEase(MathUtils::EaseType::Cubic, MathUtils::EaseMode::Out);
    
    m_scaleTweener.OnUpdate([this, hitScale, impactScale](float tweenMultiplier)
    {
        m_hitSprite.setScale(hitScale.x * tweenMultiplier, hitScale.y * tweenMultiplier);
        
        float normalizedTween = (tweenMultiplier - MIN_SCALE) / (MAX_SCALE - MIN_SCALE);
        float currentRot = HIT_SPRITE_START_ROTATION + (HIT_SPRITE_END_ROTATION - HIT_SPRITE_START_ROTATION) * normalizedTween;
        m_hitSprite.setRotation(currentRot);
        
        if(m_hasImpact)
        {
            m_impactSprite.setScale(impactScale.x * tweenMultiplier, impactScale.y * tweenMultiplier);
        }
    });
    // Do NOT start tweener yet! It will start when the delay finishes.
}

bool VfxInstance::Update(float dt)
{
    m_timer -= dt;
    if(m_timer <= 0.0f)
        return false;

    float lifeRatio = m_timer / m_maxTimer; // 1.0 (start) down to 0.0 (end)

    // Delay visibility until the last 40% of the effect's lifetime (almost finished tinting)
    if(lifeRatio > 0.4f)
    {
        sf::Color hitColor = m_hitSprite.getColor();
        hitColor.a = 0;
        m_hitSprite.setColor(hitColor);
        if(m_hasImpact)
        {
            sf::Color impactColor = m_impactSprite.getColor();
            impactColor.a = 0;
            m_impactSprite.setColor(impactColor);
        }
        return true;
    }

    // Start tweener if it hasn't started yet
    if(!m_scaleTweener.IsRunning() && m_scaleTweener.GetValue() == 0.0f) 
    {
        m_scaleTweener.Start();
        m_scaleTweener.Update(0.0f); // Initialize starting scale
    }

    // Update the tweener to scale the sprite
    m_scaleTweener.Update(dt);

    // Fade rapidly in the last 20%
    sf::Uint8 alpha = 255;
    if(lifeRatio < 0.2f) {
        alpha = static_cast<sf::Uint8>(255.0f * (lifeRatio / 0.2f)); 
    }
    
    sf::Color hitColor = m_hitSprite.getColor();
    hitColor.a = alpha;
    m_hitSprite.setColor(hitColor);
    
    if(m_hasImpact)
    {
        sf::Color impactColor = m_impactSprite.getColor();
        impactColor.a = alpha;
        m_impactSprite.setColor(impactColor);
    }

    return true;
}

void VfxInstance::Draw(sf::RenderTarget& target) const
{
    if(m_hasImpact) target.draw(m_impactSprite); // Draw shockwave/impact behind
    target.draw(m_hitSprite); // Draw main hit on top
}
