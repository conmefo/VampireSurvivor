#include "TreasureRewardView.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

namespace
{
constexpr float ViewWidth = 1920.0f;
constexpr float ViewHeight = 1080.0f;
constexpr float Pi = 3.14159265358979323846f;
constexpr float ChestCenterX = ViewWidth * 0.5f;
constexpr float ChestCenterY = 675.0f;
constexpr float OpeningStart = 0.35f;
constexpr float OpeningFrameDuration = 0.075f;
constexpr float BurstStart = 0.55f;
constexpr float RewardRevealStart = 1.35f;
constexpr float GoldCountDuration = 1.65f;
constexpr float ContinueStart = 3.15f;
constexpr float AutoCloseTime = 5.25f;
constexpr std::size_t CoinCount = 72;

float Clamp01(float value)
{
    return std::clamp(value, 0.0f, 1.0f);
}

float SmoothStep(float value)
{
    value = Clamp01(value);
    return value * value * (3.0f - 2.0f * value);
}

void CenterText(sf::Text& text, float x, float y)
{
    const sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(
        std::floor(bounds.left + bounds.width * 0.5f),
        std::floor(bounds.top + bounds.height * 0.5f));
    text.setPosition(std::floor(x), std::floor(y));
}

std::string FormatGold(int value)
{
    std::string digits = std::to_string(std::max(0, value));
    for(int index = static_cast<int>(digits.size()) - 3; index > 0; index -= 3)
    {
        digits.insert(static_cast<std::size_t>(index), ",");
    }
    return digits;
}
}

TreasureRewardView::TreasureRewardView(TextureAtlas& atlas, const sf::Font& boldFont)
    : m_font(boldFont)
{
    for(int frameIndex = 1; frameIndex <= 8; ++frameIndex)
    {
        const std::string suffix = frameIndex < 10 ? "0" + std::to_string(frameIndex)
                                                   : std::to_string(frameIndex);
        const AssetTextureData openFrame = atlas.GetTextureData("TreasureOpen_" + suffix);
        const AssetTextureData frontFrame = atlas.GetTextureData("TreasureOpenFront_" + suffix);
        if(openFrame.texture)
        {
            m_openFrames.push_back(openFrame);
        }
        if(frontFrame.texture)
        {
            m_frontFrames.push_back(frontFrame);
        }
    }

    for(int frameIndex = 1; frameIndex <= 5; ++frameIndex)
    {
        const std::string suffix = "0" + std::to_string(frameIndex);
        const AssetTextureData coinFrame = atlas.GetTextureData("coin-spin-gold_" + suffix);
        if(coinFrame.texture)
        {
            m_coinFrames.push_back(coinFrame);
        }
    }
    m_moneyPile = atlas.GetTextureData("MoneyPile");

    m_backdrop.setSize(sf::Vector2f(ViewWidth, ViewHeight));
    m_backdrop.setFillColor(sf::Color(5, 0, 12, 225));

    m_rewardLabel.setFont(m_font);
    m_rewardLabel.setString("GOLD");
    m_rewardLabel.setCharacterSize(48);
    m_rewardLabel.setStyle(sf::Text::Bold);
    m_rewardLabel.setFillColor(sf::Color(255, 225, 82));
    m_rewardLabel.setOutlineColor(sf::Color(98, 25, 3));
    m_rewardLabel.setOutlineThickness(4.0f);

    m_rewardValue = m_rewardLabel;
    m_rewardValue.setCharacterSize(72);

    m_totalValue.setFont(m_font);
    m_totalValue.setCharacterSize(30);
    m_totalValue.setFillColor(sf::Color(255, 246, 193));
    m_totalValue.setOutlineColor(sf::Color::Black);
    m_totalValue.setOutlineThickness(3.0f);

    m_continuePrompt.setFont(m_font);
    m_continuePrompt.setString("PRESS ENTER OR CLICK TO CONTINUE");
    m_continuePrompt.setCharacterSize(25);
    m_continuePrompt.setFillColor(sf::Color(255, 236, 160));
    m_continuePrompt.setOutlineColor(sf::Color::Black);
    m_continuePrompt.setOutlineThickness(2.0f);

    RebuildCoinBurst();
}

void TreasureRewardView::Show(int goldReward, int currentRunGold)
{
    m_goldReward = std::max(0, goldReward);
    m_startingGold = std::max(0, currentRunGold);
    m_appliedGold = 0;
    m_elapsed = 0.0f;
    m_visible = true;
    m_finishing = false;
    m_random.seed(static_cast<std::mt19937::result_type>(0xC01DC0DEu + m_goldReward));
    RebuildCoinBurst();

    m_rewardValue.setString("+0");
    m_totalValue.setString("TOTAL  " + FormatGold(m_startingGold));
    CenterText(m_rewardLabel, ChestCenterX, 235.0f);
    CenterText(m_rewardValue, ChestCenterX, 335.0f);
    CenterText(m_totalValue, ChestCenterX, 430.0f);
    CenterText(m_continuePrompt, ChestCenterX, 955.0f);
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
    m_onGoldAdded = std::move(callback);
}

