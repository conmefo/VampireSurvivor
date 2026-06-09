#pragma once
#include "../Core/UIElement.h"
#include "PowerUpCard.h"
#include "../../Core/Data/PowerUpData.h"
#include <vector>
#include <functional>

class UIGridLayout : public UIElement
{
public:
    UIGridLayout(TextureAtlas& atlas, const sf::Font& font);
    ~UIGridLayout() override = default;

    void SetDataset(const std::vector<PowerUpData>& data);
    void SelectIndex(int index);
    
    // O(1) mathematical hit detection
    int GetIndexAtPosition(float x, float y) const;

    void Update(float deltaTime) override;
    void Draw(sf::RenderTarget& target) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window);

    void SetPosition(const sf::Vector2f& pos) override;
    void SetScale(float scale);
    void SetGridProperties(int columns, float cellWidth, float cellHeight, float paddingX, float paddingY);
    void SetOnSelectionChangedCallback(std::function<void(const PowerUpData&)> callback);

private:
    void UpdateLayout();

    PowerUpCard m_stampCard;
    std::vector<PowerUpData> m_dataset;
    NineSliceComponent m_selectionFrame;
    
    int m_columns = 4;
    float m_cellWidth = 174.0f;
    float m_cellHeight = 174.0f;
    float m_paddingX = 15.0f;
    float m_paddingY = 15.0f;
    float m_scale = 1.0f;

    int m_pressedIndex = -1;
    int m_focusedIndex = -1;
    std::function<void(const PowerUpData&)> m_onSelectionChanged;
};
