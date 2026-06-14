#pragma once

#include <SFML/Graphics.hpp>
#include "../Core/Data/HitVfxProfile.h"
#include "../Core/Resources/TextureAtlas.h"
#include "../Core/Animation/Tweener.h"

class VfxInstance
{
private:
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
