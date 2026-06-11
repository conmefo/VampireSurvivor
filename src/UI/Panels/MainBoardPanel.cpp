#include "MainBoardPanel.h"
#include "RosterGridPanel.h"
#include "DetailPanel.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

MainBoardPanel::MainBoardPanel(TextureAtlas& atlas, const sf::Font& font, const sf::Font* boldFont)
    : UIPanel(atlas, "frame5_c4", 12, 12, 12, 12)
    , m_scrollView(nullptr)
    , m_rosterGridRaw(nullptr)
    , m_detailPanel(std::make_unique<DetailPanel>(atlas, font))
{
    auto rosterGrid = std::make_unique<RosterGridPanel>(atlas, font, boldFont);
    m_rosterGridRaw = rosterGrid.get();
    m_scrollView = std::make_unique<UIScrollView>(100.0f, 100.0f, atlas);
    m_scrollView->SetContent(std::move(rosterGrid));
    m_titleText.setFont(font);
    m_titleText.setString("Character Selection");
    m_titleText.setCharacterSize(40);
    m_titleText.setStyle(sf::Text::Bold);
}

MainBoardPanel::~MainBoardPanel() = default;

void MainBoardPanel::InitializeLayout(const sf::Vector2f& size)
{
    SetSize(size);

    sf::FloatRect bounds = m_titleText.getLocalBounds();
    m_titleText.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
    m_titleText.setPosition(m_position.x + size.x / 2.0f, m_position.y + PADDING_Y + TITLE_HEIGHT / 2.0f);

    float rosterY = m_position.y + PADDING_Y + TITLE_HEIGHT + INNER_SPACING;
    float rosterHeight = size.y - PADDING_Y * 2.0f - TITLE_HEIGHT - INNER_SPACING * 2.0f - DETAIL_HEIGHT;
    
    if(m_scrollView)
    {
        m_scrollView->SetPosition(sf::Vector2f(m_position.x + PADDING_X, rosterY));
        m_scrollView->SetSize(sf::Vector2f(size.x - PADDING_X * 2.0f - 7.0f, rosterHeight));
        m_scrollView->SetPadding(10.0f, 15.0f);
        
        // Also ensure the inner grid knows its width to layout
        if(m_rosterGridRaw)
        {
            m_rosterGridRaw->SetSize(sf::Vector2f(size.x - PADDING_X * 2.0f - 7.0f, rosterHeight));
        }
    }

    float detailY = rosterY + rosterHeight + INNER_SPACING;
    if(m_detailPanel)
    {
        m_detailPanel->SetPosition(sf::Vector2f(m_position.x + PADDING_X, detailY));
        m_detailPanel->SetSize(sf::Vector2f(size.x - PADDING_X * 2.0f, DETAIL_HEIGHT));
    }
}

void MainBoardPanel::SetPosition(const sf::Vector2f& pos)
{
    UIPanel::SetPosition(pos);
    InitializeLayout(m_size);
}

void MainBoardPanel::Update(float deltaTime)
{
    UIPanel::Update(deltaTime);
    if(m_scrollView)
    {
        m_scrollView->Update(deltaTime);
    }
    if(m_detailPanel)
    {
        m_detailPanel->Update(deltaTime);
    }
}

void MainBoardPanel::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    UIPanel::HandleEvent(event, window);
    if(m_scrollView)
    {
        m_scrollView->HandleEvent(event, window);
    }
    if(m_detailPanel)
    {
        m_detailPanel->HandleEvent(event, window);
    }
}

void MainBoardPanel::Draw(sf::RenderTarget& target)
{
    UIPanel::Draw(target);
    target.draw(m_titleText);
    
    if(m_scrollView)
    {
        m_scrollView->Draw(target);
    }
    if(m_detailPanel)
    {
        m_detailPanel->Draw(target);
    }
}
