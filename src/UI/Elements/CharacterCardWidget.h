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
class WeaponDataManager;
namespace sf { class RenderTarget; class Event; class RenderWindow; class Font; }

enum class CardState
{
    Normal,
    Selected,
    Confirmed,
    Locked
};

class CharacterCardWidget : public UIElement
{
private:
    std::string m_characterId;
    CardState m_state;
    bool m_isUnlocked;
    bool m_isPressed;

    NineSliceComponent m_backgroundNormal;
    NineSliceComponent m_backgroundSelected;
    
    sf::Sprite m_characterSprite;
    sf::Sprite m_weaponSprite;
    sf::Text m_nameText;

    std::function<void(const std::string&)> m_onClickCallback;

    bool Contains(const sf::Vector2f& point) const;
    void UpdateVisuals();

public:
    CharacterCardWidget(TextureAtlas& atlas, const sf::Font& font, const sf::Font* boldFont, const CharacterProfile& profile, bool isUnlocked, const WeaponDataManager* weaponManager = nullptr);
    ~CharacterCardWidget() override = default;

    void SetState(CardState state);
    CardState GetState() const;
    void SetTint(sf::Color color);
    const std::string& GetCharacterId() const;

    void SetOnClickCallback(std::function<void(const std::string&)> callback);

    void Update(float deltaTime) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void Draw(sf::RenderTarget& target) override;

    void SetPosition(const sf::Vector2f& pos) override;
    void SetSize(const sf::Vector2f& size) override;
};
