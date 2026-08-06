#include "LevelUpSessionController.h"
#include "../../Entities/Player.h"
#include "../../Core/Data/WeaponDataManager.h"
#include "../../Entities/Weapons/WeaponFactory.h"

void LevelUpSessionController::InitSession(int rerollCharges, int skipCharges, int banishCharges)
{
    m_pendingLevelUps = 0;
    m_rerollCharges = rerollCharges;
    m_skipCharges = skipCharges;
    m_banishCharges = banishCharges;
    m_banishedItemIds.clear();
    m_currentChoices.clear();
    m_isSessionActive = false;
}

void LevelUpSessionController::QueueLevelUp()
{
    m_pendingLevelUps++;
}

void LevelUpSessionController::StartNextLevelUp(Player& player, const WeaponDataManager& weaponData)
{
    if (m_pendingLevelUps <= 0)
    {
        m_isSessionActive = false;
        return;
    }

    m_isSessionActive = true;
    float luckMultiplier = 1.0f; // Can be pulled from player stats/buffs
    bool healthMissing = (player.GetCurrentHealth() < player.GetMaxHealth());

    m_currentChoices = m_rollEngine.RollChoices(
        player.GetLevel(),
        luckMultiplier,
        healthMissing,
        player.GetWeaponInventory(),
        weaponData,
        m_banishedItemIds,
        false
    );
}

bool LevelUpSessionController::SelectOption(
    int index,
    Player& player,
    const WeaponDataManager& weaponData,
    WeaponFactory& factory,
    std::function<void(int)> onGoldGranted)
{
    if (index < 0 || index >= static_cast<int>(m_currentChoices.size()))
    {
        return false;
    }

    const auto& option = m_currentChoices[index];

    if (option.type == LevelUpOptionType::WeaponUpgrade || option.type == LevelUpOptionType::PassiveUpgrade)
    {
        if (player.GetWeaponInventory().HasWeapon(option.id))
        {
            player.GetWeaponInventory().LevelUpWeapon(option.id);
        }
        else
        {
            auto newWeapon = factory.Create(option.id);
            if (newWeapon)
            {
                player.GetWeaponInventory().AddWeapon(std::move(newWeapon));
            }
        }
    }
    else if (option.type == LevelUpOptionType::FloorChicken)
    {
        player.Heal(30.0f);
    }
    else if (option.type == LevelUpOptionType::GoldBag)
    {
        if (onGoldGranted)
        {
            onGoldGranted(25);
        }
    }

    m_pendingLevelUps--;
    m_currentChoices.clear();
    player.GrantInvulnerability(0.5f);

    if (m_pendingLevelUps > 0)
    {
        StartNextLevelUp(player, weaponData);
    }
    else
    {
        m_isSessionActive = false;
    }

    return true;
}

bool LevelUpSessionController::Reroll(Player& player, const WeaponDataManager& weaponData)
{
    if (m_rerollCharges <= 0)
    {
        return false;
    }

    m_rerollCharges--;
    float luckMultiplier = 1.0f;
    bool healthMissing = (player.GetCurrentHealth() < player.GetMaxHealth());

    m_currentChoices = m_rollEngine.RollChoices(
        player.GetLevel(),
        luckMultiplier,
        healthMissing,
        player.GetWeaponInventory(),
        weaponData,
        m_banishedItemIds,
        false
    );

    return true;
}

bool LevelUpSessionController::Skip(Player& player)
{
    if (m_skipCharges <= 0)
    {
        return false;
    }

    m_skipCharges--;
    m_pendingLevelUps--;
    m_currentChoices.clear();

    // Grant 20% XP bonus on skip
    player.AddExperience(player.GetTargetExperience() * 0.2f);
    player.GrantInvulnerability(0.5f);

    if (m_pendingLevelUps > 0)
    {
        // Session active next level up will be started by outer loop if needed
        m_isSessionActive = false;
    }
    else
    {
        m_isSessionActive = false;
    }

    return true;
}

bool LevelUpSessionController::Banish(int index, Player& player, const WeaponDataManager& weaponData)
{
    if (m_banishCharges <= 0 || index < 0 || index >= static_cast<int>(m_currentChoices.size()))
    {
        return false;
    }

    m_banishCharges--;
    m_banishedItemIds.insert(m_currentChoices[index].id);

    float luckMultiplier = 1.0f;
    bool healthMissing = (player.GetCurrentHealth() < player.GetMaxHealth());

    m_currentChoices = m_rollEngine.RollChoices(
        player.GetLevel(),
        luckMultiplier,
        healthMissing,
        player.GetWeaponInventory(),
        weaponData,
        m_banishedItemIds,
        false
    );

    return true;
}
