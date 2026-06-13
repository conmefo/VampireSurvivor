#pragma once

#include "../BaseState.h"
#include "../../UI/Views/CharacterSelectionView.h"
#include "../../World/TileMapManager.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class CharacterSelectionScreen : public BaseState
{
public:
    explicit CharacterSelectionScreen(StateContext context, TileMapManager& mapManager);
    ~CharacterSelectionScreen() override = default;

    void Init() override;
    void HandleInput(sf::Event& event, sf::RenderWindow& window) override;
    void Update(float dt) override;
    void Draw(sf::RenderWindow& window) override;

private:
    void SetupCompositeBackground();

    std::unique_ptr<CharacterSelectionView> m_view;
    TileMapManager& m_mapManager;
    sf::RenderTexture m_compositeTexture;
    sf::Sprite m_compositeSprite;
    std::vector<sf::Sprite> m_illustrations;
    sf::RectangleShape m_topBarBg;
};
