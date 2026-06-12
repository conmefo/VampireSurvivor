#include "DetailPanel.h"
#include "../../Core/Data/CharacterProfile.h"
#include "../../Core/Data/WeaponDataManager.h"
#include "../../Core/Resources/TextureAtlas.h"
#include "../Core/TextUtility.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

DetailPanel::DetailPanel(TextureAtlas& atlas, const sf::Font& font, const sf::Font* boldFont)
    : UIPanel(atlas, "frame1_c2", 12, 12, 12, 12)
    , m_atlas(atlas)
    , m_hasProfile(false)
{
    UIPanel::SetCornerScale(2.0f);

    const sf::Font& textFont = boldFont ? *boldFont : font;

    m_nameText.setFont(textFont);
    m_nameText.setCharacterSize(24);
    m_nameText.setFillColor(sf::Color::White);

    m_descriptionText.setFont(textFont);
    m_descriptionText.setCharacterSize(24);
    m_descriptionText.setFillColor(sf::Color(200, 200, 240));

    // Create weapon background using NineSliceComponent
    m_weaponBgSprite.SetTexture(m_atlas, "frameB10");
    m_weaponBgSprite.SetMargins(12.0f, 12.0f, 12.0f, 12.0f);
    m_weaponBgSprite.SetCornerScale(2.5f);
    m_weaponBgSprite.SetSize(sf::Vector2f(80.0f, 80.0f));
}

void DetailPanel::SetCharacterProfile(const CharacterProfile& profile, const WeaponDataManager* weaponManager)
{
    m_hasProfile = true;

    m_nameText.setString(profile.GetName());
    m_rawDescription = profile.GetDescription();
    m_descriptionText.setString(m_rawDescription);

    // Update character sprite
    m_characterSprite = m_atlas.CreateSprite(profile.GetPortraitTextureId());
    sf::FloatRect charBounds = m_characterSprite.getLocalBounds();
    if(charBounds.width > 0 && charBounds.height > 0)
    {
        m_characterSprite.setScale(92.0f / charBounds.width, 92.0f / charBounds.height);
    }

    // Update weapon sprite
    if (weaponManager)
    {
        const WeaponProfile& wp = weaponManager->GetWeaponById(profile.GetStartingWeaponId());
        std::string weaponFrame = wp.GetFrameName();
        m_weaponSprite = m_atlas.CreateSprite(weaponFrame.empty() ? profile.GetStartingWeaponId() : weaponFrame);
    }
    else
    {
        m_weaponSprite = m_atlas.CreateSprite(profile.GetStartingWeaponId());
    }

    sf::FloatRect weaponBounds = m_weaponSprite.getLocalBounds();
    if(weaponBounds.width > 0 && weaponBounds.height > 0)
    {
        m_weaponSprite.setScale(42.0f / weaponBounds.width, 42.0f / weaponBounds.height);
    }

    // Re-layout immediately in case lengths changed
    SetPosition(m_position);
}

void DetailPanel::SetPosition(const sf::Vector2f& pos)
{
    UIPanel::SetPosition(pos);

    m_nameText.setPosition(pos.x + TEXT_PADDING_X, pos.y + TEXT_PADDING_Y);

    float currentY = pos.y + TEXT_PADDING_Y + 20.0f + SPACE_BETWEEN_ELEMENTS;

    // Character sprite on the left
    m_characterSprite.setPosition(pos.x + TEXT_PADDING_X, currentY);

    float characterRightX = pos.x + TEXT_PADDING_X + 92.0f; // character sprite size is 92

    // Weapon background right of character
    float weaponBgX = characterRightX + SPACE_BETWEEN_ELEMENTS;
    m_weaponBgSprite.setPosition(weaponBgX, currentY + 10.0f);

    // Weapon sprite centered on background
    float weaponX = weaponBgX + (80.0f - 42.0f) / 2.0f; // bg size 80, weapon size 42
    float weaponY = currentY + 10.0f + (80.0f - 42.0f) / 2.0f;
    m_weaponSprite.setPosition(weaponX, weaponY);

    // Description text right of panel (or right of weapon bg)
    float descX = weaponBgX + 80.0f + SPACE_BETWEEN_ELEMENTS;
    m_descriptionText.setPosition(descX, currentY);

    // Wrap text based on remaining width
    float maxWidth = m_size.x - (descX - pos.x) - TEXT_PADDING_X;
    if(maxWidth > 50.0f)
    {
        m_descriptionText.setString(m_rawDescription); // Reset to raw before wrapping
        UI::TextUtility::WrapText(m_descriptionText, maxWidth);
    }
}

void DetailPanel::SetSize(const sf::Vector2f& size)
{
    UIPanel::SetSize(size);
    SetPosition(m_position);
}

void DetailPanel::Update(float deltaTime)
{
    UIPanel::Update(deltaTime);
    m_weaponBgSprite.Update();
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
        target.draw(m_characterSprite);
        target.draw(m_weaponBgSprite);
        target.draw(m_weaponSprite);
        target.draw(m_descriptionText);
    }
}
