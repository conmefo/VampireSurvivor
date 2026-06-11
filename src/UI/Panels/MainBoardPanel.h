#pragma once

#include "UIPanel.h"
#include <memory>
#include <SFML/Graphics/Text.hpp>
#include "../Components/UIScrollView.h"

// Forward declarations
class TextureAtlas;
class RosterGridPanel;
class DetailPanel;
namespace sf { class Font; class RenderTarget; class Event; class RenderWindow; }

class MainBoardPanel : public UIPanel
{
private:
    std::unique_ptr<UIScrollView> m_scrollView;
    RosterGridPanel* m_rosterGridRaw;
    std::unique_ptr<DetailPanel> m_detailPanel;
    sf::Text m_titleText;

    static constexpr float PADDING_X = 20.0f;
    static constexpr float PADDING_Y = 20.0f;
    static constexpr float TITLE_HEIGHT = 60.0f;
    static constexpr float DETAIL_HEIGHT = 160.0f;
    static constexpr float INNER_SPACING = 10.0f;

public:
    MainBoardPanel(TextureAtlas& atlas, const sf::Font& font, const sf::Font* boldFont = nullptr);
    ~MainBoardPanel() override;

    void InitializeLayout(const sf::Vector2f& size);

    RosterGridPanel* GetRosterGrid() const { return m_rosterGridRaw; }
    DetailPanel* GetDetailPanel() const { return m_detailPanel.get(); }

    void Update(float deltaTime) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void Draw(sf::RenderTarget& target) override;
    void SetPosition(const sf::Vector2f& pos) override;
};
