#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <vector>
#include "NineSliceComponent.h"
#include "../../Core/LevelUp/LevelUpOption.h"
#include "../../Core/Resources/TextureAtlas.h"
#include "../../Core/Animation/AnimationData.h"
#include "../../Core/Animation/SpriteAnimator.h"

class LevelUpItemCard : public sf::Drawable, public sf::Transformable
{
public:
    LevelUpItemCard(const TextureAtlas& atlas, const sf::Font& font);
    ~LevelUpItemCard() override = default;

    LevelUpItemCard(LevelUpItemCard&& other) noexcept;
    LevelUpItemCard& operator=(LevelUpItemCard&& other) noexcept;

    LevelUpItemCard(const LevelUpItemCard&) = delete;
    LevelUpItemCard& operator=(const LevelUpItemCard&) = delete;

    void SetOptionData(const LevelUpOption& option, int optionIndex, bool showBanish);
    void SetLayout(const sf::Vector2f& position, float width, float height, float scaleX, float scaleY);

    void Update(float dt, const sf::RenderWindow& window);

    bool HandleMouseEvent(
        const sf::Event& event,
        const sf::RenderWindow& window,
        const std::function<void(int)>& onSelect,
        const std::function<void(int)>& onBanish);

    const LevelUpOption& GetOptionData() const { return m_option; }
    int GetOptionIndex() const { return m_optionIndex; }
    sf::FloatRect GetBounds() const { return m_cardBounds; }

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    const TextureAtlas& m_atlas;
    const sf::Font& m_font;

    NineSliceComponent m_bgNineSlice;
    sf::Sprite m_frameBSprite;
    sf::Sprite m_itemSprite;
    bool m_hasItemTexture = false;

    sf::Text m_titleText;
    sf::Text m_levelTagText;
    sf::Text m_descriptionText;
    sf::Text m_banishBtnText;

    // Spinning hover cursor arrows driven by SpriteAnimator
    AnimationData m_arrowAnimData;
    SpriteAnimator m_arrowAnimator;
    sf::Sprite m_leftArrow;
    sf::Sprite m_rightArrow;

    LevelUpOption m_option;
    int m_optionIndex = 0;
    bool m_showBanish = false;
    bool m_isHovered = false;
    bool m_isPressed = false;
    bool m_pressedInside = false;

    sf::FloatRect m_cardBounds;
    sf::FloatRect m_banishBounds;
};
