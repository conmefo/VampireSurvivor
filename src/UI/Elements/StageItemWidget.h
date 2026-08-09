#pragma once

#include "../Core/UIElement.h"
#include "../Components/NineSliceComponent.h"
#include "../../Core/Data/StageWaveDataManager.h"
#include "../../Core/Resources/TextureAtlas.h"
#include <SFML/Graphics.hpp>
#include <functional>

class StageItemWidget : public UIElement
{
public:
    StageItemWidget(TextureAtlas& atlas, const sf::Font& font, const sf::Font* boldFont, const StageInfo& stageInfo);
    ~StageItemWidget() override = default;

    void InitializeLayout(const sf::Vector2f& size);

    void SetSelected(bool selected);
    bool IsSelected() const { return m_isSelected; }

    const StageInfo& GetStageInfo() const { return m_stageInfo; }

    void SetOnClick(std::function<void(const std::string&)> callback);

    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void Update(float deltaTime) override;
    void Draw(sf::RenderTarget& target) override;
    void SetPosition(const sf::Vector2f& pos) override;

private:
    StageInfo m_stageInfo;
    bool m_isSelected = false;

    NineSliceComponent m_bgPanel;
    NineSliceComponent m_selectionFrame;
    sf::RectangleShape m_dimOverlay;

    sf::Sprite m_iconSprite;
    sf::Text m_titleText;
    sf::Text m_descText;

    std::function<void(const std::string&)> m_onClick;

    static constexpr float ICON_WIDTH = 245.0f;
    static constexpr float ICON_HEIGHT = 151.0f;
};
