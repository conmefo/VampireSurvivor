#pragma once

#include <SFML/Graphics.hpp>

#include "StateContext.h"

class BaseState
{
public:
    BaseState(StateContext context)
        : m_context(context)
    {
    }

    virtual ~BaseState() = default;

    virtual void Init() = 0;
    virtual void HandleInput(sf::Event& event) = 0;
    virtual void Update(float dt) = 0;
    virtual void Draw(sf::RenderWindow& window) = 0;

protected:
    StateContext m_context;
};
