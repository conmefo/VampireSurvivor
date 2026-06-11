#pragma once

#include "../Core/UIElement.h"
#include "../Components/NineSliceComponent.h"
#include <string>
#include <functional>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

// Forward declarations
class TextureAtlas;
class CharacterProfile;
namespace sf { class RenderTarget; class Event; class RenderWindow; class Font; }

enum class CardState
{
    Normal,
    Selected,
    Locked
};

class CharacterCardWidget : public UIElement
{
private:
    std::string m_characterId;
    CardState m_state;
    bool m_isUnlocked;

    NineSliceComponent m_backgroundNormal;
    NineSliceComponent m_backgroundSelected;
    
    sf::Sprite m_characterSprite;
    sf::Sprite m_weaponSprite;
    sf::Text m_nameText;

    std::function<void(const std::string&)> m_onClickCallback;

    bool Contains(const sf::Vector2f& point) const;
    void UpdateVisuals();

public:
    CharacterCardWidget(TextureAtlas& atlas, const sf::Font& font, const CharacterProfile& profile, bool isUnlocked);
    ~CharacterCardWidget() override = default;

    void SetState(CardState state);
    CardState GetState() const;
    const std::string& GetCharacterId() const;

    void SetOnClickCallback(std::function<void(const std::string&)> callback);

    void Update(float deltaTime) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void Draw(sf::RenderTarget& target) override;

    void SetPosition(const sf::Vector2f& pos) override;
    void SetSize(const sf::Vector2f& size) override;
};
