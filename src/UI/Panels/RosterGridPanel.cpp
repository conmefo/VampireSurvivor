#include "RosterGridPanel.h"
#include "../Elements/CharacterCardWidget.h"
#include "../../Core/Data/CharacterDataManager.h"
#include "../../Core/Data/PlayerProgressionManager.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

RosterGridPanel::RosterGridPanel(TextureAtlas& atlas, const sf::Font& font, const sf::Font* boldFont)
    : m_atlas(atlas)
    , m_font(font)
    , m_boldFont(boldFont)
{
}

void RosterGridPanel::InitializeRoster(const CharacterDataManager& dataManager, const PlayerProgressionManager& progManager, const WeaponDataManager* weaponManager)
{
    m_cards.clear();

    const auto& order = dataManager.GetCharacterOrder();
    
    for(const auto& id : order)
    {
        const CharacterProfile& profile = dataManager.GetCharacterById(id);
        if (profile.IsAlwaysHidden())
        {
            continue;
        }
        
        bool isUnlocked = progManager.IsCharacterUnlocked(id);
        auto card = std::make_unique<CharacterCardWidget>(m_atlas, m_font, m_boldFont, profile, isUnlocked, weaponManager);
        
        card->SetSize(sf::Vector2f(171.0f, 171.0f));
        card->SetOnClickCallback([this](const std::string& characterId)
        {
            this->OnCardClicked(characterId);
        });

        m_cards.push_back(std::move(card));
    }
}

void RosterGridPanel::SetOnSelectionChanged(std::function<void(const std::string&)> callback)
{
    m_onSelectionChanged = std::move(callback);
}

void RosterGridPanel::OnCardClicked(const std::string& characterId)
{
    m_selectedCharacterId = characterId;

    for(auto& card : m_cards)
    {
        if(card->GetState() == CardState::Selected)
        {
            // Reset previously selected
            card->SetState(CardState::Normal);
        }
    }

    // Find and set the new selected one
    for(auto& card : m_cards)
    {
        if(card->GetCharacterId() == characterId)
        {
            card->SetState(CardState::Selected);
        }
    }

    if(m_onSelectionChanged)
    {
        m_onSelectionChanged(m_selectedCharacterId);
    }
}

void RosterGridPanel::SetPosition(const sf::Vector2f& pos)
{
    UIElement::SetPosition(pos);
    
    // Layout cards
    for(size_t i = 0; i < m_cards.size(); ++i)
    {
        int col = i % MAX_COLUMNS;
        int row = i / MAX_COLUMNS;

        // Assuming fixed card size for layout math. Let's say 171x171.
        float cardWidth = 171.0f;
        float cardHeight = 171.0f;

        float cx = pos.x + GRID_START_OFFSET_X + col * (cardWidth + CARD_SPACING_X);
        float cy = pos.y + GRID_START_OFFSET_Y + row * (cardHeight + CARD_SPACING_Y);

        m_cards[i]->SetPosition(sf::Vector2f(cx, cy));
        m_cards[i]->SetSize(sf::Vector2f(cardWidth, cardHeight));
    }

    if(!m_cards.empty())
    {
        int totalRows = (m_cards.size() + MAX_COLUMNS - 1) / MAX_COLUMNS;
        m_size.y = GRID_START_OFFSET_Y + totalRows * (171.0f + CARD_SPACING_Y) + 20.0f; // 20.0f for bottom padding
    }
}

void RosterGridPanel::SetSize(const sf::Vector2f& size)
{
    UIElement::SetSize(size);
    SetPosition(m_position);
}

void RosterGridPanel::Update(float deltaTime)
{
    for(auto& card : m_cards)
    {
        card->Update(deltaTime);
    }
}

void RosterGridPanel::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    for(auto& card : m_cards)
    {
        card->HandleEvent(event, window);
    }
}

void RosterGridPanel::Draw(sf::RenderTarget& target)
{
    for(auto& card : m_cards)
    {
        card->Draw(target);
    }
}
