#pragma once

#include "../BaseState.h"
#include "../../UI/Views/CharacterSelectionView.h"
#include <SFML/Graphics.hpp>
#include <vector>

class CharacterSelectionScreen : public BaseState
{
public:
    explicit CharacterSelectionScreen(StateContext context);
    ~CharacterSelectionScreen() override = default;

    void Init() override;
    void HandleInput(sf::Event& event, sf::RenderWindow& window) override;
    void Update(float dt) override;
    void Draw(sf::RenderWindow& window) override;

private:
    void SetupCompositeBackground();

    std::unique_ptr<CharacterSelectionView> m_view;
    sf::RenderTexture m_compositeTexture;
    sf::Sprite m_compositeSprite;
    std::vector<sf::Sprite> m_illustrations;
    sf::RectangleShape m_topBarBg;
};
