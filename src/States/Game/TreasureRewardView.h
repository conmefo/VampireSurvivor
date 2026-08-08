#pragma once

#include "../../Core/Resources/TextureAtlas.h"
#include "../../UI/Components/NineSliceComponent.h"
#include "TreasurePhases/ITreasurePhaseState.h"
#include "TreasurePhases/TreasureRewardViewContext.h"
#include "TreasurePhases/TreasurePhase1IdleState.h"
#include "TreasurePhases/TreasurePhase2OpeningState.h"

#include <SFML/Graphics.hpp>
#include <functional>
#include <memory>

class TreasureRewardView
{
public:
    TreasureRewardView(TextureAtlas& atlas, const sf::Font& boldFont);

    void Show(int goldReward, int currentRunGold, int itemCount = 1);
    bool IsVisible() const;
    void CompleteImmediately();

    void SetOnGoldAdded(std::function<void(int)> callback);
    void SetOnComplete(std::function<void()> callback);

    void HandleEvent(const sf::Event& event, const sf::RenderWindow* window = nullptr);
    void Update(float dt);
    void UpdateLayout(const sf::Vector2f& viewSize, const sf::Vector2f& viewCenter);
    void Draw(sf::RenderTarget& target) const;

    void SetState(std::unique_ptr<ITreasurePhaseState> newState);

    TreasureRewardViewContext& GetContext() { return m_context; }
    const TreasureRewardViewContext& GetContext() const { return m_context; }

private:
    void Finish();

    TextureAtlas& m_atlas;
    const sf::Font& m_font;

    sf::RectangleShape m_backdrop;
    NineSliceComponent m_nineSliceBg;

    TreasureRewardViewContext m_context;
    std::unique_ptr<ITreasurePhaseState> m_currentState;

    bool m_visible = false;
    bool m_finishing = false;
};
