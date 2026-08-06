#pragma once

#include "Panels/UIPanel.h"
#include "Components/UIScrollbar.h"
#include <vector>
#include <memory>
#include <string>
#include <SFML/Graphics.hpp>

class ExperienceGemManager;

class GemTuningUI : public UIElement
{
public:
    GemTuningUI(TextureAtlas& atlas, const sf::Font& font, ExperienceGemManager& gemManager);
    ~GemTuningUI() override = default;

    void Update(float deltaTime) override;
    void Draw(sf::RenderTarget& target) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void SetPosition(const sf::Vector2f& pos) override;

private:
    struct SliderEntry
    {
        std::unique_ptr<UIScrollbar> scrollbar;
        sf::Text label;
        sf::Text valueText;
        float minVal;
        float maxVal;
        float* targetValue;
    };

    void AddSlider(const std::string& name, float min, float max, float* targetPtr);

    TextureAtlas& m_atlas;
    const sf::Font& m_font;
    ExperienceGemManager& m_gemManager;

    float m_outwardSpeed;
    float m_inwardAccel;
    float m_maxSpeed;
    float m_magnetRadiusMult;

    std::unique_ptr<UIPanel> m_background;
    std::vector<SliderEntry> m_sliders;
};
