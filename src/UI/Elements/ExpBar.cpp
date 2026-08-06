#include "ExpBar.h"
#include "../../Entities/Player.h"
#include "../../Core/Resources/TextureAtlas.h"

ExpBar::ExpBar(const TextureAtlas& atlas, const sf::Font& font)
{
    m_background.SetTexture(atlas, "frameB10");
    m_background.SetMargins(5.0f, 5.0f, 5.0f, 5.0f);
    m_background.SetCornerScale(2.0f);

    m_fillRect.setFillColor(sf::Color(54, 103, 207));

    m_levelText.setFont(font);
    m_levelText.setCharacterSize(23);
    m_levelText.setFillColor(sf::Color::White);
    m_levelText.setOutlineThickness(0.7f);
    m_levelText.setOutlineColor(sf::Color::White);

    SetPositionAndSize(m_position, m_size);
}

void ExpBar::SetPositionAndSize(const sf::Vector2f& position, const sf::Vector2f& size)
{
    m_position = position;
    m_size = size;

    m_background.setPosition(m_position);
    m_background.SetSize(m_size);
    m_background.Update();

    // Inner padding offset so the fill fits inside frameB10 background
    float paddingX = 6.0f;
    float paddingY = 8.0f;
    float fillHeight = std::max(0.0f, m_size.y - paddingY * 2.0f);
    float maxFillWidth = std::max(0.0f, m_size.x - paddingX * 2.0f);

    m_fillRect.setPosition(m_position.x + paddingX, m_position.y + paddingY);
    m_fillRect.setSize(sf::Vector2f(maxFillWidth * m_progressRatio, fillHeight));

    // Position text on the right side inside the bar
    sf::FloatRect textBounds = m_levelText.getLocalBounds();
    float rightPadding = 20.0f;
    float textX = m_position.x + m_size.x - textBounds.width - textBounds.left - rightPadding;
    float textY = m_position.y + (m_size.y - textBounds.height) / 2.0f - textBounds.top;
    m_levelText.setPosition(textX, textY);
}

void ExpBar::Update(float dt, const Player* player)
{
    int currentLevel = 1;
    float progressRatio = 0.0f;

    if (player)
    {
        currentLevel = player->GetLevel();
        progressRatio = player->GetExpProgressRatio();
    }

    if (currentLevel != m_cachedLevel)
    {
        m_cachedLevel = currentLevel;
        m_levelText.setString("LV " + std::to_string(m_cachedLevel));

        // Re-align text when content changes
        sf::FloatRect textBounds = m_levelText.getLocalBounds();
        float rightPadding = 20.0f;
        float textX = m_position.x + m_size.x - textBounds.width - textBounds.left - rightPadding;
        float textY = m_position.y + (m_size.y - textBounds.height) / 2.0f - textBounds.top;
        m_levelText.setPosition(textX, textY);
    }

    if (std::abs(progressRatio - m_progressRatio) > 0.0001f)
    {
        m_progressRatio = progressRatio;
        float paddingX = 8.0f;
        float paddingY = 8.0f;
        float maxFillWidth = std::max(0.0f, m_size.x - paddingX * 2.0f);
        float fillHeight = std::max(0.0f, m_size.y - paddingY * 2.0f);
        m_fillRect.setPosition(m_position.x + paddingX, m_position.y + paddingY);
        m_fillRect.setSize(sf::Vector2f(maxFillWidth * m_progressRatio, fillHeight));
    }
}

void ExpBar::Draw(sf::RenderTarget& target) const
{
    target.draw(m_background);
    if (m_progressRatio > 0.0f)
    {
        target.draw(m_fillRect);
    }
    target.draw(m_levelText);
}
