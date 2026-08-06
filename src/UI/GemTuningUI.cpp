#include "GemTuningUI.h"
#include "../Entities/Pickups/ExperienceGemManager.h"
#include <iomanip>
#include <sstream>

GemTuningUI::GemTuningUI(TextureAtlas& atlas, const sf::Font& font, ExperienceGemManager& gemManager)
    : m_atlas(atlas)
    , m_font(font)
    , m_gemManager(gemManager)
{
    m_outwardSpeed = m_gemManager.GetBaseOutwardSpeed();
    m_inwardAccel = m_gemManager.GetBaseInwardAccel();
    m_maxSpeed = m_gemManager.GetMaxMagnetSpeed();
    m_magnetRadiusMult = m_gemManager.GetMagnetRadiusMultiplier();

    m_background = std::make_unique<UIPanel>(m_atlas, "frame1_c2", 10.0f, 10.0f, 10.0f, 10.0f);
    m_background->SetSize(sf::Vector2f(280.0f, 320.0f));

    AddSlider("OutwardSpeed", 0.0f, 500.0f, &m_outwardSpeed);
    AddSlider("InwardAccel", 200.0f, 5000.0f, &m_inwardAccel);
    AddSlider("MaxMagSpeed", 300.0f, 2000.0f, &m_maxSpeed);
    AddSlider("MagRadMult", 0.5f, 5.0f, &m_magnetRadiusMult);

    SetPosition(sf::Vector2f(20.0f, 100.0f));
}

void GemTuningUI::AddSlider(const std::string& name, float min, float max, float* targetPtr)
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

    float val = *targetPtr;
    float ratio = (val - min) / (max - min);
    ratio = std::max(0.0f, std::min(1.0f, ratio));
    entry.scrollbar->SetScrollRatio(1.0f - ratio);

    m_sliders.push_back(std::move(entry));
}

void GemTuningUI::SetPosition(const sf::Vector2f& pos)
{
    UIElement::SetPosition(pos);
    if (m_background)
    {
        m_background->SetPosition(pos);
    }

    float startX = pos.x + 30.0f;
    float startY = pos.y + 40.0f;

    for (std::size_t i = 0; i < m_sliders.size(); ++i)
    {
        float x = startX + static_cast<float>(i) * 60.0f;
        m_sliders[i].label.setPosition(x, startY - 30.0f);
        m_sliders[i].label.setRotation(90.0f);

        m_sliders[i].valueText.setPosition(x + 10.0f, startY + 220.0f);

        m_sliders[i].scrollbar->SetPosition(sf::Vector2f(x, startY));
        m_sliders[i].scrollbar->UpdateLayout(200.0f, 400.0f);

        float val = *m_sliders[i].targetValue;
        float ratio = (val - m_sliders[i].minVal) / (m_sliders[i].maxVal - m_sliders[i].minVal);
        ratio = std::max(0.0f, std::min(1.0f, ratio));
        m_sliders[i].scrollbar->SetScrollRatio(1.0f - ratio);
    }
}

void GemTuningUI::Update(float deltaTime)
{
    for (auto& slider : m_sliders)
    {
        slider.scrollbar->Update(deltaTime);
        float ratio = 1.0f - slider.scrollbar->GetScrollRatio();
        *slider.targetValue = slider.minVal + ratio * (slider.maxVal - slider.minVal);

        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << *slider.targetValue;
        slider.valueText.setString(ss.str());
    }

    m_gemManager.SetBaseOutwardSpeed(m_outwardSpeed);
    m_gemManager.SetBaseInwardAccel(m_inwardAccel);
    m_gemManager.SetMaxMagnetSpeed(m_maxSpeed);
    m_gemManager.SetMagnetRadiusMultiplier(m_magnetRadiusMult);
}

void GemTuningUI::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    for (auto& entry : m_sliders)
    {
        entry.scrollbar->HandleEvent(event, window);
    }
}

void GemTuningUI::Draw(sf::RenderTarget& target)
{
    if (m_background)
    {
        m_background->Draw(target);
    }

    for (auto& entry : m_sliders)
    {
        target.draw(entry.label);
        entry.scrollbar->Draw(target);
        target.draw(entry.valueText);
    }
}
