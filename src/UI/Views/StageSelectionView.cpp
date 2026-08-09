#include "StageSelectionView.h"
#include "../../Core/WindowSettings.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>

// ============================================================================
// StageListPanel Implementation
// ============================================================================

StageListPanel::StageListPanel(TextureAtlas& atlas, const sf::Font& font, const sf::Font* boldFont)
    : m_atlas(atlas), m_font(font), m_boldFont(boldFont)
{
}

void StageListPanel::PopulateStages(const std::vector<StageInfo>& stageInfos)
{
    m_items.clear();
    for (const auto& info : stageInfos)
    {
        auto item = std::make_unique<StageItemWidget>(m_atlas, m_font, m_boldFont, info);

        item->SetOnClick([this, info](const std::string& key) {
            for (auto& w : m_items)
            {
                w->SetSelected(w->GetStageInfo().stageKey == key);
            }
            m_selectedStageKey = key;
            if (m_onSelectionChanged)
            {
                m_onSelectionChanged(info);
            }
        });

        m_items.push_back(std::move(item));
    }

    RearrangeItems();
    SelectFirst();
}

void StageListPanel::RearrangeItems()
{
    float totalH = START_OFFSET_Y + m_items.size() * (ITEM_HEIGHT + ITEM_SPACING) + 20.0f;
    SetSize(sf::Vector2f(ITEM_WIDTH, totalH));

    for (size_t i = 0; i < m_items.size(); ++i)
    {
        float itemX = m_position.x + START_OFFSET_X;
        float itemY = m_position.y + START_OFFSET_Y + i * (ITEM_HEIGHT + ITEM_SPACING);
        m_items[i]->SetPosition(sf::Vector2f(itemX, itemY));
        m_items[i]->InitializeLayout(sf::Vector2f(ITEM_WIDTH, ITEM_HEIGHT));
    }
}

void StageListPanel::SelectFirst()
{
    if (!m_items.empty())
    {
        for (size_t i = 0; i < m_items.size(); ++i)
        {
            m_items[i]->SetSelected(i == 0);
        }
        m_selectedStageKey = m_items[0]->GetStageInfo().stageKey;
        if (m_onSelectionChanged)
        {
            m_onSelectionChanged(m_items[0]->GetStageInfo());
        }
    }
}

const StageInfo* StageListPanel::GetSelectedStage() const
{
    for (const auto& item : m_items)
    {
        if (item->GetStageInfo().stageKey == m_selectedStageKey)
        {
            return &item->GetStageInfo();
        }
    }
    return nullptr;
}

void StageListPanel::SetOnSelectionChanged(std::function<void(const StageInfo&)> callback)
{
    m_onSelectionChanged = std::move(callback);
}

void StageListPanel::SetPosition(const sf::Vector2f& pos)
{
    UIElement::SetPosition(pos);
    RearrangeItems();
}

void StageListPanel::SetSize(const sf::Vector2f& size)
{
    UIElement::SetSize(size);
}

void StageListPanel::Update(float deltaTime)
{
    for (auto& item : m_items)
    {
        item->Update(deltaTime);
    }
}

void StageListPanel::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    for (auto& item : m_items)
    {
        item->HandleEvent(event, window);
    }
}

void StageListPanel::Draw(sf::RenderTarget& target)
{
    for (auto& item : m_items)
    {
        item->Draw(target);
    }
}

// ============================================================================
// StageSelectionView Implementation
#include "StageSelectionView.h"
#include "../Elements/GoldDisplayWidget.h"
#include "../Core/TextUtility.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>

// ... (StageListPanel methods stay unchanged)

