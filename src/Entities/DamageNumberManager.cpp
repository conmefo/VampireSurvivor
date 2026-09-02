#include "DamageNumberManager.h"

#include <algorithm>
#include <cmath>

namespace
{
    constexpr float InitialDigitScale = 1.6f;
    constexpr float FinalDigitScale = 1.0f;
    constexpr float DigitAdvancePixels = 6.0f;
    constexpr float SpawnYOffset = -18.0f;
}

void DamageNumberManager::Initialize(const TextureAtlas& atlas)
{
    Clear();

    m_ready = true;
    for(int i = 0; i < 10; ++i)
    {
        const std::string assetId = "DamageNumbers Atlas_" + std::to_string(i);
        AssetTextureData data = atlas.GetTextureData(assetId);
        m_digits[static_cast<std::size_t>(i)] = { data.texture, data.rect };

        if(!data.texture)
        {
            m_ready = false;
        }
    }
}

void DamageNumberManager::Clear()
{
    m_numbers.clear();
    m_spawnCounter = 0;
}

void DamageNumberManager::Spawn(float damage, const sf::Vector2f& position)
{
    if(!m_ready || damage <= 0.0f)
    {
        return;
    }

    const std::string damageText = FormatDamage(damage);
    if(damageText.empty())
    {
        return;
    }

    DamageNumber number;
    number.digits.reserve(damageText.size());

    const float totalWidth = ((static_cast<float>(damageText.size()) - 1.0f) * DigitAdvancePixels + 5.0f) * InitialDigitScale;
    float cursorX = -totalWidth * 0.5f;
    const float horizontalOffset = static_cast<float>(static_cast<int>(m_spawnCounter % 5) - 2) * 4.0f;
    const float horizontalSpeed = static_cast<float>(static_cast<int>(m_spawnCounter % 3) - 1) * 8.0f;
    const sf::Vector2f basePosition = position + sf::Vector2f(horizontalOffset, SpawnYOffset);

    for(char character : damageText)
    {
        if(character < '0' || character > '9')
        {
            continue;
        }

        const DigitGlyph& glyph = m_digits[static_cast<std::size_t>(character - '0')];
        if(!glyph.texture)
        {
            continue;
        }

        sf::Sprite digit(*glyph.texture, glyph.rect);
        digit.setScale(InitialDigitScale, InitialDigitScale);
        digit.setOrigin(0.0f, static_cast<float>(glyph.rect.height) * 0.5f);
        digit.setPosition(basePosition + sf::Vector2f(cursorX, 0.0f));
        number.digits.push_back(digit);

        cursorX += DigitAdvancePixels * InitialDigitScale;
    }

    if(number.digits.empty())
    {
        return;
    }

    number.velocity = sf::Vector2f(horizontalSpeed, -46.0f);

    ++m_spawnCounter;
    m_numbers.push_back(number);
}

void DamageNumberManager::Update(float dt)
{
    for(auto& number : m_numbers)
    {
        number.age += dt;
        for(auto& digit : number.digits)
        {
            digit.move(number.velocity * dt);
        }
        number.velocity.y -= 12.0f * dt;

        const float t = number.lifetime > 0.0f ? std::clamp(number.age / number.lifetime, 0.0f, 1.0f) : 1.0f;
        const sf::Uint8 alpha = static_cast<sf::Uint8>(255.0f * (1.0f - t));
        const float currentScale = InitialDigitScale + (FinalDigitScale - InitialDigitScale) * t;

        for(auto& digit : number.digits)
        {
            digit.setScale(currentScale, currentScale);
            digit.setColor(sf::Color(255, 255, 255, alpha));
        }
    }

    m_numbers.erase(
        std::remove_if(m_numbers.begin(), m_numbers.end(),
            [](const DamageNumber& number) {
                return number.age >= number.lifetime;
            }),
        m_numbers.end());
}

void DamageNumberManager::Draw(sf::RenderTarget& target) const
{
    for(const auto& number : m_numbers)
    {
        for(const auto& digit : number.digits)
        {
            target.draw(digit);
        }
    }
}

std::string DamageNumberManager::FormatDamage(float damage)
{
    return std::to_string(std::max(0, static_cast<int>(std::round(damage))));
}
