#include "WeaponSlotsHUD.h"
#include <algorithm>
#include <iostream>

void WeaponSlotsHUD::Initialize(const TextureAtlas& atlas)
{
    m_atlas = &atlas;

    m_frameData = atlas.GetTextureData("weaponSlots");
    if (!m_frameData.texture)
    {
        m_frameData = atlas.GetTextureData("weaponSlots.png");
    }

    m_lockedData = atlas.GetTextureData("no16");
    if (!m_lockedData.texture)
    {
        m_lockedData = atlas.GetTextureData("no16.png");
    }
}

AssetTextureData WeaponSlotsHUD::GetIconData(const std::string& key) const
{
    if (!m_atlas || key.empty())
    {
        return {};
    }

    AssetTextureData data = m_atlas->GetTextureData(key);
    if (data.texture)
    {
        return data;
    }

    if (key.size() > 4 && key.substr(key.size() - 4) == ".png")
    {
        data = m_atlas->GetTextureData(key.substr(0, key.size() - 4));
    }
    else
    {
        data = m_atlas->GetTextureData(key + ".png");
    }

    return data;
}

void WeaponSlotsHUD::DrawSlotRow(
    sf::RenderTarget& target,
    const std::vector<std::string>& itemIconKeys,
    int maxSlots,
    int rowIndex,
    const sf::Vector2f& basePos,
    float scale) const
{
    constexpr int TotalCellsPerRow = 6;
    const int effectiveMax = std::clamp(maxSlots, 0, TotalCellsPerRow);

    for (int col = 0; col < TotalCellsPerRow; ++col)
    {
        const float cellX = basePos.x + static_cast<float>(col) * m_config.cellWidth * scale;
        const float cellY = basePos.y + static_cast<float>(rowIndex) * m_config.cellHeight * scale;
        const float cellW = m_config.cellWidth * scale;
        const float cellH = m_config.cellHeight * scale;

        if (col < static_cast<int>(itemIconKeys.size()))
        {
            // Draw equipped item icon
            const AssetTextureData iconData = GetIconData(itemIconKeys[col]);
            if (iconData.texture && iconData.rect.width > 0 && iconData.rect.height > 0)
            {
                sf::Sprite iconSprite(*iconData.texture, iconData.rect);
                
                const float pad = m_config.iconPadding * scale;
                const float maxIconW = cellW - pad * 2.0f;
                const float maxIconH = cellH - pad * 2.0f;
                
                const float iconScaleX = maxIconW / static_cast<float>(iconData.rect.width);
                const float iconScaleY = maxIconH / static_cast<float>(iconData.rect.height);
                const float finalIconScale = std::min(iconScaleX, iconScaleY);

                iconSprite.setScale(finalIconScale, finalIconScale);
                
                const float drawW = static_cast<float>(iconData.rect.width) * finalIconScale;
                const float drawH = static_cast<float>(iconData.rect.height) * finalIconScale;
                
                const float offsetX = (cellW - drawW) * 0.5f;
                const float offsetY = (cellH - drawH) * 0.5f;
                
                iconSprite.setPosition(cellX + offsetX, cellY + offsetY);
                target.draw(iconSprite);
            }
        }
        else if (col >= effectiveMax)
        {
            // 6 - maxSlots cells on the right: draw locked slot icon (no16)
            if (m_lockedData.texture && m_lockedData.rect.width > 0 && m_lockedData.rect.height > 0)
            {
                sf::Sprite lockedSprite(*m_lockedData.texture, m_lockedData.rect);
                
                const float iconScaleX = (cellW / static_cast<float>(m_lockedData.rect.width)) * m_config.lockedIconScale;
                const float iconScaleY = (cellH / static_cast<float>(m_lockedData.rect.height)) * m_config.lockedIconScale;
                const float finalScale = std::min(iconScaleX, iconScaleY);

                lockedSprite.setScale(finalScale, finalScale);
                
                const float drawW = static_cast<float>(m_lockedData.rect.width) * finalScale;
                const float drawH = static_cast<float>(m_lockedData.rect.height) * finalScale;
                
                const float offsetX = (cellW - drawW) * 0.5f;
                const float offsetY = (cellH - drawH) * 0.5f;
                
                lockedSprite.setPosition(cellX + offsetX, cellY + offsetY);
                target.draw(lockedSprite);
            }
        }
        // else: empty unlocked cell inside weaponSlots frame (drawn naturally by frame background)
    }
}

void WeaponSlotsHUD::Draw(
    sf::RenderTarget& target,
    const std::vector<std::unique_ptr<Player>>& players,
    const WeaponDataManager& weaponData,
    int maxSlots) const
{
    if (!m_atlas || !m_frameData.texture)
    {
        return;
    }

    // 1. Draw the weaponSlots frame sprite
    sf::Sprite frameSprite(*m_frameData.texture, m_frameData.rect);
    frameSprite.setPosition(m_config.position);
    frameSprite.setScale(m_config.scale, m_config.scale);
    target.draw(frameSprite);

    // 2. Collect weapons and buffs from players
    std::vector<std::string> weaponIcons;
    std::vector<std::string> buffIcons;

    for (const auto& player : players)
    {
        if (!player) continue;

        // Collect weapons and weapon-based passives
        const auto& weapons = player->GetWeaponInventory().GetWeapons();
        for (const auto& w : weapons)
        {
            if (!w) continue;
            const auto& profile = w->GetProfile();
            std::string iconKey = profile.GetFrameName();
            if (iconKey.empty())
            {
                iconKey = profile.GetId();
            }

            if (!profile.IsPowerUp())
            {
                if (std::find(weaponIcons.begin(), weaponIcons.end(), iconKey) == weaponIcons.end())
                {
                    weaponIcons.push_back(iconKey);
                }
            }
            else
            {
                if (std::find(buffIcons.begin(), buffIcons.end(), iconKey) == buffIcons.end())
                {
                    buffIcons.push_back(iconKey);
                }
            }
        }

        // Collect passives / buffs from player's passiveLevels map (e.g. Spinach, Hollow Heart, Armor, etc.)
        for (const auto& pair : player->GetPassiveLevels())
        {
            const std::string& passiveId = pair.first;
            const int level = pair.second;
            if (level <= 0) continue;

            const WeaponProfile& profile = weaponData.GetWeaponById(passiveId);
            std::string iconKey = profile.GetFrameName();
            if (iconKey.empty())
            {
                iconKey = profile.GetId();
            }
            if (iconKey.empty())
            {
                iconKey = passiveId;
            }

            if (std::find(buffIcons.begin(), buffIcons.end(), iconKey) == buffIcons.end())
            {
                buffIcons.push_back(iconKey);
            }
        }
    }

    // Row 0 = Weapons (1st row)
    DrawSlotRow(target, weaponIcons, maxSlots, 0, m_config.position, m_config.scale);

    // Row 1 = Buffs / Passives (2nd row)
    DrawSlotRow(target, buffIcons, maxSlots, 1, m_config.position, m_config.scale);
}
