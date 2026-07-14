#include "RunGoldDisplay.h"

#include "../../Core/Resources/TextureAtlas.h"

#include <algorithm>
#include <cmath>
#include <string>

namespace
{
constexpr float CoinFrameDuration = 0.065f;
constexpr float GoldPulseDuration = 0.38f;
constexpr float CoinScale = 3.0f;
}

RunGoldDisplay::RunGoldDisplay(TextureAtlas& atlas, const sf::Font& font)
    : UIPanel(atlas, "frameB9", 12.0f, 12.0f, 12.0f, 12.0f),
      m_font(font)
{
    SetSize(sf::Vector2f(210.0f, 64.0f));
    SetTextAlignment(TextAlignment::Right);
    SetCornerScale(2.0f);

    const AssetTextureData coinData = atlas.GetTextureData("CoinGold");
    if(coinData.texture)
    {
        m_coinIcon.setTexture(*coinData.texture);
        m_coinIcon.setTextureRect(coinData.rect);
        m_coinIcon.setOrigin(
            static_cast<float>(coinData.rect.width) / 2.0f,
            static_cast<float>(coinData.rect.height) / 2.0f);
        m_coinIcon.setScale(3.0f, 3.0f);
    }

    for(int frameIndex = 1; frameIndex <= 5; ++frameIndex)
    {
        const AssetTextureData frame =
            atlas.GetTextureData("coin-spin-gold_0" + std::to_string(frameIndex));
        if(frame.texture)
        {
            m_coinFrames.push_back(frame);
        }
    }

    SetPosition(sf::Vector2f(1580.0f, 20.0f));
    SetGold(0);
}

void RunGoldDisplay::SetGold(int gold)
{
    gold = std::max(0, gold);
    if(gold == m_gold)
    {
        return;
    }

    const bool increased = m_gold >= 0 && gold > m_gold;
    m_gold = gold;
    SetText(std::to_string(m_gold), m_font, 30);
    if(increased)
    {
        m_pulseRemaining = GoldPulseDuration;
    }
}

void RunGoldDisplay::Update(float dt)
{
    UIPanel::Update(dt);
    dt = std::max(0.0f, dt);

    if(m_pulseRemaining > 0.0f && !m_coinFrames.empty())
    {
        m_pulseRemaining = std::max(0.0f, m_pulseRemaining - dt);
        m_coinFrameTimer += dt;
        while(m_coinFrameTimer >= CoinFrameDuration)
        {
            m_coinFrameTimer -= CoinFrameDuration;
            m_coinFrameIndex = (m_coinFrameIndex + 1) % m_coinFrames.size();
        }

        const AssetTextureData& frame = m_coinFrames[m_coinFrameIndex];
        m_coinIcon.setTexture(*frame.texture, false);
        m_coinIcon.setTextureRect(frame.rect);
        m_coinIcon.setOrigin(
            static_cast<float>(frame.rect.width) * 0.5f,
            static_cast<float>(frame.rect.height) * 0.5f);

        const float progress = 1.0f - m_pulseRemaining / GoldPulseDuration;
        const float pulse = std::sin(progress * 3.14159265f) * 0.34f;
        m_coinIcon.setScale(CoinScale * (1.0f + pulse), CoinScale * (1.0f + pulse));
    }
    else
    {
        m_coinIcon.setScale(CoinScale, CoinScale);
    }
}

void RunGoldDisplay::SetPosition(const sf::Vector2f& position)
{
    UIPanel::SetPosition(position);
    m_coinIcon.setPosition(position.x + 38.0f, position.y + m_size.y / 2.0f);
}

void RunGoldDisplay::Draw(sf::RenderTarget& target)
{
    UIPanel::Draw(target);
    if(m_isActive)
    {
        target.draw(m_coinIcon);
    }
}
