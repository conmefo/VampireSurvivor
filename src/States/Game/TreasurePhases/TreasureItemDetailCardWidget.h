#pragma once

#include "TreasureRewardViewContext.h"
#include "UI/Components/NineSliceComponent.h"
#include "Core/Resources/TextureAtlas.h"
#include <SFML/Graphics.hpp>
#include <string>

struct TreasureItemDetailCardConfig
{
    std::string frameTextureKey = "frame1_c2";
    sf::Vector2f baseSize{772.0f, 165.0f};
    float bottomMargin = 3.0f; // 2-3px above bottom of view/panel

    float textOutlineThickness = 0.0f;

    // Top-Left: Level text (White)
    sf::Vector2f levelTextPos{28.0f, 16.0f};
    unsigned int levelFontSize = 22;
    sf::Color levelTextColor = sf::Color::White;

    // Top-Right: Order text "1/3" (Dark Gray)
    sf::Vector2f orderTextPos{748.0f, 16.0f};
    unsigned int orderFontSize = 22;
    sf::Color orderTextColor = sf::Color(65, 65, 65);

    // Middle-Top: Item Name (Gold)
    sf::Vector2f nameTextPos{386.0f, 16.0f};
    unsigned int nameFontSize = 24;
    sf::Color nameTextColor = sf::Color(255, 220, 40);

    // Under Level text: Icon & Frame
    sf::Vector2f iconPos{60.0f, 100.0f};
    float iconSize = 55.0f;
    std::string iconFrameKey = "frameB"; // Icon border frame
    float iconFrameSize = 74.0f;

    // Right of Icon: Item Description (White)
    sf::Vector2f descTextPos{135.0f, 65.0f};
    unsigned int descFontSize = 22;
    sf::Color descTextColor = sf::Color::White;

    // Animation timings
    float growDuration = 0.25f;
    float shrinkDuration = 0.20f;
};

enum class DetailCardAnimState
{
    Hidden,
    Growing,
    Idle,
    Shrinking
};

class TreasureItemDetailCardWidget
{
public:
    TreasureItemDetailCardWidget() = default;

    void Init(TextureAtlas& atlas);
    void SetCardInfo(const ChestRewardItem& item, int cardIndex, int totalCards, TextureAtlas& atlas);
    void StartGrow();
    void StartShrink();

    void Update(float dt);
    void UpdateLayout(const sf::Vector2f& viewSize, const sf::Vector2f& viewCenter, const sf::Vector2f& panelPos, const sf::Vector2f& panelSize);
    void Draw(sf::RenderTarget& target, const sf::Font& font) const;

    bool IsGrowing() const { return m_animState == DetailCardAnimState::Growing; }
    bool IsShrinking() const { return m_animState == DetailCardAnimState::Shrinking; }
    bool IsIdle() const { return m_animState == DetailCardAnimState::Idle; }
    bool IsHidden() const { return m_animState == DetailCardAnimState::Hidden; }

    TreasureItemDetailCardConfig& GetConfig() { return m_config; }
    const TreasureItemDetailCardConfig& GetConfig() const { return m_config; }

private:
    TreasureItemDetailCardConfig m_config;
    NineSliceComponent m_cardBg;
    NineSliceComponent m_iconBgFrame;

    ChestRewardItem m_currentItem;
    int m_cardIndex = 0;
    int m_totalCards = 1;

    DetailCardAnimState m_animState = DetailCardAnimState::Hidden;
    float m_animTimer = 0.0f;
    float m_scaleYProgress = 0.0f;

    AssetTextureData m_iconData;

    sf::Vector2f m_cardPos;
    sf::Vector2f m_cardSize;
    float m_scaleX = 1.0f;
    float m_scaleY = 1.0f;

    mutable sf::Text m_levelText;
    mutable sf::Text m_orderText;
    mutable sf::Text m_nameText;
    mutable sf::Text m_descText;
    mutable sf::Sprite m_iconSprite;
};
