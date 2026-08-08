#pragma once

#include "UIElement.h"
#include "../States/Game/TreasurePhases/TreasurePhase2OpeningState.h"
#include <SFML/Graphics.hpp>

class ReelColorTuningUI : public UIElement
{
public:
    ReelColorTuningUI(const sf::Font& font, TreasurePhase2Config& config);
    ~ReelColorTuningUI() override = default;

    void Update(float deltaTime) override;
    void Draw(sf::RenderTarget& target) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void SetPosition(const sf::Vector2f& pos) override;

    bool ContainsPoint(const sf::Vector2f& point) const;

private:
    const sf::Font& m_font;
    TreasurePhase2Config& m_config;

    std::size_t m_selectedSetIndex = 0;  // 0 = 1-Item Set, 1 = 3-Item Set, 2 = 5-Item Set
    std::size_t m_selectedReelIndex = 0; // Reel index within selected set
    int m_activeSliderDrag = -1;         // -1 = none, 0 = R, 1 = G, 2 = B, 3 = A
};
