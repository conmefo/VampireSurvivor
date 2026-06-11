#include "CharacterCardWidget.h"
#include "../../Core/Data/CharacterProfile.h"
#include "../../Core/Resources/TextureAtlas.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

CharacterCardWidget::CharacterCardWidget(TextureAtlas& atlas, const sf::Font& font, const CharacterProfile& profile, bool isUnlocked)
    : m_characterId(profile.GetId())
    , m_state(CardState::Normal)
    , m_isUnlocked(isUnlocked)
{
    m_backgroundNormal.SetTexture(atlas, "frame5_c4");
    m_backgroundNormal.SetMargins(12, 12, 12, 12);

    m_backgroundSelected.SetTexture(atlas, "frameB9");
    m_backgroundSelected.SetMargins(10, 10, 10, 10);

    m_nameText.setFont(font);
    m_nameText.setString(profile.GetName());
    m_nameText.setCharacterSize(14);
    m_nameText.setFillColor(sf::Color::White);

    m_characterSprite = atlas.CreateSprite(profile.GetPortraitTextureId());
    m_weaponSprite = atlas.CreateSprite(profile.GetStartingWeaponId());

    if(!m_isUnlocked)
    {
        m_state = CardState::Locked;
    }

    UpdateVisuals();
}

void CharacterCardWidget::SetState(CardState state)
{
    if(m_state == state)
    {
        return;
    }
    
    if(m_state == CardState::Locked)
    {
        return;
    }

    m_state = state;
    UpdateVisuals();
}

CardState CharacterCardWidget::GetState() const
{
    return m_state;
}

void CharacterCardWidget::SetOnClickCallback(std::function<void(const std::string&)> callback)
{
    m_onClickCallback = std::move(callback);
}

void CharacterCardWidget::UpdateVisuals()
{
    if(m_state == CardState::Locked)
    {
        m_characterSprite.setColor(sf::Color::Black);
        m_weaponSprite.setColor(sf::Color::Black);
        m_nameText.setFillColor(sf::Color(100, 100, 100));
    }
    else
    {
        m_characterSprite.setColor(sf::Color::White);
        m_weaponSprite.setColor(sf::Color::White);
        m_nameText.setFillColor(sf::Color::White);
    }
}

void CharacterCardWidget::SetPosition(const sf::Vector2f& pos)
{
    UIElement::SetPosition(pos);
    
    m_backgroundNormal.setPosition(pos);
    m_backgroundSelected.setPosition(pos);

    m_nameText.setPosition(pos.x + 5.0f, pos.y + 5.0f);

    m_characterSprite.setPosition(pos.x + m_size.x / 2.0f - m_characterSprite.getGlobalBounds().width / 2.0f, 
                                  pos.y + m_size.y / 2.0f - m_characterSprite.getGlobalBounds().height / 2.0f + 10.0f);

    m_weaponSprite.setPosition(pos.x + m_size.x - m_weaponSprite.getGlobalBounds().width - 5.0f,
                               pos.y + m_size.y - m_weaponSprite.getGlobalBounds().height - 5.0f);
}

void CharacterCardWidget::SetSize(const sf::Vector2f& size)
{
    UIElement::SetSize(size);
    m_backgroundNormal.SetSize(size);
    m_backgroundSelected.SetSize(size);
    SetPosition(m_position);
}

bool CharacterCardWidget::Contains(const sf::Vector2f& point) const
{
    return point.x >= m_position.x && point.x <= m_position.x + m_size.x &&
           point.y >= m_position.y && point.y <= m_position.y + m_size.y;
}

void CharacterCardWidget::Update(float deltaTime)
{
}

void CharacterCardWidget::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        if(Contains(worldPos))
        {
            if(m_state != CardState::Locked && m_onClickCallback)
            {
                m_onClickCallback(m_characterId);
            }
        }
    }
}

void CharacterCardWidget::Draw(sf::RenderTarget& target)
{
    if(m_state == CardState::Selected)
    {
        target.draw(m_backgroundSelected);
    }
    else
    {
        target.draw(m_backgroundNormal);
    }

    target.draw(m_nameText);
    target.draw(m_characterSprite);
    
    if(m_state != CardState::Locked)
    {
        target.draw(m_weaponSprite);
    }
}
