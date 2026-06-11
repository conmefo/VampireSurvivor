#include "MainMenuState.h"
#include "../../Core/WindowSettings.h"
#include "../Game/GameState.h"
#include "../StateManager.h"
#include "PowerUpState.h"
#include "../../UI/Elements/GoldDisplayWidget.h"
#include <iostream>


MainMenuState::MainMenuState(StateContext context) : BaseState(context) {}

void MainMenuState::Init() {
  std::cout << "MainMenuState Init\n";

  for (int i = 1; i <= 8; ++i) {
    m_cursorFrames.push_back(
        m_context.atlas.GetTextureData("arrow_0" + std::to_string(i)));
  }

  {
    std::vector<AssetTextureData> validFrames;
    for (const auto &data : m_cursorFrames) {
      if (data.texture)
        validFrames.push_back(data);
    }
    m_cursorFrames = std::move(validFrames);
  }

  if (!m_cursorFrames.empty() && m_cursorFrames[0].texture) {
    m_leftCursor.setTexture(*m_cursorFrames[0].texture);
    m_leftCursor.setTextureRect(m_cursorFrames[0].rect);
    m_rightCursor.setTexture(*m_cursorFrames[0].texture);
    m_rightCursor.setTextureRect(m_cursorFrames[0].rect);

    m_leftCursor.setOrigin(m_cursorFrames[0].rect.width / 2.0f,
                           m_cursorFrames[0].rect.height / 2.0f);
    m_rightCursor.setOrigin(m_cursorFrames[0].rect.width / 2.0f,
                            m_cursorFrames[0].rect.height / 2.0f);
    m_leftCursor.setScale(2.0f, 2.0f);
    m_rightCursor.setScale(-2.0f, 2.0f);
  }

  for (int i = 0; i < 3; ++i) {
    sf::Sprite sprite;
    std::string key = "illustrations_" + std::to_string(i);
    AssetTextureData data = m_context.atlas.GetTextureData(key);
    if (data.texture) {
      sprite.setTexture(*data.texture);
      sprite.setTextureRect(data.rect);
      sprite.setOrigin(data.rect.width / 2.0f, data.rect.height / 2.0f);
    }
    
    sf::Color c = sprite.getColor();
    if (i == 1) c.a = 90;
    else if (i == 2) c.a = 150;
    sprite.setColor(c);
    
    m_illustrations.push_back(sprite);
  }

  float textureGap = Core::VIRTUAL_WIDTH / 3.0f;
  if (m_illustrations.size() == 3) {
    m_illustrations[0].setPosition(Core::VIRTUAL_WIDTH / 2.0f, Core::VIRTUAL_HEIGHT / 2.0f + 280.0f);
    m_illustrations[0].setScale(2.5f, 2.5f);
    
    m_illustrations[1].setPosition(Core::VIRTUAL_WIDTH / 2.0f - textureGap, Core::VIRTUAL_HEIGHT / 2.0f + 230.0f);
    m_illustrations[1].setScale(4.9f, 4.9f);
    
    m_illustrations[2].setPosition(Core::VIRTUAL_WIDTH / 2.0f + textureGap, Core::VIRTUAL_HEIGHT / 2.0f);
    m_illustrations[2].setScale(-3.0f, 3.0f);
  }

  SetupCompositeBackground();
  SetupUI();
}

