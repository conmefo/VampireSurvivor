#pragma once

#include "../BaseState.h"
#include "../../UI/Components/FaderComponent.h"
#include "../StateContext.h"
#include <SFML/Graphics.hpp>

class WarningState : public BaseState
{
public:
    explicit WarningState(StateContext context);
    ~WarningState() override = default;

    void Init() override;
    void HandleInput(sf::Event& event) override;
    void Update(float dt) override;
    void Draw(sf::RenderWindow& window) override;

private:
    void WrapText(sf::Text& text, float maxWidth);
    void UpdateOpacity(float opacity);

    sf::Font m_font;
    sf::Text m_headerText;
    sf::Text m_bodyText1;
    sf::Text m_bodyText2;
    sf::Text m_promptText;

    float m_fadeDurationIn;
    float m_fadeDurationOut;
    float m_elapsedTime;
    float m_opacity;

    enum class FadeState
    {
        FadeIn,
        Visible,
        FadeOut,
        Finished
    };

    FadeState m_fadeState;
    bool m_inputLocked;
};