StageSelectionView::StageSelectionView(TextureAtlas& atlas, const sf::Font& font, const sf::Font* boldFont, const std::vector<StageInfo>& stageInfos, const PlayerProgressionManager* progressionManager)
    : m_mainBoard(std::make_unique<MainBoardPanel>(atlas, font, boldFont))
    , m_goldDisplay(std::make_unique<GoldDisplayWidget>(atlas, progressionManager, font))
    , m_confirmButton(std::make_unique<UIButton>(atlas, "button_c5_normal", 10, 10, 10, 10))
    , m_backButton(std::make_unique<UIButton>(atlas, "button_c8_normal", 10, 10, 10, 10))
{
    m_mainBoard->SetTitle("Stage Selection");
    m_mainBoard->SetShowDetailPanel(false);

    m_confirmButton->SetText("START", font, 40);
    m_confirmButton->SetCornerScale(2.5f);

    m_backButton->SetText("BACK", font, 24);
    m_backButton->SetCornerScale(2.0f);

    if (m_goldDisplay)
    {
        m_goldDisplay->SetPosition(sf::Vector2f(GoldDisplayWidget::DEFAULT_X, GoldDisplayWidget::DEFAULT_Y));
    }

    // Cursors for START button
    for (int i = 1; i <= 8; ++i)
    {
        AssetTextureData data = atlas.GetTextureData("arrow_0" + std::to_string(i));
        if (data.texture) m_cursorFrames.push_back(data);
    }
    if (!m_cursorFrames.empty())
    {
        m_leftCursor.setTexture(*m_cursorFrames[0].texture);
        m_leftCursor.setTextureRect(m_cursorFrames[0].rect);
        m_rightCursor.setTexture(*m_cursorFrames[0].texture);
        m_rightCursor.setTextureRect(m_cursorFrames[0].rect);
        m_leftCursor.setOrigin(m_cursorFrames[0].rect.width / 2.0f, m_cursorFrames[0].rect.height / 2.0f);
        m_rightCursor.setOrigin(m_cursorFrames[0].rect.width / 2.0f, m_cursorFrames[0].rect.height / 2.0f);
        m_leftCursor.setScale(2.0f, 2.0f);
        m_rightCursor.setScale(-2.0f, 2.0f);
    }

    auto stageList = std::make_unique<StageListPanel>(atlas, font, boldFont);
    m_stageListRaw = stageList.get();
    m_stageListRaw->PopulateStages(stageInfos);

    m_scrollView = std::make_unique<UIScrollView>(100.0f, 100.0f, atlas);
    m_scrollView->SetContent(std::move(stageList));

    m_confirmButton->SetOnClickCallback([this]() {
        if (m_stageListRaw)
        {
            const StageInfo* selected = m_stageListRaw->GetSelectedStage();
            if (selected && m_onConfirmClicked)
            {
                m_onConfirmClicked(*selected);
            }
        }
    });

    m_backButton->SetOnClickCallback([this]() {
        if (m_onBackClicked)
        {
            m_onBackClicked();
        }
    });
}

StageSelectionView::~StageSelectionView() = default;

void StageSelectionView::InitializeLayout(const sf::Vector2f& viewportSize)
{
    static constexpr float BOARD_WIDTH = 805.0f;
    static constexpr float BOARD_HEIGHT = 980.0f;

    float boardX = (viewportSize.x - BOARD_WIDTH) / 2.0f;
    float boardY = (viewportSize.y - BOARD_HEIGHT) / 2.0f + 50.0f;

    if (m_mainBoard)
    {
        m_mainBoard->InitializeLayout(sf::Vector2f(BOARD_WIDTH, BOARD_HEIGHT));
        m_mainBoard->SetPosition(sf::Vector2f(boardX, boardY));
    }

    float scrollX = boardX + 20.0f;
    float scrollY = boardY + 90.0f;
    float scrollW = BOARD_WIDTH - 47.0f;
    float scrollH = BOARD_HEIGHT - 115.0f;

    if (m_scrollView)
    {
        m_scrollView->SetSize(sf::Vector2f(scrollW, scrollH));
        m_scrollView->SetPosition(sf::Vector2f(scrollX, scrollY));
        m_scrollView->SetPadding(10.0f, 15.0f);

        if (m_stageListRaw)
        {
            m_stageListRaw->SetSize(sf::Vector2f(scrollW - 20.0f, scrollH));
        }
    }

    float confirmWidth = 314.0f;
    float confirmHeight = 90.0f;
    float confirmX = viewportSize.x - confirmWidth - 80.0f;
    float confirmY = viewportSize.y - confirmHeight - 20.0f;
    if (m_confirmButton)
    {
        m_confirmButton->SetPosition(sf::Vector2f(confirmX, confirmY));
        m_confirmButton->SetSize(sf::Vector2f(confirmWidth, confirmHeight));
    }

    float backWidth = 130.0f;
    float backHeight = 54.0f;
    float backX = viewportSize.x * 0.65f - (backWidth / 2.0f);
    float topY = 54.0f - (backHeight / 2.0f);
    if (m_backButton)
    {
        m_backButton->SetPosition(sf::Vector2f(backX, topY));
        m_backButton->SetSize(sf::Vector2f(backWidth, backHeight));
    }
}