void TreasureRewardView::SetOnComplete(std::function<void()> callback)
{
    m_onComplete = std::move(callback);
}

void TreasureRewardView::HandleEvent(const sf::Event& event)
{
    if(!m_visible)
    {
        return;
    }

    const bool activated =
        (event.type == sf::Event::KeyPressed &&
         (event.key.code == sf::Keyboard::Enter ||
          event.key.code == sf::Keyboard::Space ||
          event.key.code == sf::Keyboard::Escape)) ||
        (event.type == sf::Event::MouseButtonPressed &&
         event.mouseButton.button == sf::Mouse::Left);
    if(!activated)
    {
        return;
    }

    if(m_elapsed < ContinueStart)
    {
        m_elapsed = ContinueStart;
        ApplyPendingGold();
    }
    else
    {
        Finish();
    }
}

void TreasureRewardView::Update(float dt)
{
    if(!m_visible)
    {
        return;
    }

    m_elapsed += std::max(0.0f, dt);
    ApplyPendingGold();
    if(m_elapsed >= AutoCloseTime)
    {
        Finish();
    }
}

void TreasureRewardView::Draw(sf::RenderTarget& target) const
{
    if(!m_visible)
    {
        return;
    }

    target.draw(m_backdrop);

    const float reveal = GetRevealProgress();
    const float burst = SmoothStep((m_elapsed - BurstStart) / 0.45f);

    sf::ConvexShape rewardReel(4);
    rewardReel.setPoint(0, sf::Vector2f(ChestCenterX - 67.0f, 70.0f));
    rewardReel.setPoint(1, sf::Vector2f(ChestCenterX + 67.0f, 70.0f));
    rewardReel.setPoint(2, sf::Vector2f(ChestCenterX + 145.0f, ChestCenterY));
    rewardReel.setPoint(3, sf::Vector2f(ChestCenterX - 145.0f, ChestCenterY));
    rewardReel.setFillColor(sf::Color(65, 34, 168, static_cast<sf::Uint8>(165.0f * burst)));
    target.draw(rewardReel, sf::BlendAdd);

    if(!m_coinFrames.empty())
    {
        const float reelScroll = std::fmod(std::max(0.0f, m_elapsed - BurstStart) * 125.0f, 110.0f);
        for(int iconIndex = -1; iconIndex < 6; ++iconIndex)
        {
            sf::Sprite reelCoin;
            const std::size_t frameIndex = static_cast<std::size_t>(iconIndex + 10) % m_coinFrames.size();
            ConfigureSprite(reelCoin, m_coinFrames[frameIndex]);
            reelCoin.setPosition(ChestCenterX, 115.0f + iconIndex * 110.0f + reelScroll);
            reelCoin.setScale(3.15f, 3.15f);
            reelCoin.setColor(sf::Color(255, 239, 95, static_cast<sf::Uint8>(205.0f * burst)));
            target.draw(reelCoin, sf::BlendAdd);
        }
    }

    sf::CircleShape glow(250.0f);
    glow.setOrigin(250.0f, 250.0f);
    glow.setPosition(ChestCenterX, ChestCenterY - 75.0f);
    glow.setFillColor(sf::Color(255, 170, 15, static_cast<sf::Uint8>(38.0f * burst)));
    target.draw(glow, sf::BlendAdd);

    for(int rayIndex = 0; rayIndex < 9; ++rayIndex)
    {
        const float rotation = m_elapsed * (rayIndex % 2 == 0 ? 19.0f : -14.0f) + rayIndex * 40.0f;
        sf::RectangleShape ray(sf::Vector2f(42.0f + (rayIndex % 3) * 18.0f, 530.0f));
        ray.setOrigin(ray.getSize().x * 0.5f, ray.getSize().y);
        ray.setPosition(ChestCenterX, ChestCenterY);
        ray.setRotation(rotation);
        const sf::Color rayColor = rayIndex % 2 == 0
            ? sf::Color(255, 207, 25, static_cast<sf::Uint8>(62.0f * burst))
            : sf::Color(202, 32, 255, static_cast<sf::Uint8>(48.0f * burst));
        ray.setFillColor(rayColor);
        target.draw(ray, sf::BlendAdd);
    }

    if(!m_openFrames.empty())
    {
        const float frameTime = std::max(0.0f, m_elapsed - OpeningStart);
        const std::size_t frameIndex = std::min(
            static_cast<std::size_t>(frameTime / OpeningFrameDuration),
            m_openFrames.size() - 1);
        ConfigureSprite(m_chest, m_openFrames[frameIndex]);
        m_chest.setPosition(ChestCenterX, ChestCenterY);
        const float entranceScale = 7.0f + 1.8f * (1.0f - SmoothStep(m_elapsed / 0.3f));
        m_chest.setScale(entranceScale, entranceScale);
        target.draw(m_chest);
    }

    if(!m_coinFrames.empty())
    {
        for(const CoinParticle& coin : m_coins)
        {
            const float age = m_elapsed - BurstStart - coin.delay;
            if(age < 0.0f || age > 3.1f)
            {
                continue;
            }

            const std::size_t frameIndex = static_cast<std::size_t>(
                (age * 15.0f + coin.spinOffset)) % m_coinFrames.size();
            sf::Sprite sprite;
            ConfigureSprite(sprite, m_coinFrames[frameIndex]);
            sprite.setScale(coin.scale * 2.25f, coin.scale * 2.25f);
            sprite.setPosition(
                ChestCenterX + coin.velocity.x * age,
                ChestCenterY - 35.0f + coin.velocity.y * age + 205.0f * age * age);
            const float fade = age > 2.35f ? Clamp01((3.1f - age) / 0.75f) : 1.0f;
            sprite.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(255.0f * fade)));
            target.draw(sprite, sf::BlendAdd);
        }
    }

    if(!m_frontFrames.empty())
    {
        const float frameTime = std::max(0.0f, m_elapsed - OpeningStart);
        const std::size_t frameIndex = std::min(
            static_cast<std::size_t>(frameTime / OpeningFrameDuration),
            m_frontFrames.size() - 1);
        ConfigureSprite(m_chestFront, m_frontFrames[frameIndex]);
        m_chestFront.setPosition(ChestCenterX, ChestCenterY + 25.0f);
        m_chestFront.setScale(7.0f, 7.0f);
        target.draw(m_chestFront);
    }

    if(m_moneyPile.texture && reveal > 0.0f)
    {
        ConfigureSprite(m_rewardIcon, m_moneyPile);
        m_rewardIcon.setPosition(ChestCenterX, 535.0f);
        const float pulse = 1.0f + std::sin(m_elapsed * 7.0f) * 0.06f;
        m_rewardIcon.setScale(4.8f * reveal * pulse, 4.8f * reveal * pulse);
        m_rewardIcon.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(255.0f * reveal)));
        target.draw(m_rewardIcon);
    }

    if(reveal > 0.0f)
    {
        target.draw(m_rewardLabel);
        target.draw(m_rewardValue);
        target.draw(m_totalValue);
    }

    if(m_elapsed >= ContinueStart)
    {
        const float pulse = 0.65f + 0.35f * std::sin((m_elapsed - ContinueStart) * 4.5f);
        sf::Text prompt = m_continuePrompt;
        prompt.setFillColor(sf::Color(255, 236, 160, static_cast<sf::Uint8>(190.0f + 65.0f * pulse)));
        target.draw(prompt);
    }
}