void MainMenuState::SetupUI() {
  float width = Core::VIRTUAL_WIDTH;
  float height = Core::VIRTUAL_HEIGHT;

  sf::Font &font = m_context.fonts.Get(FontID::Main);

  // --- CENTRAL CLUSTER ---
  float startY = height * 0.65f;
  float paddingY = 75.0f;
  float btnWidth = 200.0f * UI_SCALE;
  float btnHeight = 50.0f * UI_SCALE;

  auto createButton = [&](const std::string &normalAsset,
                          const std::string &hoverAsset,
                          const std::string &pressAsset,
                          const std::string &text, float x, float y, float w, float h, int textSize = 24) -> UIButton * {
    auto btn = std::make_unique<UIButton>(m_context.atlas, normalAsset, 10, 10, 10, 10);
    if (!hoverAsset.empty()) btn->SetHoverTexture(hoverAsset);
    if (!pressAsset.empty()) btn->SetPressTexture(pressAsset);
    btn->SetPosition(sf::Vector2f(x - w / 2.0f, y - h / 2.0f));
    btn->SetSize(sf::Vector2f(w, h));
    btn->SetStateColors(sf::Color::White, sf::Color::White, sf::Color::White,
                        sf::Color(100, 100, 100, 150));
    btn->SetText(text, font, textSize);
    btn->SetCornerScale(2.0f);
    UIButton *ptr = btn.get();
    m_uiManager.AddElement(std::move(btn));
    return ptr;
  };

  UIButton *startButton =
      createButton("button_c9_normal", "button_c9_mouseover", "button_c9_pressed", "START", width / 2.0f, startY, btnWidth + 54.0f, btnHeight + 22.0f, 43);
  m_centralCluster.push_back(startButton);
  m_centralCluster.push_back(createButton("button_c9_normal", "button_c9_mouseover", "button_c9_pressed", "ONLINE", width / 2.0f,
                                          startY + paddingY, btnWidth + 27.0f, btnHeight + 5.0f, 36));
  UIButton* powerUpBtn = createButton(
      "button_c5_normal", "button_c5_mouseover", "button_c5_pressed", "POWER UP", width / 2.0f, startY + paddingY * 2, btnWidth, btnHeight + 16.0f, 36);
  m_centralCluster.push_back(powerUpBtn);

  m_centralCluster.push_back(createButton(
      "button_c9_normal", "button_c9_mouseover", "button_c9_pressed", "COLLECTION", width / 2.0f - btnWidth - 43.0f,
      startY + paddingY * 2, btnWidth - 11.0f, btnHeight + 5.0f, 26));
  m_centralCluster.push_back(createButton(
      "button_c9_normal", "button_c9_mouseover", "button_c9_pressed", "UNLOCKS", width / 2.0f + btnWidth + 43.0f,
      startY + paddingY * 2, btnWidth - 11.0f, btnHeight + 5.0f, 26));

  for (auto *btn : m_centralCluster) {
    btn->SetOnClickCallback([]() { std::cout << "Clicked cluster button\n"; });
  }

  startButton->SetOnClickCallback([this]() {
    m_context.stateManager.PopState();
    m_context.stateManager.AddState(std::make_unique<GameState>(m_context));
  });

  powerUpBtn->SetOnClickCallback([this]() {
      m_context.stateManager.AddState(std::make_unique<PowerUpState>(m_context));
  });

  // --- TOP BAR ---
  auto quitBtn = createButton("button_c8_normal", "", "", "QUIT", width * 0.35f, 54.0f, 130.0f, 54.0f, 26);
  quitBtn->SetStateColors(sf::Color::White, sf::Color(255, 255, 255, 200), sf::Color(255, 255, 255, 150), sf::Color(100, 100, 100, 150));
  quitBtn->SetOnClickCallback([this]() { m_context.stateManager.PopState(); });

  auto optionsBtn = createButton("button_c9_normal", "button_c9_mouseover", "button_c9_pressed", "OPTIONS", width * 0.65f, 54.0f, 130.0f, 54.0f, 26);
  optionsBtn->SetOnClickCallback([]() { std::cout << "Options Clicked\n"; });

  // --- BOTTOM BAR ---
  auto creditsBtn =
      createButton("button_c9_normal_mini", "", "", "credits", width / 2.0f, height - 54.0f, 173.0f, 40.0f, 23);
  creditsBtn->SetStateColors(sf::Color::White, sf::Color(255, 255, 255, 200), sf::Color(255, 255, 255, 150), sf::Color(100, 100, 100, 150));
  creditsBtn->SetOnClickCallback([]() { std::cout << "Credits Clicked\n"; });

  // --- TOP BAR BACKGROUND ---
  m_topBarBg.setSize(sf::Vector2f(width, 99.0f));
  m_topBarBg.setPosition(0.0f, 0.0f);
  m_topBarBg.setFillColor(sf::Color(0, 0, 0, 130));

  // --- COIN PANEL ---
  auto coinPanel = std::make_unique<GoldDisplayWidget>(m_context.atlas, nullptr, font);
  m_uiManager.AddElement(std::move(coinPanel));
}

