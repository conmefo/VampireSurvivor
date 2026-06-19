#include "HitFlashComponent.h"

namespace
{
    sf::Color IntToColor(int colorInt)
    {
        sf::Uint8 r = (colorInt >> 16) & 0xFF;
        sf::Uint8 g = (colorInt >> 8) & 0xFF;
        sf::Uint8 b = colorInt & 0xFF;
        return sf::Color(r, g, b, 255);
    }
}

HitFlashComponent::HitFlashComponent()
    : m_flashTimer(0.0f),
      m_isFlashing(false),
      m_useTintFill(false),
      m_baseColor(sf::Color::White),
      m_flashColor(sf::Color::White)
{
}

void HitFlashComponent::Init(int baseTintInt)
{
    m_baseColor = IntToColor(baseTintInt);
    m_isFlashing = false;
    m_useTintFill = false;
    m_flashTimer = 0.0f;
}

void HitFlashComponent::Update(float dt)
{
    if(!m_isFlashing)
    {
        return;
    }

    m_flashTimer -= dt;
    if(m_flashTimer <= 0.0f)
    {
        m_flashTimer = 0.0f;
        m_isFlashing = false;
        m_useTintFill = false;
    }
}

void HitFlashComponent::TriggerFlash(const HitVfxProfile& profile)
{
    m_flashColor = IntToColor(profile.GetTargetTint());
    m_useTintFill = profile.IsTintFill();
    m_isFlashing = true;

    // Convert ms to seconds
    m_flashTimer = static_cast<float>(profile.GetDuration()) / 1000.0f;
    if(m_flashTimer <= 0.0f)
    {
        m_flashTimer = 0.1f;
    }
}

bool HitFlashComponent::IsFlashing() const
{
    return m_isFlashing;
}

bool HitFlashComponent::UseTintFill() const
{
    return m_isFlashing && m_useTintFill;
}

sf::Color HitFlashComponent::GetCurrentColor() const
{
    if(!m_isFlashing)
    {
        return m_baseColor;
    }

    if(m_useTintFill)
    {
        return m_flashColor;
    }

    sf::Uint8 r = static_cast<sf::Uint8>((m_baseColor.r * m_flashColor.r) / 255);
    sf::Uint8 g = static_cast<sf::Uint8>((m_baseColor.g * m_flashColor.g) / 255);
    sf::Uint8 b = static_cast<sf::Uint8>((m_baseColor.b * m_flashColor.b) / 255);
    return sf::Color(r, g, b, m_baseColor.a);
}
