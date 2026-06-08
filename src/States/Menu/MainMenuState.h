#pragma once

#include "../BaseState.h"
#include "../../UI/Elements/UIButton.h"
#include <memory>

class MainMenuState : public BaseState
{
public:
    MainMenuState(StateContext context)
        : BaseState(context)
    {
    }

    ~MainMenuState() override = default;

    void Init() override;
    void HandleInput(sf::Event& event) override;
    void Update(float dt) override;
    void Draw(sf::RenderWindow& window) override;

private:
    void CenterStartText();
    void StartGame();

    sf::Texture m_buttonTexture;
    std::unique_ptr<UIButton> m_startButton;
    sf::Font m_font;
    sf::Text m_startText;
    bool m_startPressed = false;
};
