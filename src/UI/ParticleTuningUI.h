#pragma once

#include "Panels/UIPanel.h"
#include "Components/UIScrollbar.h"
#include "../Entities/Particles/ParticleEmitter.h"
#include <vector>
#include <memory>
#include <functional>
#include <SFML/Graphics.hpp>

class ParticleTuningUI : public UIElement
{
public:
    ParticleTuningUI(TextureAtlas& atlas, sf::Font& font, vs::ParticleEmitterConfig& targetConfig);
    ~ParticleTuningUI() override = default;

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
    sf::Font& m_font;
    vs::ParticleEmitterConfig& m_config;
    
    std::unique_ptr<UIPanel> m_background;
    std::vector<SliderEntry> m_sliders;
};
