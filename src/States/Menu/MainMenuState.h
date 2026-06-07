#pragma once

#include "../BaseState.h"
#include "../StateManager.h"
#include <iostream>

class MainMenuState : public BaseState
{
public:
    MainMenuState(StateContext context)
        : BaseState(context)
    {
    }

    ~MainMenuState() override = default;

    void Init() override
    {
        std::cout << "MainMenuState Init" << std::endl;
    }

    void HandleInput(sf::Event& event) override
    {
    }

    void Update(float dt) override
    {
    }

    void Draw(sf::RenderWindow& window) override
    {
        window.clear(sf::Color(10, 50, 80));
    }
};
