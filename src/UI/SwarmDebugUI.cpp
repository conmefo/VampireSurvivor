#include "SwarmDebugUI.h"
#include "../States/Game/GameState.h"

namespace
{
constexpr float PanelWidth = 230.0f;
constexpr float ButtonHeight = 24.0f;
constexpr float ButtonGap = 5.0f;
constexpr float Padding = 10.0f;
}

SwarmDebugUI::SwarmDebugUI(TextureAtlas& atlas, const sf::Font& font, GameState& gameState)
    : m_atlas(atlas)
    , m_font(font)
    , m_gameState(gameState)
{
    m_background.setFillColor(sf::Color(18, 22, 32, 230));
    m_background.setOutlineColor(sf::Color(70, 95, 145, 200));
    m_background.setOutlineThickness(1.5f);

    m_titleText.setFont(m_font);
    m_titleText.setString("SWARM CONTROLLER");
    m_titleText.setCharacterSize(13);
    m_titleText.setFillColor(sf::Color(255, 215, 60));
    m_titleText.setStyle(sf::Text::Bold);

    m_shortcutHint.setFont(m_font);
    m_shortcutHint.setString("[F4 to Toggle]");
    m_shortcutHint.setCharacterSize(10);
    m_shortcutHint.setFillColor(sf::Color(160, 180, 210));

    // Add Swarm and Cage Buttons
    AddButton("Bat Swarm (x35)", sf::Color(42, 60, 95), [this]() {
        m_gameState.TriggerSwarmDebug("BAT_SWARM", "BAT1", 35);
    });

    AddButton("Skull Swarm (x35)", sf::Color(50, 52, 80), [this]() {
        m_gameState.TriggerSwarmDebug("SKULL_SWARM", "SKELETON", 35);
    });

    AddButton("Medusa Swarm (x35)", sf::Color(35, 70, 75), [this]() {
        m_gameState.TriggerSwarmDebug("MEDUSA_SWARM", "MEDUSA1", 35);
    });

    AddButton("Ghost Swarm (x35)", sf::Color(55, 42, 85), [this]() {
        m_gameState.TriggerSwarmDebug("GHOST_SWARM", "GHOST", 35);
    });

    AddButton("Dragon Swarm (x35)", sf::Color(85, 38, 45), [this]() {
        m_gameState.TriggerSwarmDebug("DRAGON_SWARM", "XLDRAGON1_FLAG", 35);
    });

    AddButton("Flower Cage Wall (x55)", sf::Color(32, 70, 42), [this]() {
        m_gameState.TriggerSwarmDebug("FLOWER_WALL", "FLOWER", 55);
    });

    AddButton("Medusa Wall (x55)", sf::Color(32, 65, 80), [this]() {
        m_gameState.TriggerSwarmDebug("MEDUSA_WALL", "MEDUSA1", 55);
    });

    AddButton("Boss + Arena Cage (x55)", sf::Color(95, 55, 25), [this]() {
        m_gameState.TriggerSwarmDebug("BOSS_CAGE", "FLOWER", 55);
    });

    AddButton("DEACTIVATE SWARMS", sf::Color(140, 28, 28), [this]() {
        m_gameState.ClearAllSwarmsAndCages();
    });

    LayoutButtons();
}

void SwarmDebugUI::AddButton(const std::string& label, const sf::Color& color, std::function<void()> onClick)
{
    ButtonEntry entry;
    entry.baseColor = color;
    entry.hoverColor = sf::Color(
        std::min(255, color.r + 35),
        std::min(255, color.g + 35),
        std::min(255, color.b + 35));
    entry.onClick = std::move(onClick);

    entry.box.setSize(sf::Vector2f(PanelWidth - (Padding * 2.0f), ButtonHeight));
    entry.box.setFillColor(entry.baseColor);
    entry.box.setOutlineColor(sf::Color(120, 150, 200, 160));
    entry.box.setOutlineThickness(1.0f);

    entry.label.setFont(m_font);
    entry.label.setString(label);
    entry.label.setCharacterSize(11);
    entry.label.setFillColor(sf::Color(235, 240, 250));

    m_buttons.push_back(std::move(entry));
}

void SwarmDebugUI::LayoutButtons()
{
    const float totalHeight = Padding + 24.0f + static_cast<float>(m_buttons.size()) * (ButtonHeight + ButtonGap) + Padding;
    m_background.setPosition(m_position);
    m_background.setSize(sf::Vector2f(PanelWidth, totalHeight));

    m_titleText.setPosition(m_position.x + Padding, m_position.y + Padding - 2.0f);
    m_shortcutHint.setPosition(m_position.x + PanelWidth - Padding - 75.0f, m_position.y + Padding + 1.0f);

    float currentY = m_position.y + Padding + 24.0f;
    for (auto& btn : m_buttons)
    {
        btn.box.setPosition(m_position.x + Padding, currentY);
        btn.label.setPosition(m_position.x + Padding + 8.0f, currentY + 4.0f);
        currentY += ButtonHeight + ButtonGap;
    }
}

void SwarmDebugUI::SetPosition(const sf::Vector2f& pos)
{
    m_position = pos;
    LayoutButtons();
}

void SwarmDebugUI::Update(float deltaTime)
{
    if (!m_visible) return;

    for (auto& btn : m_buttons)
    {
        if (btn.isPressed)
        {
            btn.box.setFillColor(sf::Color(
                std::max(0, btn.baseColor.r - 20),
                std::max(0, btn.baseColor.g - 20),
                std::max(0, btn.baseColor.b - 20)));
        }
        else if (btn.isHovered)
        {
            btn.box.setFillColor(btn.hoverColor);
        }
        else
        {
            btn.box.setFillColor(btn.baseColor);
        }
    }
}

void SwarmDebugUI::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    if (!m_visible) return;

    if (event.type == sf::Event::MouseMoved)
    {
        const sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
        for (auto& btn : m_buttons)
        {
            btn.isHovered = btn.box.getGlobalBounds().contains(mousePos);
        }
    }
    else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        const sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        for (auto& btn : m_buttons)
        {
            if (btn.box.getGlobalBounds().contains(mousePos))
            {
                btn.isPressed = true;
            }
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
    {
        const sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        for (auto& btn : m_buttons)
        {
            if (btn.isPressed && btn.box.getGlobalBounds().contains(mousePos))
            {
                if (btn.onClick)
                {
                    btn.onClick();
                }
            }
            btn.isPressed = false;
        }
    }
}

void SwarmDebugUI::Draw(sf::RenderTarget& target)
{
    if (!m_visible) return;

    target.draw(m_background);
    target.draw(m_titleText);
    target.draw(m_shortcutHint);

    for (const auto& btn : m_buttons)
    {
        target.draw(btn.box);
        target.draw(btn.label);
    }
}
