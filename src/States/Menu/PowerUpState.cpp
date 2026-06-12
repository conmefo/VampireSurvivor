#include "PowerUpState.h"
#include "../../Core/WindowSettings.h"
#include "../StateManager.h"
#include <iostream>
#include "../../UI/Components/UIGridLayout.h"
#include "../../UI/Components/PowerUpCard.h"
#include "../../UI/Elements/UIButton.h"
#include "../../UI/Components/UIScrollView.h"
#include "../../UI/Components/UIDetailPanel.h"
#include "../../UI/Elements/GoldDisplayWidget.h"
#include "../../Core/Data/PowerUpData.h"
#include "../../Core/Data/PowerUpDataManager.h"
#include "../../Core/Data/PlayerProgressionManager.h"
#include <vector>

PowerUpState::PowerUpState(StateContext context) : BaseState(context)
{
}

void PowerUpState::Init()
{
    m_darkOverlay.setSize(sf::Vector2f(Core::VIRTUAL_WIDTH, Core::VIRTUAL_HEIGHT));
    m_darkOverlay.setFillColor(sf::Color(0, 0, 0, 150));    // --- Main Panel Setup ---
    m_mainPanelBg.SetTexture(m_context.atlas, "frame5_c4");
    m_mainPanelBg.SetMargins(12, 12, 12, 12);
    m_mainPanelBg.SetSize(sf::Vector2f(810.0f, 975.0f));
    m_mainPanelBg.setPosition(864.0f - 405.0f, 100.0f);
    m_mainPanelBg.SetCornerScale(1.5f);
    m_mainPanelBg.Update();

    m_titleText.setFont(m_context.fonts.Get(FontID::Main));
    m_titleText.setString("PowerUp  Selection");
    m_titleText.setCharacterSize(45);
    m_titleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = m_titleText.getLocalBounds();
    m_titleText.setOrigin(titleBounds.left + titleBounds.width / 2.0f, titleBounds.top + titleBounds.height / 2.0f);
    m_titleText.setPosition(864.0f, 160.0f);

    auto refundButton = std::make_unique<UIButton>(m_context.atlas, "button_c9_mouseover", 12.0f, 12.0f, 12.0f, 12.0f);
    refundButton->SetSize(sf::Vector2f(400.0f, 70.0f));
    refundButton->SetPosition(sf::Vector2f(864.0f - 200.0f, 210.0f));
    refundButton->SetText("Refund PowerUps", m_context.fonts.Get(FontID::Main), 30);
    refundButton->SetTextStyle(sf::Text::Bold);
    refundButton->SetStateColors(sf::Color::White, sf::Color(220, 220, 220), sf::Color(150, 150, 150), sf::Color(100, 100, 100));
    refundButton->SetCornerScale(2.0f);
    refundButton->SetOnClickCallback([this]() {
        m_context.progressionData.RefundAllPowerUps(m_context.powerUpData);
        RefreshData();
    });
    
    auto backButton = std::make_unique<UIButton>(m_context.atlas, "button_c8_normal", 10.0f, 10.0f, 10.0f, 10.0f);
    backButton->SetSize(sf::Vector2f(130.0f, 54.0f));
    backButton->SetPosition(sf::Vector2f(Core::VIRTUAL_WIDTH * 0.65f - 65.0f, 54.0f - 27.0f));
    backButton->SetText("BACK", m_context.fonts.Get(FontID::Main), 26);
    backButton->SetTextStyle(sf::Text::Bold);
    backButton->SetStateColors(sf::Color::White, sf::Color(220, 220, 220), sf::Color(150, 150, 150), sf::Color(100, 100, 100));
    backButton->SetCornerScale(2.0f);
    backButton->SetOnClickCallback([this]() {
        m_context.stateManager.PopState();
    });
    
    // --- ScrollView Setup ---
    auto grid = std::make_unique<UIGridLayout>(m_context.atlas, m_context.fonts.Get(FontID::Main));
    m_gridPtr = grid.get();
    
    // 4 columns * 174 width + 3 * 15 padding = 741 total width
    auto scrollView = std::make_unique<UIScrollView>(760.0f, 618.0f, m_context.atlas);
    scrollView->SetPosition(sf::Vector2f(479.0f, 290.0f));
    scrollView->SetPadding(10.0f, 15.0f);
    scrollView->SetContent(std::move(grid));
    
    auto detailPanel = std::make_unique<UIDetailPanel>(m_context.atlas, m_context.fonts.Get(FontID::Main));
    detailPanel->SetSize(sf::Vector2f(776.0f, 150.0f));
    detailPanel->SetPosition(sf::Vector2f(Core::VIRTUAL_WIDTH / 2.0f - 388.0f, 905.0f));
    detailPanel->SetCornerScale(2.0f);
    m_detailPtr = detailPanel.get();
    
    m_gridPtr->SetOnSelectionChangedCallback([this](const PowerUpData& data) {
        m_detailPtr->UpdateContent(data);
    });

    m_detailPtr->SetOnBuyClicked([this](const std::string& powerUpId) {
        m_context.progressionData.BuyPowerUp(powerUpId, m_context.powerUpData);
        RefreshData();
    });
    
    // Choose the first card by default
    RefreshData();
    m_gridPtr->SelectIndex(0);
    
    m_uiManager.AddElement(std::move(backButton));
    m_uiManager.AddElement(std::move(refundButton));
    m_uiManager.AddElement(std::move(scrollView));
    m_uiManager.AddElement(std::move(detailPanel));
    
    auto goldDisplay = std::make_unique<GoldDisplayWidget>(m_context.atlas, nullptr, m_context.fonts.Get(FontID::Main));
    m_uiManager.AddElement(std::move(goldDisplay));
}