void StageSelectionView::SetOnBackClicked(std::function<void()> callback)
{
    m_onBackClicked = std::move(callback);
}

void StageSelectionView::SetOnConfirmClicked(std::function<void(const StageInfo&)> callback)
{
    m_onConfirmClicked = std::move(callback);
}

void StageSelectionView::UpdateCursors(float deltaTime)
{
    if (m_cursorFrames.empty() || !m_confirmButton) return;

    m_cursorAnimTimer += deltaTime;
    if (m_cursorAnimTimer >= CURSOR_FRAME_DURATION)
    {
        m_cursorAnimTimer = 0.0f;
        m_currentCursorFrame = (m_currentCursorFrame + 1) % m_cursorFrames.size();

        const AssetTextureData& frame = m_cursorFrames[m_currentCursorFrame];
        m_leftCursor.setTexture(*frame.texture);
        m_leftCursor.setTextureRect(frame.rect);
        m_leftCursor.setOrigin(frame.rect.width / 2.0f, frame.rect.height / 2.0f);

        m_rightCursor.setTexture(*frame.texture);
        m_rightCursor.setTextureRect(frame.rect);
        m_rightCursor.setOrigin(frame.rect.width / 2.0f, frame.rect.height / 2.0f);
    }

    sf::Vector2f pos = m_confirmButton->GetPosition();
    sf::Vector2f size = m_confirmButton->GetSize();
    float padding = 35.0f;
    m_leftCursor.setPosition(pos.x - padding, pos.y + size.y / 2.0f);
    m_rightCursor.setPosition(pos.x + size.x + padding, pos.y + size.y / 2.0f);
}

void StageSelectionView::Update(float deltaTime)
{
    if (m_mainBoard) m_mainBoard->Update(deltaTime);
    if (m_scrollView) m_scrollView->Update(deltaTime);
    if (m_confirmButton) m_confirmButton->Update(deltaTime);
    if (m_backButton) m_backButton->Update(deltaTime);
    if (m_goldDisplay) m_goldDisplay->Update(deltaTime);

    UpdateCursors(deltaTime);
}

void StageSelectionView::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    if (m_scrollView) m_scrollView->HandleEvent(event, window);
    if (m_confirmButton) m_confirmButton->HandleEvent(event, window);
    if (m_backButton) m_backButton->HandleEvent(event, window);
    if (m_goldDisplay) m_goldDisplay->HandleEvent(event, window);
}

void StageSelectionView::Draw(sf::RenderTarget& target)
{
    if (m_mainBoard) m_mainBoard->Draw(target);
    if (m_scrollView) m_scrollView->Draw(target);
    if (m_confirmButton) m_confirmButton->Draw(target);
    if (m_backButton) m_backButton->Draw(target);
    if (m_goldDisplay) m_goldDisplay->Draw(target);

    if (!m_cursorFrames.empty())
    {
        target.draw(m_leftCursor);
        target.draw(m_rightCursor);
    }
}
