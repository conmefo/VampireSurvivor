#include "ReelColorTuningUI.h"
#include <algorithm>
#include <cmath>

ReelColorTuningUI::ReelColorTuningUI(const sf::Font& font, TreasurePhase2Config& config)
    : m_font(font)
    , m_config(config)
{
    SetPosition(sf::Vector2f(40.0f, 40.0f));
}

void ReelColorTuningUI::SetPosition(const sf::Vector2f& pos)
{
    UIElement::SetPosition(pos);
}

bool ReelColorTuningUI::ContainsPoint(const sf::Vector2f& point) const
{
    sf::Vector2f panelPos = GetPosition();
    sf::FloatRect bounds(panelPos.x, panelPos.y, 450.0f, 385.0f);
    return bounds.contains(point);
}

void ReelColorTuningUI::Update(float /*deltaTime*/)
{
}

void ReelColorTuningUI::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased)
    {
        sf::Vector2i pixelPos(event.mouseButton.x, event.mouseButton.y);
        mousePos = window.mapPixelToCoords(pixelPos);
    }

    sf::Vector2f panelPos = GetPosition();

    if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        // 1. Set Selector Tabs (Set 1: 1-Item, Set 2: 3-Item, Set 3: 5-Item)
        for(std::size_t s = 0; s < 3; ++s)
        {
            sf::FloatRect setTabRect(panelPos.x + 20.0f + s * 135.0f, panelPos.y + 40.0f, 130.0f, 32.0f);
            if(setTabRect.contains(mousePos))
            {
                m_selectedSetIndex = s;
                std::size_t maxReels = (s == 0) ? 1 : ((s == 1) ? 3 : 5);
                if(m_selectedReelIndex >= maxReels)
                {
                    m_selectedReelIndex = 0;
                }
                return;
            }
        }

        // 2. Reel Sub-Tabs within selected Set
        std::size_t reelCount = (m_selectedSetIndex == 0) ? 1 : ((m_selectedSetIndex == 1) ? 3 : 5);
        float tabW = (m_selectedSetIndex == 0) ? 410.0f : ((m_selectedSetIndex == 1) ? 132.0f : 77.0f);
        for(std::size_t r = 0; r < reelCount; ++r)
        {
            sf::FloatRect reelTabRect(panelPos.x + 20.0f + r * (tabW + 6.0f), panelPos.y + 80.0f, tabW, 36.0f);
            if(reelTabRect.contains(mousePos))
            {
                m_selectedReelIndex = r;
                return;
            }
        }

        // 3. Copy to All Reels In Active Set Button
        sf::FloatRect copyBtnRect(panelPos.x + 20.0f, panelPos.y + 335.0f, 220.0f, 32.0f);
        if(copyBtnRect.contains(mousePos))
        {
            sf::Color curCol = (m_selectedSetIndex == 0) ? m_config.colors1Item[0]
                             : (m_selectedSetIndex == 1) ? m_config.colors3Item[m_selectedReelIndex]
                             : m_config.colors5Item[m_selectedReelIndex];

            if(m_selectedSetIndex == 0)
            {
                m_config.colors1Item[0] = curCol;
            }
            else if(m_selectedSetIndex == 1)
            {
                for(int r = 0; r < 3; ++r) m_config.colors3Item[r] = curCol;
            }
            else
            {
                for(int r = 0; r < 5; ++r) m_config.colors5Item[r] = curCol;
            }
            return;
        }

        // 4. Sliders (4 channels: R, G, B, A)
        sf::Color& curCol = (m_selectedSetIndex == 0) ? m_config.colors1Item[0]
                         : (m_selectedSetIndex == 1) ? m_config.colors3Item[m_selectedReelIndex]
                         : m_config.colors5Item[m_selectedReelIndex];

        for(int c = 0; c < 4; ++c)
        {
            sf::FloatRect trackHitBox(panelPos.x + 20.0f, panelPos.y + 130.0f + c * 48.0f, 320.0f, 26.0f);
            if(trackHitBox.contains(mousePos))
            {
                m_activeSliderDrag = c;
                float ratio = std::clamp((mousePos.x - (panelPos.x + 20.0f)) / 320.0f, 0.0f, 1.0f);
                sf::Uint8 val = static_cast<sf::Uint8>(std::round(ratio * 255.0f));

                if(c == 0) curCol.r = val;
                else if(c == 1) curCol.g = val;
                else if(c == 2) curCol.b = val;
                else if(c == 3) curCol.a = val;
                return;
            }
        }
    }
    else if(event.type == sf::Event::MouseMoved && m_activeSliderDrag >= 0 && m_activeSliderDrag <= 3)
    {
        sf::Color& curCol = (m_selectedSetIndex == 0) ? m_config.colors1Item[0]
                         : (m_selectedSetIndex == 1) ? m_config.colors3Item[m_selectedReelIndex]
                         : m_config.colors5Item[m_selectedReelIndex];

        float ratio = std::clamp((mousePos.x - (panelPos.x + 20.0f)) / 320.0f, 0.0f, 1.0f);
        sf::Uint8 val = static_cast<sf::Uint8>(std::round(ratio * 255.0f));

        if(m_activeSliderDrag == 0) curCol.r = val;
        else if(m_activeSliderDrag == 1) curCol.g = val;
        else if(m_activeSliderDrag == 2) curCol.b = val;
        else if(m_activeSliderDrag == 3) curCol.a = val;
    }
    else if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
    {
        m_activeSliderDrag = -1;
    }
}

