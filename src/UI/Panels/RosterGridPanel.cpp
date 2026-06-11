#include "RosterGridPanel.h"
#include "../Elements/CharacterCardWidget.h"
#include "../../Core/Data/CharacterDataManager.h"
#include "../../Core/Data/PlayerProgressionManager.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

RosterGridPanel::RosterGridPanel(TextureAtlas& atlas, const sf::Font& font)
    : UIPanel(atlas, "frameB9", 10, 10, 10, 10)
    , m_atlas(atlas)
    , m_font(font)
{
}

void RosterGridPanel::InitializeRoster(const CharacterDataManager& dataManager, const PlayerProgressionManager& progManager)
{
    m_cards.clear();

    const auto& characters = dataManager.GetAllCharacters();
    
    // We will just iterate and populate. Since it's a map, order might not be guaranteed, 
    // but the task doesn't specify sorting logic so this is sufficient for the requirement.
    for(const auto& pair : characters)
    {
        const CharacterProfile& profile = pair.second;
        bool isUnlocked = progManager.IsCharacterUnlocked(profile.GetId());

        auto card = std::make_unique<CharacterCardWidget>(m_atlas, m_font, profile, isUnlocked);
        
        // Bind the mediator callback
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
        if(card->GetState() == CardState::Locked)
        {
            continue;
        }

        if(card->GetState() == CardState::Selected)
        {
            // Reset previously selected
            card->SetState(CardState::Normal);
        }
    }

    // Find and set the new selected one
    for(auto& card : m_cards)
    {
        if(card->GetState() != CardState::Locked)
        {
            if(card->GetCharacterId() == characterId)
            {
                card->SetState(CardState::Selected);
            }
        }
    }

    if(m_onSelectionChanged)
    {
        m_onSelectionChanged(m_selectedCharacterId);
    }
}

void RosterGridPanel::SetPosition(const sf::Vector2f& pos)
{
    UIPanel::SetPosition(pos);
    
    // Layout cards
    for(size_t i = 0; i < m_cards.size(); ++i)
    {
        int col = i % MAX_COLUMNS;
        int row = i / MAX_COLUMNS;

        // Assuming fixed card size for layout math. Let's say 120x150.
        float cardWidth = 120.0f;
        float cardHeight = 150.0f;

        float cx = pos.x + GRID_START_OFFSET_X + col * (cardWidth + CARD_SPACING_X);
        float cy = pos.y + GRID_START_OFFSET_Y + row * (cardHeight + CARD_SPACING_Y);

        m_cards[i]->SetPosition(sf::Vector2f(cx, cy));
        m_cards[i]->SetSize(sf::Vector2f(cardWidth, cardHeight));
    }
}

void RosterGridPanel::SetSize(const sf::Vector2f& size)
{
    UIPanel::SetSize(size);
    SetPosition(m_position);
}

void RosterGridPanel::Update(float deltaTime)
{
    UIPanel::Update(deltaTime);
    for(auto& card : m_cards)
    {
        card->Update(deltaTime);
    }
}

void RosterGridPanel::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    UIPanel::HandleEvent(event, window);
    for(auto& card : m_cards)
    {
        card->HandleEvent(event, window);
    }
}

void RosterGridPanel::Draw(sf::RenderTarget& target)
{
    UIPanel::Draw(target);
    for(auto& card : m_cards)
    {
        card->Draw(target);
    }
}
