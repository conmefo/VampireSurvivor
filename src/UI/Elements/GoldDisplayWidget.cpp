#include "GoldDisplayWidget.h"
#include "../../Core/Resources/TextureAtlas.h"
#include "../../Core/Data/PlayerProgressionManager.h"
#include <SFML/Graphics/RenderTarget.hpp>

GoldDisplayWidget::GoldDisplayWidget(TextureAtlas& atlas, const PlayerProgressionManager* progressionManager, const sf::Font& font)
    : UIPanel(atlas, "frameB9", 12.0f, 12.0f, 12.0f, 12.0f)
    , m_progressionManager(progressionManager)
    , m_font(font)
    , m_lastKnownGold(-1)
{
    SetSize(sf::Vector2f(DEFAULT_WIDTH, DEFAULT_HEIGHT));
    SetTextAlignment(TextAlignment::Right);

    AssetTextureData moneyData = atlas.GetTextureData("MoneyPile");
    if(moneyData.texture)
    {
        m_coinIcon.setTexture(*moneyData.texture);
        m_coinIcon.setTextureRect(moneyData.rect);
        m_coinIcon.setOrigin(moneyData.rect.width / 2.0f, moneyData.rect.height / 2.0f);
        m_coinIcon.setScale(2.1f, 2.1f);
    }

    SetPosition(sf::Vector2f(DEFAULT_X, DEFAULT_Y));
}

void GoldDisplayWidget::SetPosition(const sf::Vector2f& pos)
{
    UIPanel::SetPosition(pos);

    m_coinIcon.setPosition(pos.x + 43.5f, pos.y + 32.0f);
}

void GoldDisplayWidget::Update(float deltaTime)
{
    UIPanel::Update(deltaTime);

    if(m_progressionManager)
    {
        int currentGold = m_progressionManager->GetGold();
        
        if(currentGold != m_lastKnownGold)
        {
            m_lastKnownGold = currentGold;
            SetText(std::to_string(m_lastKnownGold), m_font, 28);
        }
    }
}

void GoldDisplayWidget::Draw(sf::RenderTarget& target)
{
    UIPanel::Draw(target);
    target.draw(m_coinIcon);
}
