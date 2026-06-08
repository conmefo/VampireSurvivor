#pragma once

#include "../BaseState.h"

class GameState : public BaseState
{
public:
    explicit GameState(StateContext context);
    ~GameState() override = default;

    void Init() override;
    void HandleInput(sf::Event& event) override;
    void Update(float dt) override;
    void Draw(sf::RenderWindow& window) override;

private:
    sf::Font m_font;
    sf::Text m_titleText;
};

