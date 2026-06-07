#pragma once

#include "../BaseState.h"
#include <SFML/Graphics.hpp>
#include "../../UI/Components/FaderComponent.h"
#include "../StateContext.h"
#include <vector>

class TitleState : public BaseState
{
public:
    explicit TitleState(StateContext context);
    ~TitleState() override = default;

    void Init() override;
    void HandleInput(sf::Event& event) override;
    void Update(float dt) override;
    void Draw(sf::RenderWindow& window) override;

private:
    static constexpr float INITIAL_PIXEL_SIZE = 64.0f;
    static constexpr float PIXEL_SNAP_THRESHOLD = 1.5f;
    static constexpr float PIXEL_ANIM_DURATION = 2.0f;
    static constexpr float CLARITY_THRESHOLD = 8.0f;
    static constexpr float UI_STAGGER_INTERVAL = 0.2f;
    static constexpr int UI_ITEM_COUNT = 3;
    static constexpr float MAX_ALPHA = 255.0f;

    const sf::Texture* m_bgTexture;
    sf::Sprite m_bgSprite;
    const sf::Texture* m_titleTexture;
    sf::Sprite m_titleSprite;
    sf::RenderTexture m_compositeTexture;
    sf::Sprite m_compositeSprite;
    
    std::vector<const sf::Texture*> m_charTextures;
    std::vector<sf::Sprite> m_charSprites;
    std::vector<FaderComponent> m_charFaders;
    float m_stateTime = 0.0f;

    const sf::Texture* m_promptTexture;
    sf::Sprite m_promptSprite;
    FaderComponent m_promptFader;

    sf::Shader m_pixelateShader;
    float m_currentPixelSize = INITIAL_PIXEL_SIZE;
    
    bool m_isPromptActive = false;
    bool m_inputLocked = true;
    bool m_isTransitioningOut = false;
    bool m_promptFadingIn = true;
    float m_clarityTime = 0.0f;
};
