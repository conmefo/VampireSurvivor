#include "StatsPanel.h"
#include "../../Core/Data/CharacterProfile.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <sstream>
#include <iomanip>
#include <iostream>

static sf::Font s_boldFont;
static bool s_boldFontLoaded = false;

StatsPanel::StatsPanel(TextureAtlas& atlas, const sf::Font& font)
    : UIPanel(atlas, "frame1_c2", 10, 10, 10, 10)
    , m_font(font)
{
    m_statKeys = {
        "MaxHealth",
        "Recovery",
        "Armor",
        "MoveSpeed",
        "Might",
        "Speed",
        "Duration",
        "Area",
        "Cooldown",
        "Amount",
        "Revival",
        "Magnet",
        "Luck",
        "Growth",
        "Greed",
        "Curse",
        "Reroll",
        "Skip",
        "Banish"
    };

    if(!s_boldFontLoaded)
    {
        if(s_boldFont.loadFromFile("Assets/Fonts/courier_bold.ttf"))
        {
            s_boldFontLoaded = true;
        }
        else
        {
            std::cerr << "Failed to load bold font for StatsPanel." << std::endl;
            s_boldFont = m_font; // fallback
            s_boldFontLoaded = true;
        }
    }

    for(const std::string& key : m_statKeys)
    {
        StatRow row;
        row.key = key;

        row.labelText.setFont(s_boldFont);
        row.labelText.setCharacterSize(TEXT_SIZE);
        row.labelText.setString(FormatLabelName(key));
        row.labelText.setFillColor(sf::Color(220, 220, 220));

        row.valueText.setCharacterSize(TEXT_SIZE);
        row.valueText.setString("-");
        row.valueText.setFillColor(sf::Color::White);
        

        
        row.valueText.setFont(s_boldFont);

        m_rows.push_back(row);
    }
}

std::string StatsPanel::FormatLabelName(const std::string& key) const
{
    if(key == "MaxHealth") return "Max Health";
    if(key == "MoveSpeed") return "Move Speed";
    return key;
}

void StatsPanel::SetCharacterProfile(const CharacterProfile& profile)
{
    for(StatRow& row : m_rows)
    {
        float value = profile.GetStat(row.key);
        FormatStatText(row, value);
    }
    
    // Re-align right bounds since text width changed
    SetPosition(m_position);
}

void StatsPanel::FormatStatText(StatRow& row, float value) const
{
    if(value == 0.0f)
    {
        row.valueText.setString("-");
        return;
    }

    std::ostringstream oss;
    
    if(row.key == "Speed" || row.key == "Might" || row.key == "Duration" || row.key == "Area" || row.key == "Cooldown" || row.key == "Luck" || row.key == "Growth" || row.key == "Greed" || row.key == "Curse")
    {
        if(value > 0.0f)
        {
            oss << "+";
        }
        oss << static_cast<int>(value * 100.0f) << "%";
    }
    else if(row.key == "Amount" || row.key == "Revival" || row.key == "Reroll" || row.key == "Skip" || row.key == "Banish" || row.key == "Armor" || row.key == "Magnet")
    {
        if(value > 0.0f)
        {
            oss << "+";
        }
        oss << static_cast<int>(value);
    }
    else
    {
        oss << static_cast<int>(value);
    }

    row.valueText.setString(oss.str());
}

void StatsPanel::SetPosition(const sf::Vector2f& pos)
{
    UIPanel::SetPosition(pos);
    
    float currentY = pos.y + START_Y_OFFSET;
    
    for(StatRow& row : m_rows)
    {
        row.labelText.setPosition(pos.x + PADDING_X, currentY);
        
        sf::FloatRect valBounds = row.valueText.getLocalBounds();
        row.valueText.setPosition(pos.x + m_size.x - PADDING_X - valBounds.width, currentY);
        
        currentY += ROW_PADDING;
        
        if(row.key == "MoveSpeed" || row.key == "Area" || row.key == "Magnet" || row.key == "Curse")
        {
            currentY += ROW_PADDING * 0.75f; // Add a small visual gap for groupings
        }
    }
}

void StatsPanel::SetSize(const sf::Vector2f& size)
{
    UIPanel::SetSize(size);
    SetPosition(m_position);
}

void StatsPanel::Update(float deltaTime)
{
    UIPanel::Update(deltaTime);
}

void StatsPanel::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
}

void StatsPanel::Draw(sf::RenderTarget& target)
{
    UIPanel::Draw(target);

    for(const StatRow& row : m_rows)
    {
        target.draw(row.labelText);
        target.draw(row.valueText);
    }
}