void PowerUpState::HandleInput(sf::Event& event, sf::RenderWindow& window)
{
    // Press Escape to close overlay
    if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
    {
        m_context.stateManager.PopState();
    }
    
    m_uiManager.HandleEvent(event, window);
}

void PowerUpState::Update(float dt)
{
    m_uiManager.Update(dt);
}

void PowerUpState::Draw(sf::RenderWindow& window)
{
    window.draw(m_darkOverlay);
    
    window.draw(m_mainPanelBg);
    window.draw(m_titleText);
    
    m_uiManager.Draw(window);
}

void PowerUpState::RefreshData()
{
    if (!m_gridPtr) return;

    std::vector<PowerUpData> uiData;
    const auto& order = m_context.powerUpData.GetPowerUpOrder();
    
    AssetTextureData emptyBoxData = m_context.atlas.GetTextureData("menu_checkbox_24_bg");
    AssetTextureData filledBoxData = m_context.atlas.GetTextureData("menu_checkbox_24_checkmark");

    for (const std::string& id : order)
    {
        const PowerUpProfile& profile = m_context.powerUpData.GetPowerUpById(id);
        
        PowerUpData item;
        item.id = id;
        item.title = profile.GetName();
        item.description = profile.GetDescription();
        item.textureId = profile.GetFrameName();
        item.maxLevel = profile.GetMaxLevel();
        item.currentLevel = m_context.progressionData.GetPowerUpLevel(id);
        item.price = m_context.progressionData.GetNextPowerUpPrice(id, m_context.powerUpData);

        AssetTextureData iconData = m_context.atlas.GetTextureData(item.textureId);
        if (iconData.texture) item.iconRect = iconData.rect;
        
        if (emptyBoxData.texture) item.emptyBoxRect = emptyBoxData.rect;
        if (filledBoxData.texture) item.filledBoxRect = filledBoxData.rect;

        uiData.push_back(item);
    }
    
    m_gridPtr->SetDataset(uiData);
    
    // Force detail panel to update visually if something is selected
    if (m_detailPtr)
    {
        // Actually, we could just trigger selection change again
    }
}
