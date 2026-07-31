#include "ParticleTuningUI.h"
#include <iomanip>
#include <sstream>

extern float g_AxeInitialSpeed;
extern float g_AxeGravity;
extern float g_AxeAmountOverride;
extern float g_AxeShadowDelay;
extern float g_AxeScale;
extern float g_PlayerSpeedMultiplier;
extern float g_MagicWandSpeed;
extern float g_MagicWandScale;
extern float g_FireWandSpeed;
extern float g_FireWandScale;

ParticleTuningUI::ParticleTuningUI(TextureAtlas& atlas, sf::Font& font, vs::ParticleEmitterConfig& targetConfig)
    : m_atlas(atlas), m_font(font), m_config(targetConfig)
{
    m_background = std::make_unique<UIPanel>(atlas, "frame1_c2", 10.0f, 10.0f, 10.0f, 10.0f);
    // Tuning sliders unattached
}

void ParticleTuningUI::AddSlider(const std::string& name, float min, float max, float* targetPtr)
{
    SliderEntry entry;
    entry.scrollbar = std::make_unique<UIScrollbar>(m_atlas);
    entry.scrollbar->UpdateLayout(200.0f, 400.0f);
    
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
    
    // Set initial ratio matching inverted UI scale (top = 1.0, bottom = 0.0)
    float val = *targetPtr;
    float ratio = (val - min) / (max - min);
    ratio = std::max(0.0f, std::min(1.0f, ratio));
    entry.scrollbar->SetScrollRatio(1.0f - ratio);

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
        
        // Ensure scrollbar ratio stays aligned to initial target value on layout
        float val = *m_sliders[i].targetValue;
        float ratio = (val - m_sliders[i].minVal) / (m_sliders[i].maxVal - m_sliders[i].minVal);
        ratio = std::max(0.0f, std::min(1.0f, ratio));
        m_sliders[i].scrollbar->SetScrollRatio(1.0f - ratio);
    }
}

void ParticleTuningUI::Update(float deltaTime)
{
    for (auto& slider : m_sliders)
    {
        slider.scrollbar->Update(deltaTime);
        
        // Update target value based on scrollbar ratio when user interacts
        float ratio = slider.scrollbar->GetScrollRatio();
        
        // Since scrollbar top is 0 and bottom is 1, inverted: top is 1 (max) and bottom is 0 (min)
        ratio = 1.0f - ratio;
        
        *slider.targetValue = slider.minVal + ratio * (slider.maxVal - slider.minVal);
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << *slider.targetValue;
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
