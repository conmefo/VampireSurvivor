#include "StageSelectionScreen.h"
#include "../Game/StageLoadingState.h"
#include "../StateManager.h"
#include "../../Core/WindowSettings.h"
#include <algorithm>
#include <iostream>

StageSelectionScreen::StageSelectionScreen(StateContext context, TileMapManager& mapManager, std::vector<std::string> selectedCharacterIds)
    : BaseState(context)
    , m_mapManager(mapManager)
    , m_selectedCharacterIds(std::move(selectedCharacterIds))
{
}

void StageSelectionScreen::Init()
{
    m_stageWaveData.LoadData("assets/Data/STAGE_DATA.json");
    std::vector<StageInfo> stages = m_stageWaveData.GetOrderedStageInfos();
    const std::vector<std::string> supportedStageKeys = { "FOREST", "LIBRARY", "WAREHOUSE" };
    stages.erase(
        std::remove_if(stages.begin(), stages.end(), [&supportedStageKeys](const StageInfo& stage) {
            return std::find(supportedStageKeys.begin(), supportedStageKeys.end(), stage.stageKey) ==
                supportedStageKeys.end();
        }),
        stages.end());

    const sf::Font* font = m_context.fonts.GetPtr(FontID::Main);
    const sf::Font* boldFont = m_context.fonts.GetPtr(FontID::Bold);
    if (font)
    {
        m_view = std::make_unique<StageSelectionView>(m_context.atlas, *font, boldFont, stages, &m_context.progressionData);
        m_view->InitializeLayout(sf::Vector2f(Core::VIRTUAL_WIDTH, Core::VIRTUAL_HEIGHT));

        m_view->SetOnBackClicked([this]() {
            m_context.stateManager.PopState();
        });

        m_view->SetOnConfirmClicked([this](const StageInfo& stage) {
            std::cout << "Starting stage: " << stage.stageName << " (" << stage.stageKey << ")" << std::endl;

            int stageId = 1;
            if (stage.stageKey == "LIBRARY") stageId = 2;
            else if (stage.stageKey == "WAREHOUSE") stageId = 3;
            else stageId = 1;

            m_context.stateManager.PopState(); // Pop StageSelectionScreen
            m_context.stateManager.PopState(); // Pop CharacterSelectionScreen
            m_context.stateManager.PopState(); // Pop MainMenuState

            m_context.stateManager.AddState(std::make_unique<StageLoadingState>(m_context, m_mapManager, m_selectedCharacterIds, stageId));
        });

        // Composite background matching character selection screen
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

void StageSelectionScreen::HandleInput(sf::Event& event, sf::RenderWindow& window)
{
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
    {
        m_context.stateManager.PopState();
        return;
    }

    if (m_view)
    {
        m_view->HandleEvent(event, window);
    }
}

void StageSelectionScreen::Update(float dt)
{
    if (m_view)
    {
        m_view->Update(dt);
    }
}

void StageSelectionScreen::Draw(sf::RenderWindow& window)
{
    window.clear(sf::Color::Black);
    window.draw(m_compositeSprite);

    for (const auto& sprite : m_illustrations) {
        window.draw(sprite);
    }

    window.draw(m_topBarBg);

    if (m_view)
    {
        m_view->Draw(window);
    }
}

void StageSelectionScreen::SetupCompositeBackground()
{
    m_compositeTexture.create(Core::VIRTUAL_WIDTH, Core::VIRTUAL_HEIGHT);
    m_compositeTexture.clear(sf::Color::Black);

    const sf::Texture* bgTex = m_context.textures.GetPtr("Background");
    if (bgTex) {
        sf::Sprite bgSprite(*bgTex);
        bgSprite.setOrigin(bgTex->getSize().x / 2.0f, bgTex->getSize().y / 2.0f);
        bgSprite.setPosition(Core::VIRTUAL_WIDTH / 2.0f, Core::VIRTUAL_HEIGHT / 2.0f);

        float scaleX = Core::VIRTUAL_WIDTH / static_cast<float>(bgTex->getSize().x);
        float scaleY = Core::VIRTUAL_HEIGHT / static_cast<float>(bgTex->getSize().y);
        float scale = std::max(scaleX, scaleY);
        bgSprite.setScale(scale, scale);

        m_compositeTexture.draw(bgSprite);
    }

    const sf::Texture* titleTex = m_context.textures.GetPtr("Title");
    if (titleTex) {
        sf::Sprite titleSprite(*titleTex);
        titleSprite.setOrigin(titleTex->getSize().x / 2.0f, titleTex->getSize().y / 2.0f);
        titleSprite.setPosition(Core::VIRTUAL_WIDTH / 2.0f, Core::VIRTUAL_HEIGHT / 2.0f - 216.f);
        titleSprite.setScale(1.8f, 1.8f);
        m_compositeTexture.draw(titleSprite);
    }

    m_compositeTexture.display();
    m_compositeSprite.setTexture(m_compositeTexture.getTexture());
}
