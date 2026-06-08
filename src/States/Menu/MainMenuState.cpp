#include "MainMenuState.h"
#include "../../Core/WindowSettings.h"
#include "../Game/GameState.h"
#include "../StateManager.h"
#include <iostream>


MainMenuState::MainMenuState(StateContext context) : BaseState(context) {}

void MainMenuState::Init() {
  std::cout << "MainMenuState Init\n";

  m_cursorFrames.push_back(m_context.textures.GetPtr("CursorFrame_0"));
  m_cursorFrames.push_back(m_context.textures.GetPtr("CursorFrame_1"));
  m_cursorFrames.push_back(m_context.textures.GetPtr("CursorFrame_2"));
  m_cursorFrames.push_back(m_context.textures.GetPtr("CursorFrame_3"));

  // Filter out any null pointers returned when the asset is only present in the
  // atlas (i.e. not a standalone texture). This prevents later dereference of
  // nullptr.
  {
    std::vector<const sf::Texture *> validFrames;
    for (const sf::Texture *t : m_cursorFrames) {
      if (t)
        validFrames.push_back(t);
    }
    m_cursorFrames = std::move(validFrames);
  }

  if (!m_cursorFrames.empty() && m_cursorFrames[0]) {
    m_leftCursor.setTexture(*m_cursorFrames[0]);
    m_rightCursor.setTexture(*m_cursorFrames[0]);

    sf::Vector2u size = m_cursorFrames[0]->getSize();
    m_leftCursor.setOrigin(size.x / 2.0f, size.y / 2.0f);
    m_rightCursor.setOrigin(size.x / 2.0f, size.y / 2.0f);
    m_rightCursor.setScale(-1.0f, 1.0f);
  }

  SetupUI();
}

void MainMenuState::SetupUI() {
  float width = Core::VIRTUAL_WIDTH;
  float height = Core::VIRTUAL_HEIGHT;

  sf::Font &font = m_context.fonts.Get(FontID::Main);

  // --- CENTRAL CLUSTER ---
  float startY = height * 0.45f;
  float paddingY = 60.0f;
  float btnWidth = 200.0f * UI_SCALE;
  float btnHeight = 50.0f * UI_SCALE;

  auto createButton = [&](const std::string &assetId, const std::string &text,
                          float x, float y, const sf::Color &normal,
                          const sf::Color &hover) -> UIButton * {
    auto btn = std::make_unique<UIButton>(m_context.atlas, assetId, 8, 8, 8, 8);
    btn->SetPosition(sf::Vector2f(x - btnWidth / 2.0f, y - btnHeight / 2.0f));
    btn->SetSize(sf::Vector2f(btnWidth, btnHeight));
    btn->SetStateColors(normal, hover, sf::Color(100, 100, 100),
                        sf::Color(50, 50, 50, 150));
    btn->SetText(text, font, 24);
    UIButton *ptr = btn.get();
    m_uiManager.AddElement(std::move(btn));
    return ptr;
  };

  UIButton *startButton =
      createButton("ButtonBlue", "START", width / 2.0f, startY,
                   sf::Color::White, sf::Color(200, 200, 255));
  m_centralCluster.push_back(startButton);
  m_centralCluster.push_back(createButton("ButtonBlue", "ONLINE", width / 2.0f,
                                          startY + paddingY, sf::Color::White,
                                          sf::Color(200, 200, 255)));
  m_centralCluster.push_back(createButton(
      "ButtonGreen", "POWER UP", width / 2.0f, startY + paddingY * 2,
      sf::Color::White, sf::Color(200, 255, 200)));

  m_centralCluster.push_back(createButton(
      "ButtonBlue", "COLLECTION", width / 2.0f - btnWidth - 20.0f,
      startY + paddingY * 2, sf::Color::White, sf::Color(200, 200, 255)));
  m_centralCluster.push_back(createButton(
      "ButtonBlue", "UNLOCKS", width / 2.0f + btnWidth + 20.0f,
      startY + paddingY * 2, sf::Color::White, sf::Color(200, 200, 255)));

  for (auto *btn : m_centralCluster) {
    btn->SetOnClickCallback([]() { std::cout << "Clicked cluster button\n"; });
  }

  startButton->SetOnClickCallback([this]() {
    m_context.stateManager.PopState();
    m_context.stateManager.AddState(std::make_unique<GameState>(m_context));
  });

  // --- TOP BAR ---
  auto quitBtn = createButton("ButtonRed", "QUIT", width * 0.1f, 50.0f,
                              sf::Color::White, sf::Color(255, 100, 100));
  quitBtn->SetOnClickCallback([this]() { m_context.stateManager.PopState(); });

  auto optionsBtn = createButton("ButtonBlue", "OPTIONS", width * 0.9f, 50.0f,
                                 sf::Color::White, sf::Color(200, 200, 255));
  optionsBtn->SetOnClickCallback([]() { std::cout << "Options Clicked\n"; });

  // --- BOTTOM BAR ---
  auto creditsBtn =
      createButton("ButtonPill", "CREDITS", width / 2.0f, height - 50.0f,
                   sf::Color::White, sf::Color(220, 220, 220));
  creditsBtn->SetOnClickCallback([]() { std::cout << "Credits Clicked\n"; });

  // Set staggering delays on faders if needed here.
  // Currently relying on default fader behavior to just pop in or fade in
  // uniformly.
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
    m_leftCursor.setTexture(*m_cursorFrames[m_currentCursorFrame]);
    m_rightCursor.setTexture(*m_cursorFrames[m_currentCursorFrame]);
  }

  m_cursorsVisible = false;
  for (UIButton *btn : m_centralCluster) {
    if (btn->IsFocused()) {
      m_cursorsVisible = true;
      sf::Vector2f pos = btn->GetPosition();
      sf::Vector2f size = btn->GetSize();

      float padding = 40.0f; // offset cursors outside the button bounds
      m_leftCursor.setPosition(pos.x - padding, pos.y + size.y / 2.0f);
      m_rightCursor.setPosition(pos.x + size.x + padding,
                                pos.y + size.y / 2.0f);
      break;
    }
  }
}

void MainMenuState::Draw(sf::RenderWindow &window) {
  // Background and title are retained implicitly if we don't clear, but state
  // manager might require it. We'll just draw the UI on top. The TitleState was
  // popped though. We can re-draw the title composite here if needed, but for
  // now just clear dark blue.
  window.clear(sf::Color(10, 30, 50));

  m_uiManager.Draw(window);

  if (m_cursorsVisible) {
    window.draw(m_leftCursor);
    window.draw(m_rightCursor);
  }
}
