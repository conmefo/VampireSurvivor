#include "CharacterCardWidget.h"
#include "../../Core/Data/CharacterProfile.h"
#include "../../Core/Data/WeaponDataManager.h"
#include "../../Core/Resources/TextureAtlas.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

CharacterCardWidget::CharacterCardWidget(TextureAtlas& atlas, const sf::Font& font, const sf::Font* boldFont, const CharacterProfile& profile, bool isUnlocked, const WeaponDataManager* weaponManager)
    : m_characterId(profile.GetId())
    , m_state(CardState::Normal)
    , m_isUnlocked(isUnlocked)
    , m_isPressed(false)
{
    m_backgroundNormal.SetTexture(atlas, "frame1_c2");
    m_backgroundNormal.SetMargins(12, 12, 12, 12);
    
    m_backgroundSelected.SetTexture(atlas, "selectionSquare_03");
    m_backgroundSelected.SetMargins(12, 12, 12, 12);
    m_backgroundSelected.SetCornerScale(2.0f);
    m_backgroundSelected.SetSize({190.0f, 190.0f});

    if (boldFont)
    {
        m_nameText.setFont(*boldFont);
    }
    else
    {
        m_nameText.setFont(font);
    }

    std::string fullName = profile.GetName();
    size_t spacePos = fullName.find(' ');
    std::string shortName = (spacePos != std::string::npos) ? fullName.substr(0, spacePos) : fullName;
    m_nameText.setString(shortName);

    m_nameText.setCharacterSize(21);
    m_nameText.setFillColor(sf::Color::White);

    m_characterSprite = atlas.CreateSprite(profile.GetPortraitTextureId());
    
    sf::FloatRect charBounds = m_characterSprite.getLocalBounds();
    if(charBounds.width > 0 && charBounds.height > 0)
    {
        m_characterSprite.setScale(92.0f / charBounds.width, 92.0f / charBounds.height);
    }
    
    if (weaponManager)
    {
        const WeaponProfile& wp = weaponManager->GetWeaponById(profile.GetStartingWeaponId());
        std::string weaponFrame = wp.GetFrameName();
        m_weaponSprite = atlas.CreateSprite(weaponFrame.empty() ? profile.GetStartingWeaponId() : weaponFrame);
    }
    else
    {
        m_weaponSprite = atlas.CreateSprite(profile.GetStartingWeaponId());
    }

    sf::FloatRect weaponBounds = m_weaponSprite.getLocalBounds();
    if(weaponBounds.width > 0 && weaponBounds.height > 0)
    {
        m_weaponSprite.setScale(42.0f / weaponBounds.width, 42.0f / weaponBounds.height);
    }

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

    m_state = state;
    UpdateVisuals();
}

CardState CharacterCardWidget::GetState() const
{
    return m_state;
}

const std::string& CharacterCardWidget::GetCharacterId() const
{
    return m_characterId;
}

void CharacterCardWidget::SetOnClickCallback(std::function<void(const std::string&)> callback)
{
    m_onClickCallback = std::move(callback);
}

void CharacterCardWidget::UpdateVisuals()
{
    sf::Color tint = m_isPressed ? sf::Color(150, 150, 150) : sf::Color::White;
    m_backgroundNormal.SetColor(tint);
    m_backgroundSelected.SetColor(tint);

    if(!m_isUnlocked)
    {
        m_characterSprite.setColor(sf::Color::Black);
        m_weaponSprite.setColor(tint);
        m_nameText.setFillColor(sf::Color(40, 40, 40));
    }
    else
    {
        m_characterSprite.setColor(tint);
        m_weaponSprite.setColor(tint);
        m_nameText.setFillColor(m_isPressed ? sf::Color(200, 200, 200) : sf::Color::White);
    }
}

void CharacterCardWidget::SetPosition(const sf::Vector2f& pos)
{
    UIElement::SetPosition(pos);
    
    m_backgroundNormal.setPosition(pos);
    
    float selOffsetX = (190.0f - m_size.x) / 2.0f;
    float selOffsetY = (190.0f - m_size.y) / 2.0f;
    m_backgroundSelected.setPosition(sf::Vector2f(pos.x - selOffsetX, pos.y - selOffsetY));

    m_nameText.setOrigin(0.0f, 0.0f);
    m_nameText.setPosition(pos.x + 10.0f, pos.y + 10.0f);

    float spacing = 15.0f;
    float charW = m_characterSprite.getGlobalBounds().width;
    float weapW = m_weaponSprite.getGlobalBounds().width;
    float totalW = charW + spacing + weapW;
    
    float startX = pos.x + (m_size.x - totalW) / 2.0f;
    float centerY = pos.y + m_size.y / 2.0f + 30.0f;

    m_characterSprite.setPosition(startX, centerY - m_characterSprite.getGlobalBounds().height / 2.0f);
    m_weaponSprite.setPosition(startX + charW + spacing, centerY - m_weaponSprite.getGlobalBounds().height / 2.0f + 15.0f);
}

void CharacterCardWidget::SetSize(const sf::Vector2f& size)
{
    UIElement::SetSize(size);
    m_backgroundNormal.SetSize(size);
    
    m_backgroundSelected.SetSize(sf::Vector2f(190.0f, 190.0f));
    m_backgroundSelected.SetCornerScale(2.0f);
    
    SetPosition(m_position);
}

bool CharacterCardWidget::Contains(const sf::Vector2f& point) const
{
    return point.x >= m_position.x && point.x <= m_position.x + m_size.x &&
           point.y >= m_position.y && point.y <= m_position.y + m_size.y;
}

void CharacterCardWidget::Update(float deltaTime)
{
    m_backgroundNormal.Update();
    m_backgroundSelected.Update();
}

void CharacterCardWidget::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f worldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        if(Contains(worldPos))
        {
            m_isPressed = true;
            UpdateVisuals();
        }
    }
    else if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
    {
        if(m_isPressed)
        {
            m_isPressed = false;
            UpdateVisuals();
            
            sf::Vector2f worldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
            if(Contains(worldPos))
            {
                if(m_onClickCallback)
                {
                    m_onClickCallback(m_characterId);
                }
            }
        }
    }
}

void CharacterCardWidget::Draw(sf::RenderTarget& target)
{
    target.draw(m_backgroundNormal);

    if(m_state == CardState::Selected)
    {
        target.draw(m_backgroundSelected);
    }

    target.draw(m_nameText);
    target.draw(m_characterSprite);
    
    // Draw Weapon Shadow
    sf::Vector2f origPos = m_weaponSprite.getPosition();
    sf::Color origColor = m_weaponSprite.getColor();
    
    m_weaponSprite.setColor(sf::Color(0, 0, 0, 180));
    m_weaponSprite.setPosition(origPos.x + 4.0f, origPos.y + 4.0f);
    target.draw(m_weaponSprite);
    
    // Draw Actual Weapon
    m_weaponSprite.setColor(origColor);
    m_weaponSprite.setPosition(origPos);
    target.draw(m_weaponSprite);
}
