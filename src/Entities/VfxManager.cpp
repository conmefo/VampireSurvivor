#include "VfxManager.h"
#include <algorithm>

VfxManager::VfxManager()
    : m_atlas(nullptr)
{
}

void VfxManager::Initialize(TextureAtlas& atlas)
{
    m_atlas = &atlas;
}

void VfxManager::PlayVfx(const HitVfxProfile& profile, const sf::Vector2f& position)
{
    if(m_atlas != nullptr)
    {
        m_activeVfx.push_back(std::make_unique<VfxInstance>(profile, position, *m_atlas));
    }
}

void VfxManager::Update(float dt)
{
    // Utilize the Erase-Remove idiom for O(n) removal without shifting bottlenecks
    m_activeVfx.erase(
        std::remove_if(m_activeVfx.begin(), m_activeVfx.end(),
            [dt](const std::unique_ptr<VfxInstance>& vfx) {
                return !vfx->Update(dt); // Update returns false if timer <= 0
            }),
        m_activeVfx.end()
    );
}

void VfxManager::Draw(sf::RenderTarget& target) const
{
    for(const auto& vfx : m_activeVfx)
    {
        vfx->Draw(target);
    }
}
