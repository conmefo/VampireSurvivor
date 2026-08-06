#pragma once

#include "LevelUpOption.h"
#include "LevelUpRollEngine.h"
#include <vector>
#include <unordered_set>
#include <functional>

class Player;
class WeaponDataManager;
class WeaponFactory;

class LevelUpSessionController
{
public:
    LevelUpSessionController() = default;
    ~LevelUpSessionController() = default;

    void InitSession(int rerollCharges, int skipCharges, int banishCharges);

    void QueueLevelUp();
    bool HasPendingLevelUp() const { return m_pendingLevelUps > 0; }
    int GetPendingLevelUpCount() const { return m_pendingLevelUps; }

    void StartNextLevelUp(Player& player, const WeaponDataManager& weaponData);

    bool SelectOption(int index, Player& player, const WeaponDataManager& weaponData, WeaponFactory& factory, std::function<void(int)> onGoldGranted = nullptr);
    bool Reroll(Player& player, const WeaponDataManager& weaponData);
    bool Skip(Player& player);
    bool Banish(int index, Player& player, const WeaponDataManager& weaponData);

    const std::vector<LevelUpOption>& GetCurrentChoices() const { return m_currentChoices; }
    int GetRerollCharges() const { return m_rerollCharges; }
    int GetSkipCharges() const { return m_skipCharges; }
    int GetBanishCharges() const { return m_banishCharges; }

    bool IsSessionActive() const { return m_isSessionActive; }
    bool IsRandomLevelUpEnabled() const { return m_randomLevelUpEnabled; }
    void SetRandomLevelUpEnabled(bool enabled) { m_randomLevelUpEnabled = enabled; }

private:
    int m_pendingLevelUps = 0;
    int m_rerollCharges = 0;
    int m_skipCharges = 0;
    int m_banishCharges = 0;

    std::unordered_set<std::string> m_banishedItemIds;
    std::vector<LevelUpOption> m_currentChoices;
    LevelUpRollEngine m_rollEngine;
    bool m_randomLevelUpEnabled = false;
    bool m_isSessionActive = false;
};
