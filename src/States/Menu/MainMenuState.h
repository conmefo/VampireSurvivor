#pragma once

#include "../BaseState.h"
#include "../../UI/UIManager.h"
#include "../../UI/Elements/UIButton.h"
#include <SFML/Graphics.hpp>
#include <vector>

class MainMenuState : public BaseState
{
public:
    explicit MainMenuState(StateContext context);
    ~MainMenuState() override = default;

    void Init() override;
    void HandleInput(sf::Event& event, sf::RenderWindow& window) override;
    void Update(float dt) override;
    void Draw(sf::RenderWindow& window) override;

private:
    void SetupUI();
    void UpdateCursors(float dt);

    UIManager m_uiManager;
    
    // UI Layout references
    std::vector<UIButton*> m_centralCluster;
    
    // Cursor logic
    std::vector<const sf::Texture*> m_cursorFrames;
    sf::Sprite m_leftCursor;
    sf::Sprite m_rightCursor;
    
    float m_cursorAnimTimer = 0.0f;
    int m_currentCursorFrame = 0;
    bool m_cursorsVisible = false;
    
    static constexpr float CURSOR_FRAME_DURATION = 0.1f;
    static constexpr float UI_SCALE = 1.0f;
};
