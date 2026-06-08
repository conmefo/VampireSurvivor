#pragma once

#include "../BaseState.h"
<<<<<<< Updated upstream
#include "../../UI/Elements/UIButton.h"
#include <memory>
=======
#include "../../UI/UIManager.h"
#include "../../UI/Elements/UIButton.h"
#include <SFML/Graphics.hpp>
#include <vector>
>>>>>>> Stashed changes

class MainMenuState : public BaseState
{
public:
    explicit MainMenuState(StateContext context);
    ~MainMenuState() override = default;

    void Init() override;
<<<<<<< Updated upstream
    void HandleInput(sf::Event& event) override;
=======
    void HandleInput(sf::Event& event, sf::RenderWindow& window) override;
>>>>>>> Stashed changes
    void Update(float dt) override;
    void Draw(sf::RenderWindow& window) override;

private:
<<<<<<< Updated upstream
    void CenterStartText();
    void StartGame();

    sf::Texture m_buttonTexture;
    std::unique_ptr<UIButton> m_startButton;
    sf::Font m_font;
    sf::Text m_startText;
    bool m_startPressed = false;
=======
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
>>>>>>> Stashed changes
};
