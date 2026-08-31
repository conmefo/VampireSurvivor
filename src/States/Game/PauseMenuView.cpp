#include "PauseMenuView.h"

#include "../../Core/Data/PlayerProgressionManager.h"
#include "../../Core/Data/PowerUpDataManager.h"
#include "../../Core/Data/WeaponDataManager.h"
#include "../../Core/Resources/TextureAtlas.h"
#include "../../Core/WindowSettings.h"
#include "../../UI/Elements/UIButton.h"
#include "../../UI/Panels/StatsPanel.h"
#include "../../UI/Panels/UIPanel.h"
#include <algorithm>

namespace
{
void CenterText(sf::Text& text, const sf::Vector2f& position)
{
    const sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
    text.setPosition(position);
}

void FitSprite(sf::Sprite& sprite, float width, float height)
{
    const sf::FloatRect bounds = sprite.getLocalBounds();
    if(bounds.width <= 0.0f || bounds.height <= 0.0f)
    {
        return;
    }

    const float scale = std::min(width / bounds.width, height / bounds.height);
    sprite.setScale(scale, scale);
    sprite.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
}
}

PauseMenuView::PauseMenuView(
    TextureAtlas& atlas,
    const sf::Font& font,
    const sf::Font& boldFont,
    const CharacterProfile& character,
    const WeaponDataManager& weapons,
    const PlayerProgressionManager& progression,
    const PowerUpDataManager& powerUps)
    : m_atlas(atlas)
    , m_font(font)
    , m_boldFont(boldFont)
{
    m_dimmer.setSize(sf::Vector2f(Core::VIRTUAL_WIDTH, Core::VIRTUAL_HEIGHT));
    m_dimmer.setFillColor(sf::Color(3, 5, 12, 218));

    m_title.setFont(m_boldFont);
    m_title.setString("PAUSED");
    m_title.setCharacterSize(58);
    m_title.setFillColor(sf::Color(255, 232, 126));
    m_title.setOutlineColor(sf::Color(50, 20, 5));
    m_title.setOutlineThickness(4.0f);
    CenterText(m_title, sf::Vector2f(Core::VIRTUAL_WIDTH / 2.0f, 72.0f));

    m_pauseIcon = m_atlas.CreateSprite("pause");
    FitSprite(m_pauseIcon, 36.0f, 54.0f);
    m_pauseIcon.setPosition(Core::VIRTUAL_WIDTH / 2.0f - 145.0f, 72.0f);
    m_pauseIcon.setColor(sf::Color(255, 232, 126));

    m_equipmentPanel = std::make_unique<UIPanel>(m_atlas, "frame1_c2", 10, 10, 10, 10);
    m_equipmentPanel->SetPosition(sf::Vector2f(55.0f, 128.0f));
    m_equipmentPanel->SetSize(sf::Vector2f(560.0f, 205.0f));
    m_equipmentPanel->SetCornerScale(1.8f);
    m_equipmentPanel->SetColor(sf::Color(220, 220, 255, 245));

    m_equipmentTitle.setFont(m_boldFont);
    m_equipmentTitle.setString("EQUIPMENT");
    m_equipmentTitle.setCharacterSize(25);
    m_equipmentTitle.setFillColor(sf::Color(255, 224, 112));
    m_equipmentTitle.setPosition(80.0f, 143.0f);

    m_characterName.setFont(m_boldFont);
    m_characterName.setString(character.GetName());
    m_characterName.setCharacterSize(27);
    m_characterName.setFillColor(sf::Color::White);
    m_characterName.setPosition(190.0f, 185.0f);

    m_characterSprite = m_atlas.CreateSprite(character.GetPortraitTextureId());
    FitSprite(m_characterSprite, 92.0f, 92.0f);
    m_characterSprite.setPosition(125.0f, 240.0f);

    const WeaponProfile& weapon = weapons.GetWeaponById(character.GetStartingWeaponId());
    if(weapon.GetId() != "UNKNOWN")
    {
        m_weaponSprite = m_atlas.CreateSprite(
            weapon.GetFrameName().empty() ? character.GetStartingWeaponId() : weapon.GetFrameName());
    }
    FitSprite(m_weaponSprite, 58.0f, 58.0f);
    m_weaponSprite.setPosition(235.0f, 267.0f);

    m_weaponName.setFont(m_font);
    m_weaponName.setString(weapon.GetName() + "  Lv. 1");
    m_weaponName.setCharacterSize(21);
    m_weaponName.setFillColor(sf::Color(230, 230, 230));
    m_weaponName.setPosition(190.0f, 219.0f);

    for(int i = 0; i < 6; ++i)
    {
        sf::Sprite slot = m_atlas.CreateSprite(i == 0 ? "frameB10" : "frameB10_empty");
        FitSprite(slot, 66.0f, 66.0f);
        slot.setPosition(235.0f + static_cast<float>(i) * 66.0f, 267.0f);
        m_emptySlots.push_back(slot);
    }

    m_statsPanel = std::make_unique<StatsPanel>(m_atlas, m_font);
    m_statsPanel->SetPosition(sf::Vector2f(55.0f, 352.0f));
    m_statsPanel->SetSize(sf::Vector2f(560.0f, 670.0f));
    m_statsPanel->SetCharacterProfile(character, &progression, &powerUps);

    m_mapBackground.setPosition(650.0f, 128.0f);
    m_mapBackground.setSize(sf::Vector2f(675.0f, 665.0f));
    m_mapBackground.setFillColor(sf::Color(14, 31, 34, 245));
    m_mapBackground.setOutlineColor(sf::Color(207, 181, 93));
    m_mapBackground.setOutlineThickness(5.0f);

    m_mapGrid.setPosition(690.0f, 220.0f);
    m_mapGrid.setSize(sf::Vector2f(595.0f, 505.0f));
    m_mapGrid.setFillColor(sf::Color(30, 64, 58));
    m_mapGrid.setOutlineColor(sf::Color(103, 143, 105));
    m_mapGrid.setOutlineThickness(3.0f);

    m_mapTitle.setFont(m_boldFont);
    m_mapTitle.setString("MAP");
    m_mapTitle.setCharacterSize(36);
    m_mapTitle.setFillColor(sf::Color(255, 224, 112));
    CenterText(m_mapTitle, sf::Vector2f(987.5f, 168.0f));

    m_stageName.setFont(m_boldFont);
    m_stageName.setCharacterSize(25);
    m_stageName.setFillColor(sf::Color::White);
    CenterText(m_stageName, sf::Vector2f(987.5f, 205.0f));

    m_mapHint.setFont(m_font);
    m_mapHint.setString("Your current position");
    m_mapHint.setCharacterSize(20);
    m_mapHint.setFillColor(sf::Color(185, 210, 185));
    CenterText(m_mapHint, sf::Vector2f(987.5f, 756.0f));

    m_playerMarker.setRadius(12.0f);
    m_playerMarker.setOrigin(12.0f, 12.0f);
    m_playerMarker.setFillColor(sf::Color(255, 222, 72));
    m_playerMarker.setOutlineColor(sf::Color::Black);
    m_playerMarker.setOutlineThickness(4.0f);
    m_playerMarker.setPosition(987.5f, 472.5f);

    m_resumeButton = CreateButton("RESUME", sf::Vector2f(1390.0f, 225.0f), sf::Vector2f(275.0f, 78.0f));
    m_optionsButton = CreateButton("OPTIONS", sf::Vector2f(1390.0f, 325.0f), sf::Vector2f(275.0f, 72.0f));
    m_quitButton = CreateButton("QUIT", sf::Vector2f(1390.0f, 425.0f), sf::Vector2f(275.0f, 72.0f));
    m_mainButtons = {m_resumeButton, m_optionsButton, m_quitButton};

    m_resumeButton->SetOnClickCallback([this]() {
        if(m_onResume) m_onResume();
    });
    m_optionsButton->SetOnClickCallback([this]() { SetOptionsOpen(true); });
    m_quitButton->SetOnClickCallback([this]() {
        if(m_onQuit) m_onQuit();
    });

    m_optionsTitle.setFont(m_boldFont);
    m_optionsTitle.setString("OPTIONS");
    m_optionsTitle.setCharacterSize(40);
    m_optionsTitle.setFillColor(sf::Color(255, 224, 112));
    CenterText(m_optionsTitle, sf::Vector2f(1475.0f, 190.0f));

    m_optionsDescription.setFont(m_font);
    m_optionsDescription.setString("Gameplay display");
    m_optionsDescription.setCharacterSize(22);
    m_optionsDescription.setFillColor(sf::Color(210, 210, 210));
    CenterText(m_optionsDescription, sf::Vector2f(1475.0f, 240.0f));

    m_hitboxButton = CreateButton("HITBOXES: OFF", sf::Vector2f(1360.0f, 290.0f), sf::Vector2f(310.0f, 72.0f));
    m_optionsBackButton = CreateButton("BACK", sf::Vector2f(1360.0f, 390.0f), sf::Vector2f(310.0f, 72.0f));
    m_optionsButtons = {m_hitboxButton, m_optionsBackButton};

    m_hitboxButton->SetOnClickCallback([this]() {
        m_hitboxesVisible = !m_hitboxesVisible;
        UpdateHitboxButtonLabel();
        if(m_onToggleHitboxes) m_onToggleHitboxes();
    });
    m_optionsBackButton->SetOnClickCallback([this]() { SetOptionsOpen(false); });

    SetOptionsOpen(false);
    SetFocusedButton(0);
}

