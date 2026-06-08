#pragma once

#include "../BaseState.h"
#include "../StateContext.h"
#include "Core/Animation/SpriteAnimator.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class LoadingState : public BaseState
{
public:
    explicit LoadingState(StateContext context);
    ~LoadingState() override = default;

    void Init() override;
    void HandleInput(sf::Event& event, sf::RenderWindow& window) override;
    void Update(float dt) override;
    void Draw(sf::RenderWindow& window) override;

private:
    sf::Font m_font;
    sf::Text m_textTop;
    sf::Text m_textMiddle;
    sf::Text m_textBottom;

    sf::Sprite m_treasureSprite;
    AnimationData m_treasureAnimData;
    SpriteAnimator m_treasureAnimator;

    std::vector<std::string> m_dlcNames = {
        "Legacy of the Moonspell", 
        "Emergency Meeting", 
        "Operation Gun", 
        "Ode to Castlevania", 
        "Tides of the Foscari", 
        "Emerald Diorama", 
        "Ante Chamber"
    };

    int m_currentDlcIndex;
    int m_currentPercent;
    float m_stepTimer;
    int m_phase; // 0 = Loading DLCs, 1 = Final "Loading" wait

    float m_finalWaitTimer;
};