void MainMenuState::HandleInput(sf::Event &event, sf::RenderWindow &window) {
  m_uiManager.HandleEvent(event, window);
}

void MainMenuState::Update(float dt) {
  m_uiManager.Update(dt);
  UpdateCursors(dt);
}

void MainMenuState::UpdateCursors(float dt) {
  m_cursorAnimTimer += dt;
  if (m_cursorFrames.empty()) {
    return; // no cursor frames available
  }

  if (m_cursorAnimTimer >= CURSOR_FRAME_DURATION) {
    m_cursorAnimTimer = 0.0f;
    m_currentCursorFrame = (m_currentCursorFrame + 1) % m_cursorFrames.size();

    // pointer is guaranteed non-null because we filtered in Init
    m_leftCursor.setTexture(*m_cursorFrames[m_currentCursorFrame].texture);
    m_leftCursor.setTextureRect(m_cursorFrames[m_currentCursorFrame].rect);
    m_leftCursor.setOrigin(m_cursorFrames[m_currentCursorFrame].rect.width / 2.0f,
                           m_cursorFrames[m_currentCursorFrame].rect.height / 2.0f);

    m_rightCursor.setTexture(*m_cursorFrames[m_currentCursorFrame].texture);
    m_rightCursor.setTextureRect(m_cursorFrames[m_currentCursorFrame].rect);
    m_rightCursor.setOrigin(m_cursorFrames[m_currentCursorFrame].rect.width / 2.0f,
                            m_cursorFrames[m_currentCursorFrame].rect.height / 2.0f);
  }

  m_cursorsVisible = false;
  for (UIButton *btn : m_centralCluster) {
    if (btn->IsFocused()) {
      m_cursorsVisible = true;
      sf::Vector2f pos = btn->GetPosition();
      sf::Vector2f size = btn->GetSize();

      float padding = 25.0f; // offset cursors outside the button bounds
      m_leftCursor.setPosition(pos.x - padding, pos.y + size.y / 2.0f);
      m_rightCursor.setPosition(pos.x + size.x + padding,
                                pos.y + size.y / 2.0f);
      break;
    }
  }
}

void MainMenuState::Draw(sf::RenderWindow &window) {
  window.clear(sf::Color::Black);
  window.draw(m_compositeSprite);

  for (const auto& sprite : m_illustrations) {
    window.draw(sprite);
  }

  window.draw(m_topBarBg);
  m_uiManager.Draw(window);

  if (m_cursorsVisible) {
    window.draw(m_leftCursor);
    window.draw(m_rightCursor);
  }
}

void MainMenuState::SetupCompositeBackground() {
  m_compositeTexture.create(Core::VIRTUAL_WIDTH, Core::VIRTUAL_HEIGHT);
  m_compositeTexture.clear(sf::Color::Black);

  const sf::Texture *bgTex = m_context.textures.GetPtr("Background");
  if (bgTex) {
    sf::Sprite bgSprite(*bgTex);
    bgSprite.setOrigin(bgTex->getSize().x / 2.0f, bgTex->getSize().y / 2.0f);
    bgSprite.setPosition(Core::VIRTUAL_WIDTH / 2.0f,
                         Core::VIRTUAL_HEIGHT / 2.0f);

    float scaleX = Core::VIRTUAL_WIDTH / static_cast<float>(bgTex->getSize().x);
    float scaleY =
        Core::VIRTUAL_HEIGHT / static_cast<float>(bgTex->getSize().y);
    float scale = std::max(scaleX, scaleY);
    bgSprite.setScale(scale, scale);

    m_compositeTexture.draw(bgSprite);
  }

  const sf::Texture *titleTex = m_context.textures.GetPtr("Title");
  if (titleTex) {
    sf::Sprite titleSprite(*titleTex);
    titleSprite.setOrigin(titleTex->getSize().x / 2.0f,
                          titleTex->getSize().y / 2.0f);
    titleSprite.setPosition(Core::VIRTUAL_WIDTH / 2.0f,
                            Core::VIRTUAL_HEIGHT / 2.0f - 216.f);
    titleSprite.setScale(1.8f, 1.8f);
    m_compositeTexture.draw(titleSprite);
  }

  m_compositeTexture.display();
  m_compositeSprite.setTexture(m_compositeTexture.getTexture());
}
