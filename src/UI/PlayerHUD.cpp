#include "PlayerHUD.h"

PlayerHUD::PlayerHUD(const Player& player)
    : m_player(player)
{
    m_bgRect.setSize(sf::Vector2f(BAR_WIDTH, BAR_HEIGHT));
    m_bgRect.setFillColor(sf::Color(0, 0, 0, 180));
    m_bgRect.setOutlineColor(sf::Color(50, 50, 50, 200));
    m_bgRect.setOutlineThickness(1.0f);
    m_bgRect.setOrigin(BAR_WIDTH / 2.0f, BAR_HEIGHT / 2.0f);

    m_fillRect.setSize(sf::Vector2f(BAR_WIDTH - 2.0f, BAR_HEIGHT - 2.0f));
    m_fillRect.setFillColor(sf::Color(220, 20, 20)); // Red
    
    // We don't set origin for fillRect so we can easily position its top-left relative to bgRect's left edge
}

void PlayerHUD::Update(float dt)
{
    if (m_player.IsDead()) return;

    float normalizedHp = 0.0f;
    if (m_player.GetMaxHealth() > 0.0f)
    {
        normalizedHp = m_player.GetCurrentHealth() / m_player.GetMaxHealth();
    }

    if (normalizedHp < 0.0f) normalizedHp = 0.0f;
    if (normalizedHp > 1.0f) normalizedHp = 1.0f;

    // Update the width of the fill rect
    m_fillRect.setSize(sf::Vector2f((BAR_WIDTH - 2.0f) * normalizedHp, BAR_HEIGHT - 2.0f));

    // Update position to follow the player
    sf::Vector2f playerPos = m_player.GetPosition();
    m_bgRect.setPosition(playerPos.x, playerPos.y + Y_OFFSET);
    
    // Position fill rect starting from the left edge of the background
    float leftEdge = playerPos.x - (BAR_WIDTH / 2.0f) + 1.0f; // +1.0f for the border
    float topEdge = playerPos.y + Y_OFFSET - (BAR_HEIGHT / 2.0f) + 1.0f;
    m_fillRect.setPosition(leftEdge, topEdge);
}

void PlayerHUD::Draw(sf::RenderTarget& target) const
{
    if (m_player.IsDead()) return;

    target.draw(m_bgRect);
    target.draw(m_fillRect);
}
