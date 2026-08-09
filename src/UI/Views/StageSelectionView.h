#pragma once

#include "../Core/UIElement.h"
#include "../Panels/MainBoardPanel.h"
#include "../Elements/UIButton.h"
#include "../Components/UIScrollView.h"
#include "../Elements/StageItemWidget.h"
#include "../../Core/Data/StageWaveDataManager.h"
#include "../../Core/Resources/TextureAtlas.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <functional>

class StageListPanel : public UIElement
{
public:
    StageListPanel(TextureAtlas& atlas, const sf::Font& font, const sf::Font* boldFont);
    ~StageListPanel() override = default;

    void PopulateStages(const std::vector<StageInfo>& stageInfos);
    void SetOnSelectionChanged(std::function<void(const StageInfo&)> callback);

    void SelectFirst();
    const StageInfo* GetSelectedStage() const;

    void Update(float deltaTime) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void Draw(sf::RenderTarget& target) override;
    void SetPosition(const sf::Vector2f& pos) override;
    void SetSize(const sf::Vector2f& size) override;

private:
    TextureAtlas& m_atlas;
    const sf::Font& m_font;
    const sf::Font* m_boldFont;

    std::vector<std::unique_ptr<StageItemWidget>> m_items;
    std::string m_selectedStageKey;
    std::function<void(const StageInfo&)> m_onSelectionChanged;

    static constexpr float ITEM_WIDTH = 745.0f;
    static constexpr float ITEM_HEIGHT = 151.0f;
    static constexpr float ITEM_SPACING = 25.0f;
    static constexpr float START_OFFSET_X = -5.0f;
    static constexpr float START_OFFSET_Y = 3.0f;

    void RearrangeItems();
};

class GoldDisplayWidget;
class PlayerProgressionManager;

class StageSelectionView : public UIElement
{
public:
    StageSelectionView(TextureAtlas& atlas, const sf::Font& font, const sf::Font* boldFont, const std::vector<StageInfo>& stageInfos, const PlayerProgressionManager* progressionManager = nullptr);
    ~StageSelectionView() override;

    void InitializeLayout(const sf::Vector2f& viewportSize);

    void SetOnBackClicked(std::function<void()> callback);
    void SetOnConfirmClicked(std::function<void(const StageInfo&)> callback);

    void UpdateCursors(float deltaTime);

    void Update(float deltaTime) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void Draw(sf::RenderTarget& target) override;

private:
    std::unique_ptr<MainBoardPanel> m_mainBoard;
    std::unique_ptr<UIScrollView> m_scrollView;
    StageListPanel* m_stageListRaw = nullptr;
    std::unique_ptr<GoldDisplayWidget> m_goldDisplay;

    std::unique_ptr<UIButton> m_confirmButton;
    std::unique_ptr<UIButton> m_backButton;

    std::function<void()> m_onBackClicked;
    std::function<void(const StageInfo&)> m_onConfirmClicked;

    std::vector<AssetTextureData> m_cursorFrames;
    sf::Sprite m_leftCursor;
    sf::Sprite m_rightCursor;
    float m_cursorAnimTimer = 0.0f;
    int m_currentCursorFrame = 0;
    static constexpr float CURSOR_FRAME_DURATION = 0.1f;
};
