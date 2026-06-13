#include "CharacterSelectionScreen.h"
#include "../Game/GameState.h"
#include "../Game/StageLoadingState.h"
#include "../StateManager.h"
#include "../../Core/WindowSettings.h"
#include <iostream>

CharacterSelectionScreen::CharacterSelectionScreen(StateContext context, TileMapManager& mapManager)
    : BaseState(context), m_mapManager(mapManager)
{
}

void CharacterSelectionScreen::Init()
{
    const sf::Font* font = m_context.fonts.GetPtr(FontID::Main);
    const sf::Font* boldFont = m_context.fonts.GetPtr(FontID::Bold);
    if(font)
    {
        m_view = std::make_unique<CharacterSelectionView>(
            m_context.atlas, 
            *font, 
            boldFont,
            m_context.characterData, 
            &m_context.progressionData,
            &m_context.weaponData,
            &m_context.powerUpData
        );

        m_view->InitializeLayout(sf::Vector2f(Core::VIRTUAL_WIDTH, Core::VIRTUAL_HEIGHT));

        m_view->SetOnBackClicked([this]()
        {
            m_context.stateManager.PopState();
        });

        m_view->SetOnConfirmClicked([this](const std::vector<std::string>& characterIds)
        {
            if (characterIds.empty()) return;
            std::cout << "Starting game with characters: ";
            for (const auto& id : characterIds) std::cout << id << " ";
            std::cout << std::endl;
            
            m_context.stateManager.PopState(); // Pop CharacterSelectionScreen
            m_context.stateManager.PopState(); // Pop MainMenuState
            
            // Hardcode stage 1 for now, as StageSelectionState is future scope
            m_context.stateManager.AddState(std::make_unique<StageLoadingState>(m_context, m_mapManager, characterIds.front(), 1));
        });

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

        m_topBarBg.setSize(sf::Vector2f(Core::VIRTUAL_WIDTH, 99.0f));
        m_topBarBg.setPosition(0.0f, 0.0f);
        m_topBarBg.setFillColor(sf::Color(0, 0, 0, 130));

        SetupCompositeBackground();
    }
}

void CharacterSelectionScreen::HandleInput(sf::Event& event, sf::RenderWindow& window)
{
    if(m_view)
    {
        m_view->HandleEvent(event, window);
    }
}

void CharacterSelectionScreen::Update(float dt)
{
    if(m_view)
    {
        m_view->Update(dt);
    }
}

void CharacterSelectionScreen::Draw(sf::RenderWindow& window)
{
    window.clear(sf::Color::Black);
    window.draw(m_compositeSprite);

    for (const auto& sprite : m_illustrations) {
        window.draw(sprite);
    }

    window.draw(m_topBarBg);

    if(m_view)
    {
        m_view->Draw(window);
    }
}

void CharacterSelectionScreen::SetupCompositeBackground() {
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
