#include "StateManager.h"
#include "BaseState.h"

void StateManager::AddState(std::unique_ptr<BaseState> state)
{
    m_isAdding = true;
    m_newState = std::move(state);
}

void StateManager::PopState()
{
    m_isRemoving = true;
}

void StateManager::ChangeStateWithTransition(std::unique_ptr<BaseState> state, float duration, sf::Color color)
{
    m_newState = std::move(state);
    
    m_transitionManager.StartTransition(duration, color, [this]()
    {
        if(!m_states.empty())
        {
            m_states.pop_back();
        }
        if(m_newState)
        {
            m_states.push_back(std::move(m_newState));
            m_states.back()->Init();
        }
        m_transitionManager.ProceedToFadeIn();
    });
}

void StateManager::ProcessStateChanges()
{
    if(m_isRemoving && !m_states.empty())
    {
        m_states.pop_back();
        m_isRemoving = false;
    }

    if(m_isAdding)
    {
        m_states.push_back(std::move(m_newState));
        m_states.back()->Init();
        m_isAdding = false;
    }
}

void StateManager::HandleInput(sf::Event& event, sf::RenderWindow& window)
{
    if(m_transitionManager.IsTransitioning())
    {
        return;
    }

    if(!m_states.empty())
    {
        m_states.back()->HandleInput(event, window);
    }
}

void StateManager::Update(float dt)
{
    m_transitionManager.Update(dt);

    if(!m_states.empty())
    {
        m_states.back()->Update(dt);
    }
}

void StateManager::Draw(sf::RenderWindow& window)
{
    if(m_states.empty())
    {
        return;
    }

    size_t firstToDraw = m_states.size() - 1;
    while(firstToDraw > 0 && m_states[firstToDraw]->IsOverlay())
    {
        firstToDraw--;
    }

    for(size_t i = firstToDraw; i < m_states.size(); ++i)
    {
        m_states[i]->Draw(window);
    }
    
    m_transitionManager.Draw(window);
}

bool StateManager::IsEmpty() const
{
    return m_states.empty();
}
