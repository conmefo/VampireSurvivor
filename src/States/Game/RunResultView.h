#pragma once

#include "../../UI/UIManager.h"

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

class TextureAtlas;
class UIButton;

enum class RunResultOutcome
{
    Completed,
    Defeated
};

class RunResultView
{
public:
    RunResultView(TextureAtlas& atlas, const sf::Font& font, const sf::Font& boldFont);

    void SetResult(RunResultOutcome outcome, const std::string& stageName, int survivalTimeSeconds);
    void SetOnReturnToMenu(std::function<void()> callback);

    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void Update(float dt);
    void Draw(sf::RenderTarget& target);

private:
    void RequestReturnToMenu();

    UIManager m_uiManager;
    sf::RectangleShape m_dimmer;
    sf::RectangleShape m_panel;
    sf::Text m_title;
    sf::Text m_stageName;
    sf::Text m_survivalTime;
    UIButton* m_returnButton = nullptr;
    std::function<void()> m_onReturnToMenu;
    bool m_returnRequested = false;
};