void TreasureRewardView::ConfigureSprite(sf::Sprite& sprite, const AssetTextureData& data) const
{
    if(!data.texture)
    {
        return;
    }
    sprite.setTexture(*data.texture, false);
    sprite.setTextureRect(data.rect);
    sprite.setOrigin(
        static_cast<float>(data.rect.width) * 0.5f,
        static_cast<float>(data.rect.height) * 0.5f);
}

void TreasureRewardView::RebuildCoinBurst()
{
    m_coins.clear();
    m_coins.reserve(CoinCount);
    std::uniform_real_distribution<float> angleDistribution(-2.95f, -0.19f);
    std::uniform_real_distribution<float> speedDistribution(220.0f, 590.0f);
    std::uniform_real_distribution<float> delayDistribution(0.0f, 0.72f);
    std::uniform_real_distribution<float> scaleDistribution(0.65f, 1.35f);
    std::uniform_real_distribution<float> spinDistribution(0.0f, 20.0f);
    for(std::size_t index = 0; index < CoinCount; ++index)
    {
        const float angle = angleDistribution(m_random);
        const float speed = speedDistribution(m_random);
        CoinParticle particle;
        particle.velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
        particle.delay = delayDistribution(m_random);
        particle.scale = scaleDistribution(m_random);
        particle.spinOffset = spinDistribution(m_random);
        m_coins.push_back(particle);
    }
}

void TreasureRewardView::ApplyPendingGold()
{
    const float progress = SmoothStep((m_elapsed - RewardRevealStart) / GoldCountDuration);
    const int targetApplied = std::min(m_goldReward, static_cast<int>(std::round(m_goldReward * progress)));
    const int delta = targetApplied - m_appliedGold;
    if(delta > 0 && m_onGoldAdded)
    {
        m_onGoldAdded(delta);
    }
    m_appliedGold = targetApplied;

    m_rewardValue.setString("+" + FormatGold(m_appliedGold));
    m_totalValue.setString("TOTAL  " + FormatGold(m_startingGold + m_appliedGold));
    CenterText(m_rewardValue, ChestCenterX, 335.0f);
    CenterText(m_totalValue, ChestCenterX, 430.0f);
}

void TreasureRewardView::Finish()
{
    if(!m_visible || m_finishing)
    {
        return;
    }

    m_finishing = true;
    if(m_appliedGold < m_goldReward)
    {
        const int delta = m_goldReward - m_appliedGold;
        if(m_onGoldAdded)
        {
            m_onGoldAdded(delta);
        }
        m_appliedGold = m_goldReward;
    }
    m_visible = false;
    if(m_onComplete)
    {
        m_onComplete();
    }
}

float TreasureRewardView::GetRevealProgress() const
{
    return SmoothStep((m_elapsed - RewardRevealStart) / 0.4f);
}