PauseMenuView::~PauseMenuView() = default;

UIButton* PauseMenuView::CreateButton(
    const std::string& label,
    const sf::Vector2f& position,
    const sf::Vector2f& size)
{
    auto button = std::make_unique<UIButton>(
        m_atlas, "button_c9_normal", 10.0f, 10.0f, 10.0f, 10.0f);
    button->SetHoverTexture("button_c9_mouseover");
    button->SetPressTexture("button_c9_pressed");
    button->SetPosition(position);
    button->SetSize(size);
    button->SetText(label, m_boldFont, 30);
    button->SetCornerScale(2.0f);
    UIButton* result = button.get();
    m_uiManager.AddElement(std::move(button));
    return result;
}

void PauseMenuView::SetOnResume(std::function<void()> callback)
{
    m_onResume = std::move(callback);
}

void PauseMenuView::SetOnQuit(std::function<void()> callback)
{
    m_onQuit = std::move(callback);
}

void PauseMenuView::SetOnToggleHitboxes(std::function<void()> callback)
{
    m_onToggleHitboxes = std::move(callback);
}

void PauseMenuView::SetStageName(const std::string& stageName)
{
    m_stageName.setString(stageName);
    CenterText(m_stageName, sf::Vector2f(987.5f, 205.0f));
}

