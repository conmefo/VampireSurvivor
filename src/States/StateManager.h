#pragma once

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include "../Core/UI/ScreenTransitionManager.h"

class BaseState;

class StateManager
{
public:
    StateManager() = default;
    ~StateManager() = default;

    void AddState(std::unique_ptr<BaseState> state);
    void PopState();
    void ClearAndQuit();
    void ChangeStateWithTransition(std::unique_ptr<BaseState> state, float duration = 0.5f, sf::Color color = sf::Color::Black);
    void ClearAndSetState(std::unique_ptr<BaseState> state, float duration = 0.35f, sf::Color color = sf::Color::Black);
    void ProcessStateChanges();
    bool IsQuitRequested() const { return m_quitRequested; }

    void HandleInput(sf::Event& event, sf::RenderWindow& window);
    void Update(float dt);
    void Draw(sf::RenderWindow& window);

    bool IsEmpty() const;

private:
    std::vector<std::unique_ptr<BaseState>> m_states;
    std::unique_ptr<BaseState> m_newState;
    bool m_isRemoving = false;
    bool m_isAdding = false;
    bool m_quitRequested = false;

    ScreenTransitionManager m_transitionManager;
};
