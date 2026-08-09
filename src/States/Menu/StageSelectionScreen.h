#pragma once

#include "../BaseState.h"
#include "../../UI/Views/StageSelectionView.h"
#include "../../Core/Data/StageWaveDataManager.h"
#include "../../World/TileMapManager.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>

class StageSelectionScreen : public BaseState
{
public:
    StageSelectionScreen(StateContext context, TileMapManager& mapManager, std::vector<std::string> selectedCharacterIds);
    ~StageSelectionScreen() override = default;

    void Init() override;
    void HandleInput(sf::Event& event, sf::RenderWindow& window) override;
    void Update(float dt) override;
    void Draw(sf::RenderWindow& window) override;

private:
    void SetupCompositeBackground();

    TileMapManager& m_mapManager;
    std::vector<std::string> m_selectedCharacterIds;

    StageWaveDataManager m_stageWaveData;
    std::unique_ptr<StageSelectionView> m_view;

    sf::RenderTexture m_compositeTexture;
    sf::Sprite m_compositeSprite;
    std::vector<sf::Sprite> m_illustrations;
    sf::RectangleShape m_topBarBg;
};
