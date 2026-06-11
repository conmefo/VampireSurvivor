#include "DetailPanel.h"
#include "../../Core/Data/CharacterProfile.h"
#include "../../Core/Resources/TextureAtlas.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

DetailPanel::DetailPanel(TextureAtlas& atlas, const sf::Font& font)
    : UIPanel(atlas, "frame1_c2", 12, 12, 12, 12)
    , m_atlas(atlas)
    , m_hasProfile(false)
{
    UIPanel::SetCornerScale(2.0f);

    m_nameText.setFont(font);
    m_nameText.setCharacterSize(24);
    m_nameText.setFillColor(sf::Color::White);

    m_descriptionText.setFont(font);
    m_descriptionText.setCharacterSize(14);
    m_descriptionText.setFillColor(sf::Color(200, 200, 200));
}

void DetailPanel::SetCharacterProfile(const CharacterProfile& profile)
{
    m_hasProfile = true;

    m_nameText.setString(profile.GetName());
    m_descriptionText.setString(profile.GetDescription());

    // Update the sprite
    m_weaponSprite = m_atlas.CreateSprite(profile.GetStartingWeaponId());

    // Re-layout immediately in case lengths changed
    SetPosition(m_position);
}

void DetailPanel::SetPosition(const sf::Vector2f& pos)
{
    UIPanel::SetPosition(pos);

    m_nameText.setPosition(pos.x + TEXT_PADDING_X, pos.y + TEXT_PADDING_Y);

    float currentY = pos.y + TEXT_PADDING_Y + m_nameText.getGlobalBounds().height + SPACE_BETWEEN_ELEMENTS;

    m_weaponSprite.setPosition(pos.x + TEXT_PADDING_X, currentY);

    // Place description below or next to weapon. Let's place it next to weapon.
    float descX = pos.x + TEXT_PADDING_X + m_weaponSprite.getGlobalBounds().width + SPACE_BETWEEN_ELEMENTS;
    m_descriptionText.setPosition(descX, currentY);
}

void DetailPanel::SetSize(const sf::Vector2f& size)
{
    UIPanel::SetSize(size);
    SetPosition(m_position);
}

void DetailPanel::Update(float deltaTime)
{
    UIPanel::Update(deltaTime);
}

void DetailPanel::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    UIPanel::HandleEvent(event, window);
}

void DetailPanel::Draw(sf::RenderTarget& target)
{
    UIPanel::Draw(target);

    if(m_hasProfile)
    {
        target.draw(m_nameText);
        target.draw(m_weaponSprite);
        target.draw(m_descriptionText);
    }
}
