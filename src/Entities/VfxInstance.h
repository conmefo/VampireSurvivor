#pragma once

#include <SFML/Graphics.hpp>
#include "../Core/Data/HitVfxProfile.h"
#include "../Core/Resources/TextureAtlas.h"
#include "../Core/Animation/Tweener.h"

class VfxInstance
{
private:
    static constexpr float HIT_SPRITE_SIZE_X = 50.0f;
    static constexpr float HIT_SPRITE_SIZE_Y = 100.0f;
    static constexpr float IMPACT_SPRITE_SIZE_X = 72.0f;
    static constexpr float IMPACT_SPRITE_SIZE_Y = 72.0f;
    static constexpr float HIT_SPRITE_OFFSET_X = 7.0f;
    static constexpr float HIT_SPRITE_OFFSET_Y = -15.0f;
    static constexpr float HIT_SPRITE_START_ROTATION = 40.0f;
    static constexpr float HIT_SPRITE_END_ROTATION = 0.0f;
    static constexpr float MIN_SCALE = 0.3f;
    static constexpr float MAX_SCALE = 1.0f;
    static constexpr float LIMIT_SCALE = 10.0f;

    sf::Sprite m_hitSprite;
    sf::Sprite m_impactSprite;
    bool m_hasImpact;
    float m_timer;
    float m_maxTimer;
    Tweener m_scaleTweener;
    const HitVfxProfile* m_profile;

public:
    VfxInstance(const HitVfxProfile& profile, const sf::Vector2f& position, TextureAtlas& atlas);
    ~VfxInstance() = default;

    bool Update(float dt);
    void Draw(sf::RenderTarget& target) const;
};
