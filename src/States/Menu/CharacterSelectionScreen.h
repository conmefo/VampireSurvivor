#pragma once

#include "../BaseState.h"
#include "../../UI/Views/CharacterSelectionView.h"
#include <memory>
#include <SFML/Graphics.hpp>

class CharacterSelectionScreen : public BaseState
{
private:
    std::unique_ptr<CharacterSelectionView> m_view;

public:
    explicit CharacterSelectionScreen(StateContext context);
    ~CharacterSelectionScreen() override = default;

    void Init() override;
    void HandleInput(sf::Event& event, sf::RenderWindow& window) override;
    void Update(float dt) override;
    void Draw(sf::RenderWindow& window) override;

    bool IsOverlay() const override { return true; }
};
