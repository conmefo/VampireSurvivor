#pragma once

#include "UIPanel.h"
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <string>

// Forward declarations
class TextureAtlas;
class CharacterProfile;
namespace sf { class Font; class RenderTarget; class Event; class RenderWindow; }

class DetailPanel : public UIPanel
{
private:
    TextureAtlas& m_atlas;
    
    sf::Text m_nameText;
    sf::Text m_descriptionText;
    sf::Sprite m_weaponSprite;
    
    bool m_hasProfile;

    static constexpr float TEXT_PADDING_X = 20.0f;
    static constexpr float TEXT_PADDING_Y = 20.0f;
    static constexpr float SPACE_BETWEEN_ELEMENTS = 15.0f;
    static constexpr float WEAPON_OFFSET_Y = 50.0f;

public:
    DetailPanel(TextureAtlas& atlas, const sf::Font& font);
    ~DetailPanel() override = default;

    void SetCharacterProfile(const CharacterProfile& profile);

    void Update(float deltaTime) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void Draw(sf::RenderTarget& target) override;

    void SetPosition(const sf::Vector2f& pos) override;
    void SetSize(const sf::Vector2f& size) override;
};
