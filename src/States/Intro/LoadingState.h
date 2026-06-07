#pragma once

#include "../BaseState.h"
#include "../StateContext.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class LoadingState : public BaseState
{
public:
    struct AssetPaths
    {
        std::string fontPath = "assets/fonts/arial.ttf";
        std::vector<std::string> chestFramePaths = {
            "assets/textures/chest/frame_0.png",
            "assets/textures/chest/frame_1.png",
            "assets/textures/chest/frame_2.png",
            "assets/textures/chest/frame_3.png"
        };
    };

    struct Config
    {
        float taskSwitchInterval = 0.5f;
        float frameDuration = 0.15f;
        float totalLoadTime = 3.0f;
        std::vector<std::string> loadingTasks = {
            "Loading textures",
            "Parsing configs",
            "Caching sounds",
            "Spawning entities",
            "Initializing engine"
        };
    };

    explicit LoadingState(StateContext context);
    LoadingState(StateContext context, const Config& config, const AssetPaths& assetPaths);
    ~LoadingState() override = default;

    void Init() override;
    void HandleInput(sf::Event& event) override;
    void Update(float dt) override;
    void Draw(sf::RenderWindow& window) override;

private:
    void UpdateText();
    void UpdateChestVisuals();

    Config m_config;
    AssetPaths m_assetPaths;

    sf::Font m_font;
    sf::Text m_text;

    std::vector<sf::Texture> m_chestTextures;
    sf::Sprite m_chestSprite;
    bool m_useTextures;

    sf::RectangleShape m_chestPlaceholder;
    sf::RectangleShape m_progressBarOutline;
    sf::RectangleShape m_progressBarFill;

    float m_elapsedTime;
    float m_taskTimer;
    float m_animTimer;
    float m_progress;

    size_t m_currentTaskIndex;
    int m_currentFrame;
};
