#pragma once

#include "../Core/UIElement.h"
#include "../Components/NineSliceComponent.h"
#include <string>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

// Forward declarations
class TextureAtlas;
class CharacterProfile;
class WeaponDataManager;
namespace sf { class RenderTarget; class Event; class RenderWindow; class Font; }

class UIPlayerCoopCard : public UIElement
{
private:
    std::string m_playerName;
    const CharacterProfile* m_currentProfile;
    
    NineSliceComponent m_backgroundNormal;
    
    sf::Sprite m_characterSprite;
    sf::Sprite m_weaponSprite;
    sf::Text m_nameText;
    sf::Text m_playerText;
    
    bool m_isSelected;
    
    void UpdateVisuals();

public:
    UIPlayerCoopCard(TextureAtlas& atlas, const sf::Font& font, const sf::Font* boldFont, const std::string& playerName);
    ~UIPlayerCoopCard() override = default;

    void SetCharacterProfile(const CharacterProfile* profile, const WeaponDataManager* weaponManager, TextureAtlas& atlas);
    void SetSelected(bool selected);
    
    void Update(float deltaTime) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void Draw(sf::RenderTarget& target) override;

    void SetPosition(const sf::Vector2f& pos) override;
    void SetSize(const sf::Vector2f& size) override;
};
