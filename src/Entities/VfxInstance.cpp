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
    m_hitSprite.setPosition(position);

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

    // We no longer apply targetTint. 
    // The sprites in vfx.png are already colored correctly (e.g., Hit1 is orange, feedback-4 is blue).
    // Applying a Multiply tint in SFML would ruin their white highlights and darken them!
    m_hitSprite.setColor(sf::Color(255, 255, 255, 255));
    if(m_hasImpact) m_impactSprite.setColor(sf::Color(255, 255, 255, 255));

    // Calculate scale to ensure maximum size is exactly ~55x55 pixels
    sf::FloatRect hitBounds = m_hitSprite.getLocalBounds();
    float targetScaleX = 43.0f / hitBounds.width;
    float targetScaleY = 43.0f / hitBounds.height;
    float targetScale = std::min(targetScaleX, targetScaleY);
    if(targetScale <= 0.0f || targetScale > 10.0f) targetScale = 1.0f;

    // Setup Tweener for scaling
    m_scaleTweener.SetStartValue(targetScale * 0.3f); // Start small
    m_scaleTweener.SetEndValue(targetScale);          // End at exactly 55px
    m_scaleTweener.SetDuration(m_maxTimer);
    m_scaleTweener.SetEase(MathUtils::EaseType::Cubic, MathUtils::EaseMode::Out); // Ease-out cubic
    
    m_scaleTweener.OnUpdate([this](float value) {
        m_hitSprite.setScale(value, value);
        if(m_hasImpact) m_impactSprite.setScale(value, value);
    });
    
    m_scaleTweener.Start();
}

bool VfxInstance::Update(float dt)
{
    m_timer -= dt;
    if(m_timer <= 0.0f)
        return false;

    // Update the tweener to scale the sprite
    m_scaleTweener.Update(dt);

    float lifeRatio = m_timer / m_maxTimer; // 1.0 (start) down to 0.0 (end)

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
