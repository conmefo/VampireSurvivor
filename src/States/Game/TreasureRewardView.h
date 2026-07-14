#pragma once

#include "../../Core/Resources/TextureAtlas.h"

#include <SFML/Graphics.hpp>
#include <functional>
#include <random>
#include <vector>

class TreasureRewardView
{
public:
    TreasureRewardView(TextureAtlas& atlas, const sf::Font& boldFont);

    void Show(int goldReward, int currentRunGold);
    bool IsVisible() const;
    void CompleteImmediately();

    void SetOnGoldAdded(std::function<void(int)> callback);
    void SetOnComplete(std::function<void()> callback);

    void HandleEvent(const sf::Event& event);
    void Update(float dt);
    void Draw(sf::RenderTarget& target) const;

private:
    struct CoinParticle
    {
        sf::Vector2f velocity;
        float delay = 0.0f;
        float spinOffset = 0.0f;
        float scale = 1.0f;
    };

    void ConfigureSprite(sf::Sprite& sprite, const AssetTextureData& data) const;
    void RebuildCoinBurst();
    void ApplyPendingGold();
    void Finish();
    float GetRevealProgress() const;

    std::vector<AssetTextureData> m_openFrames;
    std::vector<AssetTextureData> m_frontFrames;
    std::vector<AssetTextureData> m_coinFrames;
    AssetTextureData m_moneyPile{};
    const sf::Font& m_font;

    sf::RectangleShape m_backdrop;
    mutable sf::Sprite m_chest;
    mutable sf::Sprite m_chestFront;
    mutable sf::Sprite m_rewardIcon;
    sf::Text m_rewardLabel;
    sf::Text m_rewardValue;
    sf::Text m_totalValue;
    sf::Text m_continuePrompt;
    std::vector<CoinParticle> m_coins;

    std::function<void(int)> m_onGoldAdded;
    std::function<void()> m_onComplete;
    std::mt19937 m_random{0xC01DC0DEu};
    float m_elapsed = 0.0f;
    int m_goldReward = 0;
    int m_startingGold = 0;
    int m_appliedGold = 0;
    bool m_visible = false;
    bool m_finishing = false;
};