void PauseMenuView::SetPlayerPosition(const sf::Vector2f& position, const sf::Vector2f& worldSize)
{
    if(worldSize.x <= 0.0f || worldSize.y <= 0.0f)
    {
        m_playerMarker.setPosition(987.5f, 472.5f);
        return;
    }

    const float normalizedX = std::clamp(position.x / worldSize.x, 0.0f, 1.0f);
    const float normalizedY = std::clamp(position.y / worldSize.y, 0.0f, 1.0f);
    m_playerMarker.setPosition(
        690.0f + normalizedX * 595.0f,
        220.0f + normalizedY * 505.0f);
}

void PauseMenuView::SetHitboxesVisible(bool visible)
{
    m_hitboxesVisible = visible;
    UpdateHitboxButtonLabel();
}

bool PauseMenuView::IsOptionsOpen() const
{
    return m_optionsOpen;
}

void PauseMenuView::SetOptionsOpen(bool open)
{
    m_optionsOpen = open;
    for(UIButton* button : m_mainButtons)
    {
        button->SetActive(!open);
    }
    for(UIButton* button : m_optionsButtons)
    {
        button->SetActive(open);
    }
    m_focusedIndex = 0;
    SetFocusedButton(0);
}

void PauseMenuView::SetFocusedButton(int index)
{
    std::vector<UIButton*>& buttons = m_optionsOpen ? m_optionsButtons : m_mainButtons;
    if(buttons.empty())
    {
        return;
    }

    m_focusedIndex = (index + static_cast<int>(buttons.size())) % static_cast<int>(buttons.size());
    for(std::size_t i = 0; i < buttons.size(); ++i)
    {
        buttons[i]->SetFocus(static_cast<int>(i) == m_focusedIndex);
    }
}

