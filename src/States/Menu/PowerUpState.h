#pragma once

#include "../BaseState.h"
#include "../../UI/UIManager.h"
#include "../../UI/Components/PowerUpCard.h"
#include "../../UI/Components/NineSliceComponent.h"
#include <SFML/Graphics/Text.hpp>
#include <memory>

class PowerUpState : public BaseState
{
public:
    PowerUpState(StateContext context);
    
    void Init() override;
    void HandleInput(sf::Event& event, sf::RenderWindow& window) override;
    void Update(float dt) override;
    void Draw(sf::RenderWindow& window) override;
    
    bool IsOverlay() const override { return true; }

private:
    UIManager m_uiManager;
    sf::RectangleShape m_darkOverlay;

    NineSliceComponent m_mainPanelBg;
    sf::Text m_titleText;

    class UIGridLayout* m_gridPtr = nullptr;
    class UIDetailPanel* m_detailPtr = nullptr;

    void RefreshData();
};