void ReelColorTuningUI::Draw(sf::RenderTarget& target)
{
    sf::Vector2f panelPos = GetPosition();
    sf::Vector2f panelSize(450.0f, 385.0f);

    // 1. Background Panel
    sf::RectangleShape bg(panelSize);
    bg.setPosition(panelPos);
    bg.setFillColor(sf::Color(18, 18, 28, 240));
    bg.setOutlineColor(sf::Color(80, 80, 140, 220));
    bg.setOutlineThickness(2.0f);
    target.draw(bg);

    // 2. Panel Title
    sf::Text title;
    title.setFont(m_font);
    title.setString("REEL BEAM COLORS (3 COLOR SETS)");
    title.setCharacterSize(15);
    title.setFillColor(sf::Color(255, 215, 0));
    title.setPosition(panelPos.x + 20.0f, panelPos.y + 12.0f);
    target.draw(title);

    // 3. Set Selector Tabs (1-Item, 3-Item, 5-Item)
    const std::string setNames[3] = {"Set 1 (1-Item)", "Set 2 (3-Item)", "Set 3 (5-Item)"};
    for(std::size_t s = 0; s < 3; ++s)
    {
        bool isSetSelected = (s == m_selectedSetIndex);
        sf::Vector2f tabPos(panelPos.x + 20.0f + s * 135.0f, panelPos.y + 40.0f);
        sf::Vector2f tabSize(130.0f, 32.0f);

        sf::RectangleShape tab(tabSize);
        tab.setPosition(tabPos);
        tab.setFillColor(isSetSelected ? sf::Color(70, 70, 120) : sf::Color(35, 35, 55));
        tab.setOutlineColor(isSetSelected ? sf::Color(255, 215, 0) : sf::Color(65, 65, 90));
        tab.setOutlineThickness(isSetSelected ? 2.0f : 1.0f);
        target.draw(tab);

        sf::Text tabText;
        tabText.setFont(m_font);
        tabText.setString(setNames[s]);
        tabText.setCharacterSize(13);
        tabText.setFillColor(isSetSelected ? sf::Color::White : sf::Color(170, 170, 190));
        sf::FloatRect b = tabText.getLocalBounds();
        tabText.setPosition(tabPos.x + (tabSize.x - b.width) * 0.5f - b.left, tabPos.y + (tabSize.y - b.height) * 0.5f - b.top);
        target.draw(tabText);
    }

    // 4. Reel Sub-Tabs within selected Set
    std::size_t reelCount = (m_selectedSetIndex == 0) ? 1 : ((m_selectedSetIndex == 1) ? 3 : 5);
    float tabW = (m_selectedSetIndex == 0) ? 410.0f : ((m_selectedSetIndex == 1) ? 132.0f : 77.0f);

    for(std::size_t r = 0; r < reelCount; ++r)
    {
        bool isReelSelected = (r == m_selectedReelIndex);
        sf::Vector2f rTabPos(panelPos.x + 20.0f + r * (tabW + 6.0f), panelPos.y + 80.0f);
        sf::Vector2f rTabSize(tabW, 36.0f);

        sf::RectangleShape rTab(rTabSize);
        rTab.setPosition(rTabPos);
        rTab.setFillColor(isReelSelected ? sf::Color(55, 75, 115) : sf::Color(30, 30, 48));
        rTab.setOutlineColor(isReelSelected ? sf::Color(255, 215, 0) : sf::Color(60, 60, 85));
        rTab.setOutlineThickness(isReelSelected ? 2.0f : 1.0f);
        target.draw(rTab);

        sf::Text rText;
        rText.setFont(m_font);
        rText.setString("Reel " + std::to_string(r + 1));
        rText.setCharacterSize(13);
        rText.setFillColor(isReelSelected ? sf::Color::White : sf::Color(180, 180, 200));
        sf::FloatRect b = rText.getLocalBounds();
        rText.setPosition(rTabPos.x + (rTabSize.x - b.width) * 0.5f - b.left, rTabPos.y + 3.0f);
        target.draw(rText);

        // Color Swatch on Reel Sub-Tab
        sf::Color swatchCol = (m_selectedSetIndex == 0) ? m_config.colors1Item[0]
                            : (m_selectedSetIndex == 1) ? m_config.colors3Item[r]
                            : m_config.colors5Item[r];

        sf::RectangleShape swatch(sf::Vector2f(rTabSize.x - 16.0f, 10.0f));
        swatch.setPosition(rTabPos.x + 8.0f, rTabPos.y + 21.0f);
        swatch.setFillColor(swatchCol);
        swatch.setOutlineColor(sf::Color::Black);
        swatch.setOutlineThickness(1.0f);
        target.draw(swatch);
    }

    // 5. Sliders for Selected Reel
    const sf::Color& curCol = (m_selectedSetIndex == 0) ? m_config.colors1Item[0]
                            : (m_selectedSetIndex == 1) ? m_config.colors3Item[m_selectedReelIndex]
                            : m_config.colors5Item[m_selectedReelIndex];

    const std::string labels[4] = {"Red", "Green", "Blue", "Alpha"};
    const sf::Uint8 values[4] = {curCol.r, curCol.g, curCol.b, curCol.a};
    const sf::Color fillColors[4] = {
        sf::Color(235, 60, 60),   // Red
        sf::Color(60, 220, 60),   // Green
        sf::Color(60, 120, 255),  // Blue
        sf::Color(200, 200, 220)  // Alpha
    };

    float trackWidth = 320.0f;

    for(int c = 0; c < 4; ++c)
    {
        float startY = panelPos.y + 128.0f + c * 48.0f;

        // Label and Value
        sf::Text labelText;
        labelText.setFont(m_font);
        labelText.setString(labels[c] + ": " + std::to_string(values[c]));
        labelText.setCharacterSize(14);
        labelText.setFillColor(sf::Color::White);
        labelText.setPosition(panelPos.x + 20.0f, startY);
        target.draw(labelText);

        // Slider Background Track (320px wide)
        sf::Vector2f trackPos(panelPos.x + 20.0f, startY + 22.0f);
        sf::RectangleShape trackBg(sf::Vector2f(trackWidth, 12.0f));
        trackBg.setPosition(trackPos);
        trackBg.setFillColor(sf::Color(35, 35, 48));
        trackBg.setOutlineColor(sf::Color(70, 70, 95));
        trackBg.setOutlineThickness(1.0f);
        target.draw(trackBg);

        // Slider Fill Bar
        float fillW = (static_cast<float>(values[c]) / 255.0f) * trackWidth;
        if(fillW > 0.0f)
        {
            sf::RectangleShape fillBar(sf::Vector2f(fillW, 12.0f));
            fillBar.setPosition(trackPos);
            fillBar.setFillColor(fillColors[c]);
            target.draw(fillBar);
        }

        // Slider Thumb Handle
        sf::RectangleShape handle(sf::Vector2f(16.0f, 20.0f));
        handle.setPosition(trackPos.x + fillW - 8.0f, trackPos.y - 4.0f);
        handle.setFillColor(sf::Color(245, 245, 255));
        handle.setOutlineColor(sf::Color::Black);
        handle.setOutlineThickness(1.5f);
        target.draw(handle);
    }

    // 6. Combined Color Preview Box (Right side of sliders)
    float previewX = panelPos.x + 355.0f;
    float previewY = panelPos.y + 140.0f;
    sf::Vector2f previewSize(75.0f, 130.0f);

    sf::RectangleShape chkBg(previewSize);
    chkBg.setPosition(previewX, previewY);
    chkBg.setFillColor(sf::Color(50, 50, 60));
    chkBg.setOutlineColor(sf::Color::White);
    chkBg.setOutlineThickness(1.5f);
    target.draw(chkBg);

    sf::RectangleShape previewFill(previewSize);
    previewFill.setPosition(previewX, previewY);
    previewFill.setFillColor(curCol);
    target.draw(previewFill);

    sf::Text prevLabel;
    prevLabel.setFont(m_font);
    prevLabel.setString("PREVIEW");
    prevLabel.setCharacterSize(12);
    prevLabel.setFillColor(sf::Color::White);
    sf::FloatRect pb = prevLabel.getLocalBounds();
    prevLabel.setPosition(previewX + (previewSize.x - pb.width) * 0.5f - pb.left, previewY + previewSize.y + 6.0f);
    target.draw(prevLabel);

    // 7. Copy to All Reels In Set Button
    sf::Vector2f copyPos(panelPos.x + 20.0f, panelPos.y + 335.0f);
    sf::Vector2f copySize(220.0f, 32.0f);
    sf::RectangleShape copyBtn(copySize);
    copyBtn.setPosition(copyPos);
    copyBtn.setFillColor(sf::Color(45, 75, 130));
    copyBtn.setOutlineColor(sf::Color(100, 150, 240));
    copyBtn.setOutlineThickness(1.5f);
    target.draw(copyBtn);

    sf::Text copyText;
    copyText.setFont(m_font);
    copyText.setString("COPY TO ALL REELS IN SET");
    copyText.setCharacterSize(12);
    copyText.setFillColor(sf::Color::White);
    sf::FloatRect cb = copyText.getLocalBounds();
    copyText.setPosition(copyPos.x + (copySize.x - cb.width) * 0.5f - cb.left, copyPos.y + (copySize.y - cb.height) * 0.5f - cb.top);
    target.draw(copyText);
}
