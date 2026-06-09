#pragma once

#include "../BaseState.h"
#include "../StateContext.h"
#include "../../UI/Components/FaderComponent.h"
#include <SFML/Graphics.hpp>
#include <vector>

class TitleState : public BaseState
{
public:
    explicit TitleState(StateContext context);
    ~TitleState() override = default;

    void Init() override;
    void HandleInput(sf::Event& event, sf::RenderWindow& window) override;
    void Update(float dt) override;
    void Draw(sf::RenderWindow& window) override;

private:
    void SetupCompositeBackground();
    void UpdateFaders(float dt);

    sf::RenderTexture m_compositeTexture;
    sf::Sprite m_compositeSprite;

    sf::Shader m_pixelateShader;
    float m_currentPixelSize;
    const float PIXEL_SNAP_THRESHOLD = 15.f;

    bool m_shaderActive;
    bool m_inputLocked;

    // Fading textures
    struct FadeItem
    {
        sf::Sprite sprite;
        FaderComponent fader;
        float delayTimer;
        bool started;
    };

    std::vector<FadeItem> m_illustrations;
    const float STAGGER_INTERVAL = 0.1f;

    // Press to start
    sf::Font m_font;
    sf::Text m_promptText;
    FaderComponent m_promptFader;
    bool m_promptFadingIn;
};