void PauseMenuView::ActivateFocusedButton()
{
    if(m_optionsOpen)
    {
        if(m_focusedIndex == 0)
        {
            m_hitboxesVisible = !m_hitboxesVisible;
            UpdateHitboxButtonLabel();
            if(m_onToggleHitboxes) m_onToggleHitboxes();
        }
        else
        {
            SetOptionsOpen(false);
        }
        return;
    }

    if(m_focusedIndex == 0)
    {
        if(m_onResume) m_onResume();
    }
    else if(m_focusedIndex == 1)
    {
        SetOptionsOpen(true);
    }
    else if(m_onQuit)
    {
        m_onQuit();
    }
}

void PauseMenuView::UpdateHitboxButtonLabel()
{
    m_hitboxButton->SetText(
        m_hitboxesVisible ? "HITBOXES: ON" : "HITBOXES: OFF",
        m_boldFont,
        28);
}

void PauseMenuView::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    m_uiManager.HandleEvent(event, window);

    if(event.type != sf::Event::KeyPressed)
    {
        return;
    }

    if(event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W)
    {
        SetFocusedButton(m_focusedIndex - 1);
    }
    else if(event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S)
    {
        SetFocusedButton(m_focusedIndex + 1);
    }
    else if(event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Space)
    {
        ActivateFocusedButton();
    }
    else if(event.key.code == sf::Keyboard::Escape && m_optionsOpen)
    {
        SetOptionsOpen(false);
    }
}

void PauseMenuView::Update(float dt)
{
    m_uiManager.Update(dt);
    m_statsPanel->Update(dt);
    m_equipmentPanel->Update(dt);
}

void PauseMenuView::Draw(sf::RenderTarget& target)
{
    target.draw(m_dimmer);
    target.draw(m_pauseIcon);
    target.draw(m_title);

    m_equipmentPanel->Draw(target);
    target.draw(m_equipmentTitle);
    target.draw(m_characterSprite);
    target.draw(m_characterName);
    target.draw(m_weaponName);
    for(const sf::Sprite& slot : m_emptySlots)
    {
        target.draw(slot);
    }
    target.draw(m_weaponSprite);
    m_statsPanel->Draw(target);

    target.draw(m_mapBackground);
    target.draw(m_mapGrid);
    target.draw(m_mapTitle);
    target.draw(m_stageName);
    target.draw(m_mapHint);
    target.draw(m_playerMarker);

    if(m_optionsOpen)
    {
        sf::RectangleShape optionsBackdrop(sf::Vector2f(380.0f, 390.0f));
        optionsBackdrop.setPosition(1285.0f, 130.0f);
        optionsBackdrop.setFillColor(sf::Color(18, 14, 28, 245));
        optionsBackdrop.setOutlineColor(sf::Color(207, 181, 93));
        optionsBackdrop.setOutlineThickness(4.0f);
        target.draw(optionsBackdrop);
        target.draw(m_optionsTitle);
        target.draw(m_optionsDescription);
    }

    m_uiManager.Draw(target);
}
