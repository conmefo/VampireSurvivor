#include "RunGoldDisplay.h"

#include "../../Core/Resources/TextureAtlas.h"

#include <algorithm>
#include <string>

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

    m_gold = gold;
    SetText(std::to_string(m_gold), m_font, 30);
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
