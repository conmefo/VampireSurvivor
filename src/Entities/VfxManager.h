#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include "VfxInstance.h"
#include "../Core/Resources/TextureAtlas.h"

class VfxManager
{
private:
    std::vector<VfxInstance> m_activeVfx;
    TextureAtlas* m_atlas;

public:
    VfxManager();
    ~VfxManager() = default;

    void Initialize(TextureAtlas& atlas);
    void PlayVfx(const HitVfxProfile& profile, const sf::Vector2f& position);
    
    void Update(float dt);
    void Draw(sf::RenderTarget& target) const;
};
