#pragma once

#include "../Panels/UIPanel.h"
#include "../../Core/Resources/TextureAtlas.h"

#include <SFML/Graphics/Sprite.hpp>
#include <vector>

class RunGoldDisplay : public UIPanel
{
public:
    RunGoldDisplay(TextureAtlas& atlas, const sf::Font& font);

    void SetGold(int gold);
    void SetPosition(const sf::Vector2f& position) override;
    void Update(float dt) override;
    void Draw(sf::RenderTarget& target) override;

private:
    sf::Sprite m_coinIcon;
    AssetTextureData m_idleCoinFrame;
    std::vector<AssetTextureData> m_coinFrames;
    const sf::Font& m_font;
    int m_gold = -1;
    std::size_t m_coinFrameIndex = 0;
    float m_coinFrameTimer = 0.0f;
    float m_pulseRemaining = 0.0f;
};
