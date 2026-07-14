#pragma once

#include "../../UI/UIManager.h"

#include <SFML/Graphics.hpp>
#include <functional>

class TextureAtlas;
class UIButton;

class GameOverView
{
public:
    GameOverView(TextureAtlas& atlas, const sf::Texture& titleTexture, const sf::Font& boldFont);

    void Show();
    void SetOnQuit(std::function<void()> callback);

    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void Update(float dt);
    void Draw(sf::RenderTarget& target);

private:
    void RequestQuit();
    void UpdateEntranceAnimation();

    UIManager m_uiManager;
    sf::RectangleShape m_crimsonOverlay;
    sf::Sprite m_title;
    UIButton* m_quitButton = nullptr;
    std::function<void()> m_onQuit;
    float m_finalTitleScale = 1.0f;
    float m_entranceElapsed = 0.0f;
    bool m_quitRequested = false;
};
