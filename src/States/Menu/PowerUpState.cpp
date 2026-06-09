#include "PowerUpState.h"
#include "../../Core/WindowSettings.h"
#include "../StateManager.h"
#include <iostream>
#include "../../UI/Components/UIGridLayout.h"
#include "../../UI/Components/PowerUpCard.h"
#include "../../UI/Elements/UIButton.h"
#include "../../UI/Components/UIScrollView.h"
#include "../../UI/Components/UIDetailPanel.h"
#include "../../Core/Data/PowerUpData.h"
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
    refundButton->SetOnClickCallback([]() {
        std::cout << "Refund PowerUps clicked!\n";
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
    UIGridLayout* gridPtr = grid.get();
    
    AssetTextureData icon1 = m_context.atlas.GetTextureData("MoneyPile");
    AssetTextureData emptyBoxData = m_context.atlas.GetTextureData("menu_checkbox_24_bg");
    AssetTextureData filledBoxData = m_context.atlas.GetTextureData("menu_checkbox_24_checkmark");
    
    std::vector<PowerUpData> dummyData;
    for(int i = 0; i < 15; ++i)
    {
        PowerUpData item;
        item.title = "PowerUp " + std::to_string(i + 1);
        item.currentLevel = i % 5;
        item.maxLevel = 5;
        if(icon1.texture) item.iconRect = icon1.rect;
        if(emptyBoxData.texture) item.emptyBoxRect = emptyBoxData.rect;
        if(filledBoxData.texture) item.filledBoxRect = filledBoxData.rect;
        
        dummyData.push_back(item);
    }
    
    grid->SetDataset(dummyData);
    
    // 4 columns * 174 width + 3 * 15 padding = 741 total width
    auto scrollView = std::make_unique<UIScrollView>(760.0f, 618.0f, m_context.atlas);
    scrollView->SetPosition(sf::Vector2f(479.0f, 290.0f));
    scrollView->SetPadding(10.0f, 15.0f);
    scrollView->SetContent(std::move(grid));
    
    auto detailPanel = std::make_unique<UIDetailPanel>(m_context.atlas, m_context.fonts.Get(FontID::Main));
    detailPanel->SetSize(sf::Vector2f(776.0f, 150.0f));
    detailPanel->SetPosition(sf::Vector2f(Core::VIRTUAL_WIDTH / 2.0f - 388.0f, 905.0f));
    detailPanel->SetCornerScale(2.0f);
    UIDetailPanel* detailPtr = detailPanel.get();
    
    gridPtr->SetOnSelectionChangedCallback([detailPtr](const PowerUpData& data) {
        detailPtr->UpdateContent(data);
    });
    
    // Choose the first card by default
    gridPtr->SelectIndex(0);
    
    m_uiManager.AddElement(std::move(backButton));
    m_uiManager.AddElement(std::move(refundButton));
    m_uiManager.AddElement(std::move(scrollView));
    m_uiManager.AddElement(std::move(detailPanel));
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
