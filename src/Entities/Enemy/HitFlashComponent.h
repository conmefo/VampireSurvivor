#pragma once

#include <SFML/Graphics.hpp>
#include "../../Core/Data/HitVfxProfile.h"

class HitFlashComponent
{
private:
    float m_flashTimer;
    bool m_isFlashing;
    bool m_useTintFill;
    sf::Color m_baseColor;
    sf::Color m_flashColor;

public:
    HitFlashComponent();
    ~HitFlashComponent() = default;

    void Init(int baseTintInt);
    void Update(float dt);
    void TriggerFlash(const HitVfxProfile& profile);

    bool IsFlashing() const;
    bool UseTintFill() const;
    sf::Color GetCurrentColor() const;
};
