#include "TreasureRewardView.h"
#include "TreasurePhases/TreasurePhase3RewardState.h"
#include <algorithm>

TreasureRewardView::TreasureRewardView(TextureAtlas& atlas, const sf::Font& boldFont)
    : m_atlas(atlas)
    , m_font(boldFont)
    , m_context{atlas, boldFont, m_nineSliceBg}
{
    m_backdrop.setSize(sf::Vector2f(1920.0f, 1080.0f));
    m_backdrop.setFillColor(sf::Color::Transparent);

    m_nineSliceBg.SetTexture(atlas, "frame5_c4");
    m_nineSliceBg.SetMargins(12.0f, 12.0f, 12.0f, 12.0f);
    m_nineSliceBg.SetCornerScale(1.4f);

    m_context.triggerOpening = [this]() {
        SetState(std::make_unique<TreasurePhase2OpeningState>());
    };

    m_context.triggerReward = [this]() {
        SetState(std::make_unique<TreasurePhase3RewardState>());
    };

    m_context.finish = [this]() {
        Finish();
    };

    UpdateLayout(sf::Vector2f(1920.0f, 1080.0f), sf::Vector2f(960.0f, 540.0f));
}

void TreasureRewardView::Show(int goldReward, int currentRunGold, int itemCount)
{
    m_context.goldReward = std::max(0, goldReward);
    m_context.currentRunGold = std::max(0, currentRunGold);
    m_context.itemCount = std::max(1, itemCount);
    m_visible = true;
    m_finishing = false;

    // Populate sample rewards with authentic names & descriptions from WEAPON_DATA.json / POWERUP_DATA.json
    m_context.rewards.clear();
    struct SampleReward
    {
        std::string iconKey;
        std::string name;
        std::string desc;
    };

    const std::vector<SampleReward> samplePool = {
        {"Whip.png", "Whip", "Attacks horizontally, passes through enemies."},
        {"WandFire.png", "Fire Wand", "Fires at a random enemy, deals high damage."},
        {"Garlic.png", "Garlic", "Damages nearby enemies. Reduces resistance to knockback and freeze."},
        {"HolyWater.png", "Santa Water", "Generates damaging zones and attracts gems."},
        {"LightningRing.png", "Lightning Ring", "Strikes random enemies."}
    };

    for(int i = 0; i < m_context.itemCount; ++i)
    {
        const auto& sample = samplePool[i % samplePool.size()];
        m_context.rewards.push_back({sample.iconKey, sample.name, sample.desc, 5, false});
    }

    UpdateLayout(m_context.viewSize, m_context.viewCenter);
    SetState(std::make_unique<TreasurePhase1IdleState>());
}

bool TreasureRewardView::IsVisible() const
{
    return m_visible;
}

void TreasureRewardView::CompleteImmediately()
{
    Finish();
}

void TreasureRewardView::SetOnGoldAdded(std::function<void(int)> callback)
{
    m_context.onGoldAdded = std::move(callback);
}

void TreasureRewardView::SetOnComplete(std::function<void()> callback)
{
    m_context.onComplete = std::move(callback);
}

void TreasureRewardView::SetState(std::unique_ptr<ITreasurePhaseState> newState)
{
    if(m_currentState)
    {
        m_currentState->OnExit(m_context);
    }
    m_currentState = std::move(newState);
    if(m_currentState)
    {
        m_currentState->OnEnter(m_context);
    }
}

void TreasureRewardView::HandleEvent(const sf::Event& event, const sf::RenderWindow* window)
{
    if(!m_visible || !m_currentState)
    {
        return;
    }
    m_currentState->HandleEvent(event, window, m_context);
}

void TreasureRewardView::Update(float dt)
{
    if(!m_visible || !m_currentState)
    {
        return;
    }
    m_currentState->Update(dt, m_context);
}

void TreasureRewardView::UpdateLayout(const sf::Vector2f& viewSize, const sf::Vector2f& viewCenter)
{
    m_context.viewSize = viewSize;
    m_context.viewCenter = viewCenter;

    m_backdrop.setSize(viewSize);
    m_backdrop.setPosition(viewCenter - viewSize / 2.0f);
    m_backdrop.setFillColor(sf::Color::Transparent);

    if(m_currentState)
    {
        m_currentState->UpdateLayout(viewSize, viewCenter, m_context);
    }
    else
    {
        float scaleX = viewSize.x / 1920.0f;
        float scaleY = viewSize.y / 1080.0f;
        m_context.panelSize = sf::Vector2f(640.0f * scaleX, 860.0f * scaleY);
        m_context.panelPos = viewCenter - m_context.panelSize / 2.0f;
        m_nineSliceBg.SetSize(m_context.panelSize);
        m_nineSliceBg.SetCornerScale(1.4f * scaleY);
        m_nineSliceBg.setPosition(m_context.panelPos);
        m_nineSliceBg.Update();
    }
}

void TreasureRewardView::Draw(sf::RenderTarget& target) const
{
    if(!m_visible)
    {
        return;
    }

    target.draw(m_backdrop);

    if(m_currentState)
    {
        m_currentState->DrawBackground(target, m_context);
    }

    target.draw(m_nineSliceBg);

    if(m_currentState)
    {
        m_currentState->Draw(target, m_context);
    }
}

void TreasureRewardView::Finish()
{
    if(!m_visible || m_finishing)
    {
        return;
    }

    m_finishing = true;
    m_visible = false;
    if(m_context.onComplete)
    {
        m_context.onComplete();
    }
}
