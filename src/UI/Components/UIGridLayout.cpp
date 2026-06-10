#include "UIGridLayout.h"
#include <cmath>
#include <algorithm>

UIGridLayout::UIGridLayout(TextureAtlas& atlas, const sf::Font& font)
    : m_stampCard(atlas, font)
{
    m_stampCard.InitDummyData();
    
    m_selectionFrame.SetTexture(atlas, "selectionSquare_03");
    m_selectionFrame.SetMargins(12, 12, 12, 12);
    m_selectionFrame.SetSize(sf::Vector2f(190.0f, 190.0f));
    m_selectionFrame.SetCornerScale(2.0f);
}

void UIGridLayout::SetDataset(const std::vector<PowerUpData>& data)
{
    m_dataset = data;
    UpdateLayout();
}

void UIGridLayout::SelectIndex(int index)
{
    if (index >= 0 && index < static_cast<int>(m_dataset.size()))
    {
        m_focusedIndex = index;
        if (m_onSelectionChanged)
        {
            m_onSelectionChanged(m_dataset[m_focusedIndex]);
        }
    }
}

void UIGridLayout::SetGridProperties(int columns, float cellWidth, float cellHeight, float paddingX, float paddingY)
{
    m_columns = columns;
    m_cellWidth = cellWidth;
    m_cellHeight = cellHeight;
    m_paddingX = paddingX;
    m_paddingY = paddingY;
    UpdateLayout();
}

int UIGridLayout::GetIndexAtPosition(float x, float y) const
{
    float localX = x - m_position.x;
    float localY = y - m_position.y;
    
    if(localX < 0 || localY < 0)
    {
        return -1;
    }
    
    float totalCellWidth = (m_cellWidth + m_paddingX) * m_scale;
    float totalCellHeight = (m_cellHeight + m_paddingY) * m_scale;
    
    int col = static_cast<int>(localX / totalCellWidth);
    int row = static_cast<int>(localY / totalCellHeight);
    
    if(col >= m_columns)
    {
        return -1;
    }
    
    float remX = std::fmod(localX, totalCellWidth);
    float remY = std::fmod(localY, totalCellHeight);
    
    if(remX > m_cellWidth * m_scale || remY > m_cellHeight * m_scale)
    {
        return -1;
    }
    
    int index = row * m_columns + col;
    if(index >= 0 && index < static_cast<int>(m_dataset.size()))
    {
        return index;
    }
    
    return -1;
}

void UIGridLayout::UpdateLayout()
{
    int rows = std::ceil(static_cast<float>(m_dataset.size()) / m_columns);
    float totalHeight = rows * (m_cellHeight + m_paddingY) * m_scale - (m_paddingY * m_scale);
    float totalWidth = m_columns * (m_cellWidth + m_paddingX) * m_scale - (m_paddingX * m_scale);
    m_size = sf::Vector2f(totalWidth, std::max(0.0f, totalHeight));
}

void UIGridLayout::SetPosition(const sf::Vector2f& pos)
{
    UIElement::SetPosition(pos);
    UpdateLayout();
}

void UIGridLayout::SetScale(float scale)
{
    m_scale = scale;
    m_stampCard.SetScale(scale);
    
    m_selectionFrame.SetSize(sf::Vector2f(190.0f * m_scale, 190.0f * m_scale));
    
    UpdateLayout();
}

void UIGridLayout::SetOnSelectionChangedCallback(std::function<void(const PowerUpData&)> callback)
{
    m_onSelectionChanged = callback;
}

void UIGridLayout::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
        m_pressedIndex = GetIndexAtPosition(worldPos.x, worldPos.y);
    }
    else if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
        int releaseIndex = GetIndexAtPosition(worldPos.x, worldPos.y);
        
        if(releaseIndex != -1 && releaseIndex == m_pressedIndex)
        {
            m_focusedIndex = releaseIndex;
            if(m_onSelectionChanged)
            {
                m_onSelectionChanged(m_dataset[m_focusedIndex]);
            }
        }
        m_pressedIndex = -1;
    }
}

void UIGridLayout::Update(float deltaTime)
{
}

void UIGridLayout::Draw(sf::RenderTarget& target)
{
    float totalCellWidth = (m_cellWidth + m_paddingX) * m_scale;
    float totalCellHeight = (m_cellHeight + m_paddingY) * m_scale;

    for(size_t i = 0; i < m_dataset.size(); ++i)
    {
        int col = i % m_columns;
        int row = i / m_columns;

        float x = m_position.x + col * totalCellWidth + (m_cellWidth * m_scale) / 2.0f;
        float y = m_position.y + row * totalCellHeight + (m_cellHeight * m_scale) / 2.0f;

        m_stampCard.SetPosition(sf::Vector2f(x, y));
        m_stampCard.InjectData(m_dataset[i]);
        
        if(static_cast<int>(i) == m_pressedIndex)
        {
            m_stampCard.SetBgColor(sf::Color(150, 150, 150));
        }
        else
        {
            m_stampCard.SetBgColor(sf::Color::White);
        }
        
        m_stampCard.Draw(target);
        
        if(static_cast<int>(i) == m_focusedIndex)
        {
            m_selectionFrame.setPosition(x - (190.0f * m_scale) / 2.0f, y - (190.0f * m_scale) / 2.0f);
            m_selectionFrame.Update();
            target.draw(m_selectionFrame);
        }
    }
}
