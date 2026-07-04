#include "ParticleTuningUI.h"
#include <iomanip>
#include <sstream>

ParticleTuningUI::ParticleTuningUI(TextureAtlas& atlas, sf::Font& font, vs::ParticleEmitterConfig& targetConfig)
    : m_atlas(atlas), m_font(font), m_config(targetConfig)
{
    m_background = std::make_unique<UIPanel>(atlas, "frame1_c2", 10.0f, 10.0f, 10.0f, 10.0f);
    m_background->SetColor(sf::Color(0, 0, 0, 180));
    m_background->SetSize(sf::Vector2f(1100.0f, 500.0f));

    // Add sliders mapped to the config
    AddSlider("Speed", 0.0f, 300.0f, &m_config.startSpeed);
    AddSlider("Lifetime", 0.1f, 5.0f, &m_config.startLifetime);
    AddSlider("Emission Rate", 0.0f, 500.0f, &m_config.emissionRate);
    AddSlider("Min Size", 0.01f, 5.0f, &m_config.minSize);
    AddSlider("Max Size", 0.01f, 5.0f, &m_config.maxSize);
    AddSlider("End Size", 0.0f, 5.0f, &m_config.endSize);
    AddSlider("Shape Radius", 0.0f, 100.0f, &m_config.shapeRadius);
    AddSlider("Spread Angle", 0.0f, 360.0f, &m_config.shapeAngle);
    AddSlider("Gravity", -10.0f, 10.0f, &m_config.gravityModifier);
    AddSlider("Damping", 0.0f, 10.0f, &m_config.damping);
    AddSlider("Color R", 0.0f, 255.0f, &m_config.colorR);
    AddSlider("Color G", 0.0f, 255.0f, &m_config.colorG);
    AddSlider("Color B", 0.0f, 255.0f, &m_config.colorB);
    AddSlider("Opacity", 0.0f, 255.0f, &m_config.colorA);
    AddSlider("Wpn Scale X", 0.1f, 5.0f, &m_config.weaponScaleX);
    AddSlider("Wpn Scale Y", 0.1f, 5.0f, &m_config.weaponScaleY);
    AddSlider("Offset", -200.0f, 200.0f, &m_config.emitterOffset);
    AddSlider("Trail Width", 1.0f, 50.0f, &m_config.trailWidth);
    AddSlider("Trail Fade Ratio", 0.0f, 1.0f, &m_config.trailFadeStart);
    AddSlider("Trail Length", 0.1f, 5.0f, &m_config.trailLength);
}

void ParticleTuningUI::AddSlider(const std::string& name, float min, float max, float* targetPtr)
{
    SliderEntry entry;
    entry.scrollbar = std::make_unique<UIScrollbar>(m_atlas);
    entry.scrollbar->UpdateLayout(20.0f, 200.0f); // Make it a horizontal-ish setup or use vertical since it's a scrollbar
    // Actually UIScrollbar is vertical. We will place them side by side.
    
    entry.label.setFont(m_font);
    entry.label.setCharacterSize(14);
    entry.label.setString(name);
    entry.label.setFillColor(sf::Color::White);
    
    entry.valueText.setFont(m_font);
    entry.valueText.setCharacterSize(14);
    entry.valueText.setFillColor(sf::Color::Yellow);

    entry.minVal = min;
    entry.maxVal = max;
    entry.targetValue = targetPtr;
    
    // Set initial ratio
    float ratio = (*targetPtr - min) / (max - min);
    ratio = std::max(0.0f, std::min(1.0f, ratio));
    entry.scrollbar->SetScrollRatio(ratio);

    m_sliders.push_back(std::move(entry));
}

void ParticleTuningUI::SetPosition(const sf::Vector2f& pos)
{
    UIElement::SetPosition(pos);
    m_background->SetPosition(pos);
    
    float startX = pos.x + 30.0f;
    float startY = pos.y + 40.0f;
    
    for (size_t i = 0; i < m_sliders.size(); ++i)
    {
        float x = startX + i * 55.0f; // Increased horizontal spacing
        m_sliders[i].label.setPosition(x, startY - 30.0f);
        // rotate text
        m_sliders[i].label.setRotation(90.0f);
        
        m_sliders[i].valueText.setPosition(x + 15.0f, startY + 220.0f);
        
        m_sliders[i].scrollbar->SetPosition(sf::Vector2f(x, startY));
        m_sliders[i].scrollbar->UpdateLayout(200.0f, 400.0f);
    }
}

void ParticleTuningUI::Update(float deltaTime)
{
    for (auto& slider : m_sliders)
    {
        slider.scrollbar->Update(deltaTime);
        
        // Update target value based on scrollbar ratio
        float ratio = slider.scrollbar->GetScrollRatio();
        
        // Since scrollbar top is usually 0 and bottom is 1, let's invert it so bottom is 0 and top is 1
        ratio = 1.0f - ratio;
        
        *slider.targetValue = slider.minVal + ratio * (slider.maxVal - slider.minVal);
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << *slider.targetValue;
        slider.valueText.setString(ss.str());
    }
}

void ParticleTuningUI::Draw(sf::RenderTarget& target)
{
    m_background->Draw(target);
    for (auto& slider : m_sliders)
    {
        target.draw(slider.label);
        slider.scrollbar->Draw(target);
        target.draw(slider.valueText);
    }
}

void ParticleTuningUI::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    for (auto& slider : m_sliders)
    {
        slider.scrollbar->HandleEvent(event, window);
    }
}
