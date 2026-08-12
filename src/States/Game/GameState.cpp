#include "GameState.h"
#include "PauseMenuView.h"
#include "GameOverView.h"
#include "TreasureRewardView.h"
#include "../../Entities/Projectiles/RunetracerProjectile.h"
#include "../StateManager.h"
#include "../Menu/MainMenuState.h"
#include "../../Core/WindowSettings.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>
#include "../../Core/Data/CharacterDataManager.h"
#include "../../Core/Data/PlayerProgressionManager.h"
#include "../../Core/Data/WeaponDataManager.h"
#include "../../Entities/Weapons/WhipWeapon.h"
#include "../../Entities/Weapons/MagicMissileWeapon.h"
#include "../../Entities/Weapons/FireballWeapon.h"
#include "../../Entities/Weapons/RunetracerWeapon.h"
#include "../../Entities/Weapons/KnifeWeapon.h"
#include "../../Entities/Weapons/AxeWeapon.h"
#include "../../Entities/Weapons/GarlicWeapon.h"
#include "../../Entities/Weapons/SantaWaterWeapon.h"
#include "../../Entities/Pickups/TreasureChestManager.h"
#include "../../Core/Math/MathUtils.h"
#include "../../Core/Physics/Collision.h"
#include "../../UI/Elements/RunGoldDisplay.h"
#include "../../UI/ParticleTuningUI.h"
#include "../../Core/Audio/AudioIdentifiers.h"

namespace {
constexpr std::size_t MaxRuntimeEnemies = 2000;
constexpr int MaxOpeningSpawns = 12;
constexpr int MaxSpawnBatchPerTick = 3;
constexpr int MaxEventSpawnBatchPerTick = 4;
constexpr int MaxEventSpawnsPerTrigger = 16;
constexpr int MaxQueuedEventSpawns = 32;
constexpr int DefaultEventSpawnCount = 8;
constexpr float MinWaveSpawnIntervalSeconds = 0.15f;
constexpr float EventSpawnCooldownSeconds = 0.1f;
constexpr float DefaultEventRepeatIntervalMs = 1000.0f;
constexpr float MinEventRepeatIntervalMs = 250.0f;
constexpr float Pi = 3.14159265358979323846f;

bool IsSupportedStageEvent(const std::string& eventType)
{
    return eventType == "BAT_SWARM" ||
           eventType == "GENERIC_SWARM" ||
           eventType == "FLOWER_WALL" ||
           eventType == "MEDUSA_WALL";
}

float GetStageEventRepeatIntervalMs(const StageWaveEvent& event)
{
    const int repeatCount = std::max(1, event.repeat);
    if(repeatCount <= 1)
    {
        return 0.0f;
    }

    if(event.durationMs > 0.0f)
    {
        return std::max(MinEventRepeatIntervalMs, event.durationMs / static_cast<float>(repeatCount));
    }

    if(event.delayMs > 0.0f)
    {
        return std::max(MinEventRepeatIntervalMs, event.delayMs);
    }

    return DefaultEventRepeatIntervalMs;
}

const char *GetStageEnemyPath(int stageNumber) {
    return stageNumber == 1 ? "assets/Data/enemies/forest_enemies.json"
                            : "assets/Data/enemies/stage_enemies.json";
}

const char *GetStageMapPath(int stageNumber) {
    if (stageNumber == 1) {
        return "assets/Data/maps/forest_map.json";
    }

    return stageNumber == 3 ? "assets/Data/maps/plant_map.json"
                            : "assets/Data/maps/library_map.json";
}

const char *GetStageName(int stageNumber) {
    if (stageNumber == 1) {
        return "Mad Forest";
    }

    return stageNumber == 3 ? "Dairy Plant" : "Inlaid Library";
}

const char *GetStageWaveKey(int stageNumber) {
    if (stageNumber == 1) {
        return "FOREST";
    }

    return stageNumber == 3 ? "WAREHOUSE" : "LIBRARY";
}

bool StartsWith(const std::string& value, const char* prefix) {
    return value.rfind(prefix, 0) == 0;
}

static bool s_disableLevelUpUI = true;
} // namespace

GameState::GameState(StateContext context, TileMapManager& mapManager, const std::vector<std::string>& selectedCharacterIds)
    : BaseState(std::move(context)), m_mapManager(mapManager), m_enemyPool(m_enemyDatabase), m_selectedCharacterIds(selectedCharacterIds)
    , m_weaponFactory(m_context.weaponData) {}

GameState::~GameState()
{
    if(m_treasureRewardView)
    {
        m_treasureRewardView->CompleteImmediately();
    }
    BankRunGold();
}

void GameState::Init() {
    std::cout << "GameState Init" << std::endl;

    m_sharedExperience = 0.0f;
    m_sharedLevel = 1;

    m_vfxManager.Initialize(m_context.atlas);
    m_particleManager.Initialize(&m_context.atlas, &m_context.particleData);
    m_projectileManager.Initialize(&m_particleManager);
    m_experienceGems.Initialize(m_context.atlas);
    m_experienceGems.SetOnGemCollected([this](float xp) {
        m_context.audio.PlaySfx(SfxID::GemPickup);
        AddSharedExperience(xp);
    });
    m_damageNumbers.Initialize(m_context.atlas);
    m_treasureChests = std::make_unique<TreasureChestManager>();
    m_treasureChests->Initialize(m_context.atlas);

    m_worldView.setSize(ViewWidth / WorldZoom, ViewHeight / WorldZoom);

    if(!m_stageWaveData.LoadData("assets/Data/STAGE_DATA.json"))
    {
        std::cerr << "GameState: failed to load stage wave data" << std::endl;
    }

    LoadStage(1);

    // Start stage BGM
    m_context.audio.PlayMusic(BgmID::StageForest);

    m_players.clear();
    m_playerHUDs.clear();
    m_pulsePets.clear();

    const bool isMultiplayer = m_selectedCharacterIds.size() > 1;
    const int maxWeaponSlots = isMultiplayer ? 3 : 6;

    // Collect all starting weapons for initial distribution
    std::vector<std::string> initialWeaponIds;

    for (std::size_t idx = 0; idx < m_selectedCharacterIds.size(); ++idx)
    {
        const std::string& charId = m_selectedCharacterIds[idx];
        const CharacterProfile& profile = m_context.characterData.GetCharacterById(charId);

        std::string originalSpriteName = profile.GetSpriteName();
        std::string prefix = originalSpriteName;
        size_t dotPos = prefix.find_last_of('.');
        if(dotPos != std::string::npos) { prefix = prefix.substr(0, dotPos); }
        size_t underscorePos = prefix.find_last_of('_');
        if(underscorePos != std::string::npos) { prefix = prefix.substr(0, underscorePos); }

        std::vector<sf::IntRect> frames;
        const sf::Texture* texture = nullptr;

        for(int i = 1; i <= profile.GetWalkingFrames(); ++i)
        {
            std::string frameName = prefix + (i < 10 ? "_0" : "_") + std::to_string(i);
            AssetTextureData data = m_context.atlas.GetTextureData(frameName);
            if(data.texture)
            {
                texture = data.texture;
                frames.push_back(data.rect);
            }
        }

        if(texture && !frames.empty())
        {
            auto player = std::make_unique<Player>(profile, *texture, frames, static_cast<int>(idx));
            player->SetIsMultiplayer(isMultiplayer);
            sf::Vector2f spawnPos = m_cameraCenter + sf::Vector2f(idx * 50.0f, 0.0f);
            player->SetPosition(spawnPos);
            player->GetWeaponInventory().SetFactory(&m_weaponFactory);
            player->GetWeaponInventory().SetAudioService(&m_context.audio);
            player->GetWeaponInventory().SetMaxSlots(maxWeaponSlots);
            player->ApplyGlobalBuffs(m_context.progressionData, m_context.powerUpData);

            int pulsePetLevel = m_context.progressionData.GetPowerUpLevel("PET_PULSE");
            if (pulsePetLevel >= 1)
            {
                m_pulsePets.push_back(std::make_unique<PulsePet>(m_context.atlas, pulsePetLevel));
            }

            player->SetOnHitVfxCallback([this](const std::string& vfxName, sf::Vector2f pos) {
                const HitVfxProfile& vfxProfile = m_context.hitVfxData.GetVfxByName(vfxName);
                if(vfxProfile.GetId() != -1) {
                    m_vfxManager.PlayVfx(vfxProfile, pos);
                }
            });

            m_playerHUDs.push_back(std::make_unique<PlayerHUD>(*player));
            m_players.push_back(std::move(player));

            const std::string& startWep = profile.GetStartingWeaponId();
            if(!startWep.empty() && std::find(initialWeaponIds.begin(), initialWeaponIds.end(), startWep) == initialWeaponIds.end())
            {
                initialWeaponIds.push_back(startWep);
            }
        }
        else
        {
            std::cerr << "Failed to find texture data for player sprite: " << profile.GetSpriteName() << std::endl;
        }
    }

    m_expBar = std::make_unique<ExpBar>(m_context.atlas, m_context.fonts.Get(FontID::Main));

    // Distribute starting weapons to all players
    for (const std::string& wepId : initialWeaponIds)
    {
        for (auto& p : m_players)
        {
            auto w = m_weaponFactory.Create(wepId);
            if(w)
            {
                p->GetWeaponInventory().AddWeapon(std::move(w));
            }
        }
    }

    const sf::Font* font = m_context.fonts.GetPtr(FontID::Main);
    const sf::Font* boldFont = m_context.fonts.GetPtr(FontID::Bold);
    if(font && boldFont && !m_players.empty())
    {
        const CharacterProfile& profile0 = m_context.characterData.GetCharacterById(m_selectedCharacterIds[0]);
        m_pauseMenu = std::make_unique<PauseMenuView>(
            m_context.atlas,
            *font,
            *boldFont,
            profile0,
            m_context.weaponData,
            m_context.progressionData,
            m_context.powerUpData);
        m_pauseMenu->SetStageName(GetStageName(m_currentStage));
        m_pauseMenu->SetHitboxesVisible(m_showHitboxes);
        m_pauseMenu->SetOnResume([this]() { TogglePause(); });
        m_pauseMenu->SetOnQuit([this]() { ReturnToMainMenu(); });
        m_pauseMenu->SetOnToggleHitboxes([this]() {
            m_showHitboxes = !m_showHitboxes;
            m_pauseMenu->SetHitboxesVisible(m_showHitboxes);
        });

        const sf::Texture* gameOverTitle = m_context.textures.GetPtr("GameOverTitle");
        if(gameOverTitle)
        {
            m_gameOverView =
                std::make_unique<GameOverView>(m_context.atlas, *gameOverTitle, *boldFont);
            m_gameOverView->SetOnQuit([this]() { ReturnToMainMenu(); });
        }
    }

    if(boldFont && font)
    {
        m_runGoldDisplay = std::make_unique<RunGoldDisplay>(m_context.atlas, *boldFont);
        m_treasureRewardView =
            std::make_unique<TreasureRewardView>(m_context.atlas, *boldFont);
        m_treasureRewardView->SetOnGoldAdded([this](int gold) { AddRunGold(gold); });
    }

    if (font)
    {
        int rerolls = m_context.progressionData.GetPowerUpLevel("reroll");
        int skips = m_context.progressionData.GetPowerUpLevel("skip");
        int banishes = m_context.progressionData.GetPowerUpLevel("banish");
        m_levelUpController.InitSession(rerolls, skips, banishes);

        m_levelUpView = std::make_unique<SimpleTextLevelUpView>(m_context.atlas, *font);

        m_levelUpView->SetOnSelectOption([this](int index) {
            if (!m_players.empty())
            {
                m_levelUpController.SelectOption(index, m_players, m_sharedLevel, m_context.weaponData, m_weaponFactory, [this](int gold) {
                    AddRunGold(gold);
                });
                if (m_levelUpController.IsSessionActive())
                {
                    m_levelUpView->UpdateChoices(
                        m_levelUpController.GetCurrentChoices(),
                        m_levelUpController.GetPendingLevelUpCount(),
                        m_levelUpController.GetRerollCharges(),
                        m_levelUpController.GetSkipCharges(),
                        m_levelUpController.GetBanishCharges()
                    );
                }
            }
        });

        m_levelUpView->SetOnReroll([this]() {
            if (!m_players.empty())
            {
                if (m_levelUpController.Reroll(m_players, m_sharedLevel, m_context.weaponData))
                {
                    m_levelUpView->UpdateChoices(
                        m_levelUpController.GetCurrentChoices(),
                        m_levelUpController.GetPendingLevelUpCount(),
                        m_levelUpController.GetRerollCharges(),
                        m_levelUpController.GetSkipCharges(),
                        m_levelUpController.GetBanishCharges()
                    );
                }
            }
        });

        m_levelUpView->SetOnSkip([this]() {
            if (!m_players.empty())
            {
                if (m_levelUpController.Skip(m_players))
                {
                    if (m_levelUpController.IsSessionActive())
                    {
                        m_levelUpView->UpdateChoices(
                            m_levelUpController.GetCurrentChoices(),
                            m_levelUpController.GetPendingLevelUpCount(),
                            m_levelUpController.GetRerollCharges(),
                            m_levelUpController.GetSkipCharges(),
                            m_levelUpController.GetBanishCharges()
                        );
                    }
                }
            }
        });

        m_levelUpView->SetOnBanish([this](int index) {
            if (!m_players.empty())
            {
                if (m_levelUpController.Banish(index, m_players, m_sharedLevel, m_context.weaponData))
                {
                    m_levelUpView->UpdateChoices(
                        m_levelUpController.GetCurrentChoices(),
                        m_levelUpController.GetPendingLevelUpCount(),
                        m_levelUpController.GetRerollCharges(),
                        m_levelUpController.GetSkipCharges(),
                        m_levelUpController.GetBanishCharges()
                    );
                }
            }
        });
    }

    for (auto& p : m_players)
    {
        p->SetOnLevelUpCallback([this]() {
            m_levelUpController.QueueLevelUp();
            if (!m_levelUpController.IsSessionActive() && !m_players.empty())
            {
                m_levelUpController.StartNextLevelUp(m_players, m_sharedLevel, m_context.weaponData);
                if (m_levelUpView && m_levelUpController.IsSessionActive())
                {
                    m_levelUpView->UpdateChoices(
                        m_levelUpController.GetCurrentChoices(),
                        m_levelUpController.GetPendingLevelUpCount(),
                        m_levelUpController.GetRerollCharges(),
                        m_levelUpController.GetSkipCharges(),
                        m_levelUpController.GetBanishCharges()
                    );
                }
            }
        });
    }

    if(boldFont)
    {
        m_stageTimerText.setFont(*boldFont);
        m_stageTimerText.setCharacterSize(42);
        m_stageTimerText.setFillColor(sf::Color(245, 245, 235));
        m_stageTimerText.setOutlineColor(sf::Color(35, 35, 35, 230));
        m_stageTimerText.setOutlineThickness(2.0f);

        m_stageTimerShadowText = m_stageTimerText;
        m_stageTimerShadowText.setFillColor(sf::Color(0, 0, 0, 140));
        m_stageTimerShadowText.setOutlineThickness(0.0f);

        m_stageInfoText.setFont(*font);
        m_stageInfoText.setCharacterSize(17);
        m_stageInfoText.setFillColor(sf::Color(210, 210, 198));
        m_stageInfoText.setOutlineColor(sf::Color(0, 0, 0, 190));
        m_stageInfoText.setOutlineThickness(1.0f);

        m_stageTimerBacking.setFillColor(sf::Color(0, 0, 0, 120));
        UpdateStageTimerText();

        m_gemTuningUI = std::make_unique<GemTuningUI>(m_context.atlas, *font, m_experienceGems);
        m_gemTuningUI->SetPosition(sf::Vector2f(110.0f, 100.0f));
    }

    ApplyCameraToView();
}

void GameState::HandleInput(sf::Event &event, sf::RenderWindow &window) {
    if (m_levelUpController.IsSessionActive())
    {
        if (m_levelUpView)
        {
            m_levelUpView->HandleEvent(event, window);
        }
        return;
    }

    if(m_treasureRewardView && m_treasureRewardView->IsVisible())
    {
        m_treasureRewardView->HandleEvent(event, &window);
        return;
    }

    if(IsGameOverVisible())
    {
        if(m_gameOverView)
        {
            const sf::Vector2u windowSize = window.getSize();
            const sf::FloatRect viewport = MathUtils::CalculateLetterboxViewport(
                static_cast<float>(windowSize.x),
                static_cast<float>(windowSize.y),
                ViewWidth / ViewHeight);
            sf::View previousView = window.getView();
            sf::View resultView(sf::FloatRect(0.0f, 0.0f, ViewWidth, ViewHeight));
            resultView.setViewport(viewport);
            window.setView(resultView);
            m_gameOverView->HandleEvent(event, window);
            window.setView(previousView);
        }
        else if(event.type == sf::Event::KeyPressed &&
                (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Escape))
        {
            ReturnToMainMenu();
        }
        return;
    }

    if(m_runState == RunState::DefeatAnimating)
    {
        return;
    }

    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        if(m_isPaused && m_pauseMenu)
        {
            if(m_pauseMenu->IsOptionsOpen())
            {
                m_pauseMenu->HandleEvent(event, window);
            }
            else
            {
                TogglePause();
            }
        }
        else
        {
            TogglePause();
        }
    } else if(m_isPaused) {
        if(m_pauseMenu)
        {
            m_pauseMenu->HandleEvent(event, window);
        }
        return;
#ifndef NDEBUG
    } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F10) {
        FinishRun(RunState::Completed);
        return;
    } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::G) {
        AddRunGold(100);
        return;
    } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::L) {
        // Debug: level-up Song of Mana
        BroadcastWeaponLevelUp("SONG");
        return;
    } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C) {
        Player* p0 = GetFirstAlivePlayer();
        if(p0 && m_treasureChests)
        {
            m_treasureChests->Spawn(p0->GetPosition() + sf::Vector2f(80.0f, 0.0f));
        }
        return;
    } else if (event.type == sf::Event::KeyPressed &&
               (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1)) {
        if(m_treasureRewardView)
        {
            m_treasureRewardView->Show(100, m_runGold, 1);
        }
        return;
    } else if (event.type == sf::Event::KeyPressed &&
               (event.key.code == sf::Keyboard::Num3 || event.key.code == sf::Keyboard::Numpad3)) {
        if(m_treasureRewardView)
        {
            m_treasureRewardView->Show(300, m_runGold, 3);
        }
        return;
    } else if (event.type == sf::Event::KeyPressed &&
               (event.key.code == sf::Keyboard::Num5 || event.key.code == sf::Keyboard::Numpad5)) {
        if(m_treasureRewardView)
        {
            m_treasureRewardView->Show(500, m_runGold, 5);
        }
        return;
    } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::T) {
        if(m_treasureRewardView)
        {
            m_treasureRewardView->Show(150, m_runGold, 1);
        }
        return;
    } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F1) {
        Player* p0 = GetFirstAlivePlayer();
        if (p0) {
            m_experienceGems.SpawnMultiple(p0->GetPosition(), 10, 2.0f); // 10 Blue Gems
        }
        return;
    } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F2) {
        Player* p0 = GetFirstAlivePlayer();
        if (p0) {
            m_experienceGems.SpawnMultiple(p0->GetPosition(), 10, 10.0f); // 10 Green Gems
        }
        return;
    } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F3) {
        Player* p0 = GetFirstAlivePlayer();
        if (p0) {
            m_experienceGems.SpawnMultiple(p0->GetPosition(), 10, 50.0f); // 10 Red Gems
        }
        return;
    } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F4) {
        Player* p0 = GetFirstAlivePlayer();
        if (p0) {
            m_experienceGems.SpawnMultiple(p0->GetPosition(), 450, 2.0f); // Test 400-gem aggregation cap
        }
        return;
    } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F5) {
        AddSharedExperience(500.0f); // Fast forward level-up progression
        return;
    } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F6) {
        Player* p0 = GetFirstAlivePlayer();
        if (p0) {
            m_experienceGems.SpawnRandomInRadius(p0->GetPosition(), 30, 40.0f, 300.0f); // Random gems around player
        }
        return;
    } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F7) {
        BroadcastWeaponAdd("MAGNET");
        return;
#endif
    } else if (event.type == sf::Event::KeyPressed && (event.key.code == sf::Keyboard::B || event.key.code == sf::Keyboard::F8)) {
        Player* p0 = GetFirstAlivePlayer();
        if (p0) {
            const sf::Vector2f pPos = p0->GetPosition();
            constexpr int count = 500;
            constexpr float radius = 350.0f;
            for (int i = 0; i < count; ++i) {
                const float angle = (2.0f * 3.14159265f * i) / count;
                const sf::Vector2f spawnPos = pPos + sf::Vector2f(std::cos(angle) * radius, std::sin(angle) * radius);
                m_enemyPool.Acquire("BAT1", spawnPos);
            }
            std::cout << "[STRESS TEST] Spawned 500 enemies! Total active: " << m_enemyPool.GetActiveCount() << std::endl;
        }
    } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::N) {
        Player* p0 = GetFirstAlivePlayer();
        if (p0) {
            const sf::Vector2f pPos = p0->GetPosition();
            constexpr int count = 1000;
            constexpr float radius = 450.0f;
            for (int i = 0; i < count; ++i) {
                const float angle = (2.0f * 3.14159265f * i) / count;
                const sf::Vector2f spawnPos = pPos + sf::Vector2f(std::cos(angle) * radius, std::sin(angle) * radius);
                m_enemyPool.Acquire("ZOMBIE", spawnPos);
            }
            std::cout << "[STRESS TEST] Spawned 1000 enemies! Total active: " << m_enemyPool.GetActiveCount() << std::endl;
        }
    } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C) {
        m_enemyPool.DeactivateAll();
        std::cout << "[STRESS TEST] Cleared all active enemies!" << std::endl;
    } else if (event.type == sf::Event::KeyPressed && (event.key.code == sf::Keyboard::V || event.key.code == sf::Keyboard::F9)) {
        s_disableLevelUpUI = !s_disableLevelUpUI;
        std::cout << "[STRESS TEST] Level Up UI set to: " << (s_disableLevelUpUI ? "DISABLED" : "ENABLED") << std::endl;
    } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::H) {
        m_showHitboxes = !m_showHitboxes;
    } else if (event.type == sf::Event::KeyPressed &&
               (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::Numpad2)) {
        LoadStage(2);
    } else if (event.type == sf::Event::KeyPressed &&
               (event.key.code == sf::Keyboard::Num3 || event.key.code == sf::Keyboard::Numpad3)) {
        LoadStage(3);
    } else if (event.type == sf::Event::KeyPressed &&
               (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1)) {
        LoadStage(1);
    } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::K) {
        Player* p0 = GetFirstAlivePlayer();
        if (p0) {
            p0->TakeDamage(99999.0f);
        }
    } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::L) {
        for (auto& p : m_players) {
            if (p && p->IsDead()) {
                p->Revive();
            }
        }
    } else if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), m_worldView);
        std::string vfxName = "Default";
        if(event.mouseButton.button == sf::Mouse::Left) vfxName = "Fire";
        else if(event.mouseButton.button == sf::Mouse::Right) vfxName = "TimeFreeze";
        else if(event.mouseButton.button == sf::Mouse::Middle) vfxName = "Explo";

        const HitVfxProfile& profile = m_context.hitVfxData.GetVfxByName(vfxName);
        m_vfxManager.PlayVfx(profile, mousePos);
    } else if (event.type == sf::Event::Resized) {
        m_worldView.setSize(ViewWidth / WorldZoom, ViewHeight / WorldZoom);
        ApplyCameraToView();
    }

    if (m_tuningUI) {
        sf::View oldView = window.getView();
        sf::View tuningUiView(sf::FloatRect(0.0f, 0.0f, ViewWidth, ViewHeight));
        window.setView(tuningUiView);
        m_tuningUI->HandleEvent(event, window);
        window.setView(oldView);
    }

    if (m_gemTuningUI) {
        sf::View oldView = window.getView();
        sf::View tuningUiView(sf::FloatRect(0.0f, 0.0f, ViewWidth, ViewHeight));
        window.setView(tuningUiView);
        m_gemTuningUI->HandleEvent(event, window);
        window.setView(oldView);
    }
}

void GameState::Update(float dt) {
    // Update audio listener position and viewport culling bounds in real-time
    m_context.audio.Update(dt, GetViewBounds(), m_cameraCenter);

    if(m_runGoldDisplay)
    {
        m_runGoldDisplay->Update(dt);
    }
    if(m_expBar)
    {
        m_expBar->Update(dt, m_sharedLevel, GetSharedExpProgressRatio());
    }

    if(m_treasureRewardView && m_treasureRewardView->IsVisible())
    {
        m_treasureRewardView->Update(dt);
        return;
    }

    if (m_levelUpController.IsSessionActive())
    {
        if (m_levelUpView)
        {
            m_levelUpView->Update(dt);
        }
        return;
    }

    if(IsGameOverVisible())
    {
        if(m_gameOverView)
        {
            m_gameOverView->Update(dt);
        }
        return;
    }

    if(m_runState == RunState::DefeatAnimating)
    {
        UpdateDefeatAnimation(dt);
        return;
    }

    if(m_isPaused)
    {
        if(m_pauseMenu)
        {
            m_pauseMenu->Update(dt);
        }
        return;
    }

    if(AreAllPlayersDead())
    {
        m_runState = RunState::DefeatAnimating;
        m_defeatAnimationTimer = 0.0f;
        UpdateDefeatAnimation(dt);
        return;
    }

    // Camera Center = average of all alive players' positions
    sf::Vector2f avgPos(0.0f, 0.0f);
    int aliveCount = 0;

    for (std::size_t i = 0; i < m_players.size(); ++i)
    {
        auto& p = m_players[i];
        if (!p) continue;

        p->Update(dt);
    }

    // 2-Player Screen Boundary Distance Tethering
    if (m_players.size() >= 2 && m_players[0] && m_players[1] && !m_players[0]->IsDead() && !m_players[1]->IsDead())
    {
        sf::Vector2f p0Pos = m_players[0]->GetPosition();
        sf::Vector2f p1Pos = m_players[1]->GetPosition();
        sf::Vector2f diff = p1Pos - p0Pos;

        constexpr float MaxXDist = 650.0f;
        constexpr float MaxYDist = 360.0f;

        float normX = diff.x / MaxXDist;
        float normY = diff.y / MaxYDist;
        float ellipseDistSq = normX * normX + normY * normY;

        if (ellipseDistSq > 1.0f)
        {
            float scale = 1.0f / std::sqrt(ellipseDistSq);
            sf::Vector2f clampedDiff(diff.x * scale, diff.y * scale);
            sf::Vector2f midpoint = (p0Pos + p1Pos) * 0.5f;

            m_players[0]->SetPosition(midpoint - clampedDiff * 0.5f);
            m_players[1]->SetPosition(midpoint + clampedDiff * 0.5f);
        }
    }

    for (std::size_t i = 0; i < m_players.size(); ++i)
    {
        auto& p = m_players[i];
        if (!p) continue;

        if (!p->IsDead())
        {
            avgPos += p->GetPosition();
            aliveCount++;
        }

        p->GetWeaponInventory().Update(dt, m_projectileManager, m_context.atlas, *p, m_enemyPool);

        if (i < m_playerHUDs.size() && m_playerHUDs[i])
        {
            m_playerHUDs[i]->Update(dt);
        }

        if (i < m_pulsePets.size() && m_pulsePets[i])
        {
            m_pulsePets[i]->Update(dt, *p, m_enemyPool, &m_damageNumbers, &m_experienceGems);
        }
    }

    if (aliveCount > 0)
    {
        m_cameraCenter = avgPos / static_cast<float>(aliveCount);
    }
    ApplyCameraToView();

    UpdateStageTimer(dt);
    if(m_runState != RunState::Playing)
    {
        return;
    }

    // DEBUG MODE: Set to true to disable enemy spawning/movement and spawn static dummies for testing
    constexpr bool DebugStaticTargetsMode = false;

    if (!DebugStaticTargetsMode) {
        UpdateStageSpawner(dt);
        UpdateStageEvents(dt);
        m_enemyPool.Update(dt, m_cameraCenter);
    } else {
        // Disabled dummy enemy spawns for clean tuning testing
    }

    m_projectileManager.SetViewBounds(GetViewBounds());
    m_projectileManager.Update(dt);

    // Projectile-enemy collision resolution
    std::vector<CollisionTarget> collisionTargets;
    const auto& activeEnemies = m_enemyPool.GetActiveEnemies();
    for(EnemyBase* enemy : activeEnemies)
    {
        if(enemy && enemy->IsAlive())
        {
            float radius = enemy->GetCollisionRadius();
            const sf::Vector2f collisionCenter = enemy->GetCollisionCenter();
            sf::FloatRect enemyBounds(
                collisionCenter.x - radius,
                collisionCenter.y - radius,
                radius * 2.0f,
                radius * 2.0f
            );
            collisionTargets.push_back({ enemyBounds, static_cast<void*>(enemy) });
        }
    }

    Player* primaryPlayer = GetFirstAlivePlayer();

    auto hits = m_projectileManager.CheckCollisions(collisionTargets);
    for(auto& hit : hits)
    {
        EnemyBase* enemy = static_cast<EnemyBase*>(hit.second);
        Projectile* proj = hit.first;
        if(enemy && proj && primaryPlayer)
        {
            sf::Vector2f originalPos = enemy->GetPosition();
            const sf::Vector2f collisionCenter = enemy->GetCollisionCenter();

            sf::Vector2f diff = collisionCenter - primaryPlayer->GetPosition();
            float len = std::sqrt(diff.x * diff.x + diff.y * diff.y);
            sf::Vector2f knockbackDir = (len > 0.0f) ? (diff / len) : sf::Vector2f(1.0f, 0.0f);
            const float damage = proj->GetPower();
            const bool killed = enemy->TakeDamage(damage, knockbackDir);
            m_damageNumbers.Spawn(damage, originalPos - sf::Vector2f(0.0f, enemy->GetCollisionRadius()));

            // Audio: coalesced enemy hit/death SFX (spatial, low priority)
            if(killed)
            {
                m_context.audio.PlaySfxAt(SfxID::EnemyDeath, originalPos, 1.0f, AudioPriority::Medium);
                m_experienceGems.Spawn(originalPos, enemy->GetExpYield() * GetStageXpBonus());
                if(m_bossEnemies.erase(enemy) > 0 && m_treasureChests)
                {
                    PlaySoundOptions chestOpts;
                    chestOpts.priority = AudioPriority::High;
                    m_context.audio.PlaySfx(SfxID::ChestOpen, chestOpts);
                    m_treasureChests->Spawn(originalPos);
                }
            }
            else
            {
                m_context.audio.PlaySfxAt(SfxID::EnemyHit, originalPos, 1.0f, AudioPriority::Low);
            }

            constexpr float KNOCKBACK_FORCE = 15.0f;
            if(!killed)
            {
                enemy->ApplyKnockback(knockbackDir * KNOCKBACK_FORCE);
            }
        }
    }

    m_vfxManager.Update(dt);
    m_particleManager.Update(dt);
    m_damageNumbers.Update(dt);

    if(!AreAllPlayersDead())
    {
        m_experienceGems.Update(dt, m_players);

        if(m_treasureChests)
        {
            for (const auto& p : m_players)
            {
                if (!p || p->IsDead()) continue;
                m_treasureChests->Update(
                    dt,
                    p->GetPosition(),
                    [this](int gold) {
                        if(m_treasureRewardView)
                        {
                            m_treasureRewardView->Show(gold, m_runGold);
                        }
                        else
                        {
                            AddRunGold(gold);
                        }
                    });
                if(m_treasureRewardView && m_treasureRewardView->IsVisible())
                {
                    return;
                }
            }
        }
    }

    if (m_tileMap) {
        const std::vector<sf::FloatRect> obstacles =
            m_tileMap->GetEnemyCollisionRectsInArea(GetViewBounds());
        m_enemyPool.ResolveObstacleCollisions(obstacles);
    }
    m_enemyPool.ResolveEnemyCollisions();
    ApplyEnemyContactDamage();

    if(AreAllPlayersDead())
    {
        m_runState = RunState::DefeatAnimating;
        m_defeatAnimationTimer = 0.0f;
        PlaySoundOptions deathOpts;
        deathOpts.priority = AudioPriority::Critical;
        m_context.audio.PlaySfx(SfxID::PlayerDeath, deathOpts);
        return;
    }

    // Update tuning UI to match absolute screen position
    if (m_tuningUI) {
        m_tuningUI->SetPosition(sf::Vector2f(20.0f, 100.0f));
        m_tuningUI->Update(dt);
    }

    if (m_gemTuningUI) {
        m_gemTuningUI->SetPosition(sf::Vector2f(110.0f, 100.0f));
        m_gemTuningUI->Update(dt);
    }

    if (m_testEmitter) {
        m_testEmitter->SetPosition(m_cameraCenter + sf::Vector2f(0.0f, m_testParticleConfig.emitterOffset));
        m_testEmitter->GetConfig() = m_testParticleConfig;
    }
}

void GameState::Draw(sf::RenderWindow &window) {
    window.clear(sf::Color(12, 28, 12));

    sf::Vector2u winSize = window.getSize();
    sf::FloatRect viewport = MathUtils::CalculateLetterboxViewport(static_cast<float>(winSize.x), static_cast<float>(winSize.y), ViewWidth / ViewHeight);
    m_worldView.setViewport(viewport);

    sf::View previousView = window.getView();

    window.setView(m_worldView);
    if (m_tileMap) {
        m_tileMap->Draw(window, m_worldView);
    }
    m_enemyPool.Draw(window);

    m_experienceGems.Draw(window);

    for (std::size_t i = 0; i < m_players.size(); ++i)
    {
        if (m_players[i])
        {
            m_players[i]->Draw(window);
            if (i < m_playerHUDs.size() && m_playerHUDs[i])
            {
                m_playerHUDs[i]->Draw(window);
            }
        }
    }

    m_particleManager.Draw(window);
    m_projectileManager.Draw(window);
    m_vfxManager.Draw(window);

    for (auto& pet : m_pulsePets)
    {
        if (pet) pet->Draw(window);
    }
    if(m_treasureChests)
    {
        m_treasureChests->Draw(window);
    }
    m_damageNumbers.Draw(window);

    if (m_showHitboxes) {
        DrawHitboxes(window);
    }

    // Draw 16:10 Dim Bars Overlay (1920x1080 view -> 1728x1080 play area = 96px bars)
    sf::View uiView(sf::FloatRect(0.0f, 0.0f, ViewWidth, ViewHeight));
    uiView.setViewport(viewport);
    window.setView(uiView);

    sf::RectangleShape leftDimBar(sf::Vector2f(96.0f, ViewHeight));
    leftDimBar.setFillColor(sf::Color(0, 0, 0, 170));
    leftDimBar.setPosition(0.0f, 0.0f);
    window.draw(leftDimBar);

    sf::RectangleShape rightDimBar(sf::Vector2f(96.0f, ViewHeight));
    rightDimBar.setFillColor(sf::Color(0, 0, 0, 170));
    rightDimBar.setPosition(ViewWidth - 96.0f, 0.0f);
    window.draw(rightDimBar);

    if(m_expBar)
    {
        m_expBar->Draw(window);
    }

    DrawStageTimer(window);
    if(m_runGoldDisplay)
    {
        m_runGoldDisplay->Draw(window);
    }

    const sf::Font* debugFont = m_context.fonts.GetPtr(FontID::Main);
    if (debugFont)
    {
        sf::Text enemyHudText;
        enemyHudText.setFont(*debugFont);
        enemyHudText.setCharacterSize(22);
        enemyHudText.setFillColor(sf::Color::Yellow);
        enemyHudText.setOutlineColor(sf::Color::Black);
        enemyHudText.setOutlineThickness(2.0f);
        enemyHudText.setPosition(110.0f, 18.0f);

        const std::string hudStr = "ENEMIES: " + std::to_string(m_enemyPool.GetActiveCount()) +
                                   " / 2000 | LEVEL-UP: " + (s_disableLevelUpUI ? "DISABLED (Press V to toggle)" : "ENABLED (Press V to toggle)");
        enemyHudText.setString(hudStr);
        window.draw(enemyHudText);
    }

    window.setView(previousView);

    if (m_tuningUI) {
        // Draw in a fixed UI view so it is not affected by camera zoom
        sf::View tuningUiView(sf::FloatRect(0.0f, 0.0f, ViewWidth, ViewHeight));
        window.setView(tuningUiView);
        m_tuningUI->Draw(window);
        window.setView(previousView);
    }

    if (m_gemTuningUI) {
        sf::View tuningUiView(sf::FloatRect(0.0f, 0.0f, ViewWidth, ViewHeight));
        window.setView(tuningUiView);
        m_gemTuningUI->Draw(window);
        window.setView(previousView);
    }

    if(m_isPaused && m_pauseMenu)
    {
        sf::View pauseView(sf::FloatRect(0.0f, 0.0f, Core::VIRTUAL_WIDTH, Core::VIRTUAL_HEIGHT));
        pauseView.setViewport(previousView.getViewport());
        window.setView(pauseView);

        sf::Vector2f worldSize(0.0f, 0.0f);
        if(m_tileMap)
        {
            worldSize = m_tileMap->GetWorldSize();
        }
        Player* p0 = GetFirstAlivePlayer();
        if(p0)
        {
            m_pauseMenu->SetPlayerPosition(p0->GetPosition(), worldSize);
        }
        m_pauseMenu->Draw(window);
        window.setView(previousView);
    }

    if(IsGameOverVisible() && m_gameOverView)
    {
        sf::View resultView(sf::FloatRect(0.0f, 0.0f, ViewWidth, ViewHeight));
        resultView.setViewport(viewport);
        window.setView(resultView);
        m_gameOverView->Draw(window);
        window.setView(previousView);
    }

    if(m_treasureRewardView && m_treasureRewardView->IsVisible())
    {
        sf::View treasureView(sf::FloatRect(0.0f, 0.0f, ViewWidth, ViewHeight));
        treasureView.setViewport(viewport);
        window.setView(treasureView);
        m_treasureRewardView->Draw(window);
        window.setView(previousView);
    }

    if(m_levelUpController.IsSessionActive() && m_levelUpView)
    {
        sf::View levelUpUiView(sf::FloatRect(0.0f, 0.0f, ViewWidth, ViewHeight));
        levelUpUiView.setViewport(viewport);
        window.setView(levelUpUiView);
        m_levelUpView->Draw(window);
        window.setView(previousView);
    }
}

void GameState::LoadStage(int stageNumber) {
    m_runState = RunState::Playing;
    m_defeatAnimationTimer = 0.0f;
    m_isPaused = false;
    m_currentStage = stageNumber;
    m_runGold = 0;
    m_runGoldBanked = false;
    if(m_runGoldDisplay)
    {
        m_runGoldDisplay->SetGold(m_runGold);
    }
    m_enemyPool.Clear();
    m_bossEnemies.clear();
    if(m_treasureChests)
    {
        m_treasureChests->Clear();
    }
    m_experienceGems.Clear();
    m_damageNumbers.Clear();

    m_tileMap = m_mapManager.GetMap(stageNumber);
    if (!m_tileMap) {
        std::cerr << "GameState: Map for stage " << stageNumber << " was not preloaded!" << std::endl;
    }

    if (!m_enemyDatabase.LoadFromFile(GetStageEnemyPath(stageNumber))) {
        std::cerr << "Failed to load " << GetStageName(stageNumber) << " enemies" << std::endl;
    }

    if (m_tileMap) {
        sf::Vector2f worldSize = m_tileMap->GetWorldSize();
        m_cameraCenter = sf::Vector2f(worldSize.x / 2.0f, worldSize.y / 2.0f);
    } else {
        m_cameraCenter = sf::Vector2f(400.0f, 300.0f);
    }

    for (std::size_t idx = 0; idx < m_players.size(); ++idx)
    {
        auto& p = m_players[idx];
        if (p) {
            if(p->IsDead())
            {
                p->Revive();
            }
            p->SetPosition(m_cameraCenter + sf::Vector2f(idx * 50.0f, 0.0f));
        }
    }

    ApplyCameraToView();

    const std::string stageKey = GetStageWaveKey(stageNumber);
    m_activeStageWaves = m_stageWaveData.GetStageWaves(stageKey);
    m_activeStageInfo = m_stageWaveData.GetStageInfo(stageKey);

    if(m_pauseMenu)
    {
        m_pauseMenu->SetStageName(m_activeStageInfo ? m_activeStageInfo->stageName : GetStageName(stageNumber));
    }

    ResetStageSpawner();
    UpdateStageTimerText();

    std::cout << "Loaded stage " << stageNumber << ": " << GetStageName(stageNumber) << " with "
              << m_enemyPool.GetActiveCount() << " opening wave enemies" << std::endl;
}

void GameState::ResetStageSpawner()
{
    m_currentWave = nullptr;
    m_stageElapsed = 0.0f;
    m_waveSpawnTimer = 0.0f;
    m_waveSpawnCursor = 0;
    m_spawnedBossWaveMinutes.clear();
    m_runtimeStageEvents.clear();
    UpdateStageTimerText();

    if(!m_activeStageWaves || m_activeStageWaves->empty())
    {
        return;
    }

    m_currentWave = GetCurrentStageWave();
    if(!m_currentWave)
    {
        return;
    }

    ResetStageEventsForCurrentWave();

    const int requestedOpeningSpawns =
        m_currentWave->startingSpawns > 0 ? m_currentWave->startingSpawns : m_currentWave->minimum;
    const int openingSpawns = std::min(requestedOpeningSpawns, MaxOpeningSpawns);

    for(int i = 0; i < openingSpawns; ++i)
    {
        if(m_currentWave->enemies.empty())
        {
            break;
        }

        const std::string& enemyId = m_currentWave->enemies[static_cast<std::size_t>(i) % m_currentWave->enemies.size()];
        SpawnWaveEnemy(enemyId);
    }

    SpawnWaveBosses(*m_currentWave);
}

void GameState::UpdateStageSpawner(float dt)
{
    if(!m_activeStageWaves || m_activeStageWaves->empty())
    {
        return;
    }

    const StageWaveDefinition* nextWave = GetCurrentStageWave();
    if(!nextWave)
    {
        return;
    }

    if(nextWave != m_currentWave)
    {
        m_currentWave = nextWave;
        m_waveSpawnTimer = 0.0f;
        ResetStageEventsForCurrentWave();
        SpawnWaveBosses(*m_currentWave);
    }

    if(m_currentWave->enemies.empty())
    {
        return;
    }

    const std::size_t targetCount =
        std::min<std::size_t>(static_cast<std::size_t>(std::max(0, m_currentWave->minimum)), MaxRuntimeEnemies);
    const std::size_t activeCount = m_enemyPool.GetActiveCount();
    if(activeCount >= targetCount)
    {
        m_waveSpawnTimer = 0.0f;
        return;
    }

    m_waveSpawnTimer += dt;
    const float spawnInterval =
        std::max(MinWaveSpawnIntervalSeconds, m_currentWave->frequencyMs / 1000.0f);
    if(m_waveSpawnTimer < spawnInterval)
    {
        return;
    }

    m_waveSpawnTimer = 0.0f;
    const std::size_t missingCount = targetCount - activeCount;
    const int spawnCount = static_cast<int>(std::min<std::size_t>(missingCount, MaxSpawnBatchPerTick));
    for(int i = 0; i < spawnCount; ++i)
    {
        const std::string& enemyId =
            m_currentWave->enemies[static_cast<std::size_t>(m_waveSpawnCursor) % m_currentWave->enemies.size()];
        SpawnWaveEnemy(enemyId);
    }
}

void GameState::ResetStageEventsForCurrentWave()
{
    m_runtimeStageEvents.clear();
    if(!m_currentWave)
    {
        return;
    }

    for(const StageWaveEvent& definition : m_currentWave->events)
    {
        if(!IsSupportedStageEvent(definition.eventType))
        {
            continue;
        }

        std::string requestedEnemyId = definition.moreY;
        if(requestedEnemyId.empty())
        {
            if(definition.eventType == "BAT_SWARM")
            {
                requestedEnemyId = "BAT1";
            }
            else if(definition.eventType == "FLOWER_WALL")
            {
                requestedEnemyId = "XLFLOWER";
            }
            else if(definition.eventType == "MEDUSA_WALL")
            {
                requestedEnemyId = "MEDUSA1";
            }
            else if(!m_currentWave->enemies.empty())
            {
                requestedEnemyId = m_currentWave->enemies.front();
            }
        }

        if(requestedEnemyId.empty())
        {
            continue;
        }

        const std::string resolvedEnemyId = ResolveSpawnEnemyId(requestedEnemyId);
        if(resolvedEnemyId.empty())
        {
            continue;
        }

        RuntimeStageEvent runtimeEvent;
        runtimeEvent.definition = definition;
        runtimeEvent.enemyId = resolvedEnemyId;
        runtimeEvent.nextTriggerMs = std::max(0.0f, definition.delayMs);
        runtimeEvent.spawnCount = std::clamp(
            definition.moreX > 0 ? definition.moreX : DefaultEventSpawnCount,
            1,
            MaxEventSpawnsPerTrigger);
        m_runtimeStageEvents.push_back(std::move(runtimeEvent));
    }
}

void GameState::UpdateStageEvents(float dt)
{
    if(m_runtimeStageEvents.empty())
    {
        return;
    }

    const float elapsedSeconds = std::max(0.0f, dt);
    const float elapsedMs = elapsedSeconds * 1000.0f;
    for(RuntimeStageEvent& runtimeEvent : m_runtimeStageEvents)
    {
        runtimeEvent.elapsedMs += elapsedMs;
        runtimeEvent.spawnCooldown = std::max(0.0f, runtimeEvent.spawnCooldown - elapsedSeconds);

        const int repeatCount = std::max(1, runtimeEvent.definition.repeat);
        const float repeatIntervalMs = GetStageEventRepeatIntervalMs(runtimeEvent.definition);
        while(runtimeEvent.triggerCount < repeatCount &&
              runtimeEvent.elapsedMs >= runtimeEvent.nextTriggerMs)
        {
            if(RollStageEventChance(runtimeEvent.definition.chance))
            {
                runtimeEvent.pendingSpawns = std::min(
                    MaxQueuedEventSpawns,
                    runtimeEvent.pendingSpawns + runtimeEvent.spawnCount);
            }

            ++runtimeEvent.triggerCount;
            runtimeEvent.nextTriggerMs += repeatIntervalMs;
        }
    }

    int spawnBudget = MaxEventSpawnBatchPerTick;
    std::size_t activeEnemyCount = m_enemyPool.GetActiveCount();
    for(RuntimeStageEvent& runtimeEvent : m_runtimeStageEvents)
    {
        if(spawnBudget <= 0 || activeEnemyCount >= MaxRuntimeEnemies)
        {
            break;
        }
        if(runtimeEvent.pendingSpawns <= 0 || runtimeEvent.spawnCooldown > 0.0f)
        {
            continue;
        }

        const int availableSlots = static_cast<int>(MaxRuntimeEnemies - activeEnemyCount);
        const int spawnCount = std::min(
            runtimeEvent.pendingSpawns,
            std::min(spawnBudget, availableSlots));
        for(int i = 0; i < spawnCount; ++i)
        {
            const sf::Vector2f spawnPosition =
                GetStageEventSpawnPosition(runtimeEvent, runtimeEvent.spawnSequence);
            if(SpawnEnemyAt(runtimeEvent.enemyId, spawnPosition))
            {
                ++activeEnemyCount;
            }

            ++runtimeEvent.spawnSequence;
            --runtimeEvent.pendingSpawns;
            --spawnBudget;
            ++m_waveSpawnCursor;
        }

        runtimeEvent.spawnCooldown = EventSpawnCooldownSeconds;
    }
}

EnemyBase* GameState::SpawnWaveEnemy(const std::string& enemyId)
{
    const sf::Vector2f spawnPosition = GetWaveSpawnPosition(m_waveSpawnCursor);
    EnemyBase* enemy = SpawnEnemyAt(enemyId, spawnPosition);
    ++m_waveSpawnCursor;
    return enemy;
}

EnemyBase* GameState::SpawnEnemyAt(const std::string& enemyId, const sf::Vector2f& position)
{
    const std::string resolvedEnemyId = ResolveSpawnEnemyId(enemyId);
    if(resolvedEnemyId.empty())
    {
        return nullptr;
    }

    const EnemyDefinition* definition = m_enemyDatabase.GetDefinition(resolvedEnemyId);
    if(!definition)
    {
        return nullptr;
    }

    return m_enemyPool.Acquire(
        resolvedEnemyId,
        position,
        ApplyStageEnemyModifiers(definition->stats));
}

void GameState::SpawnWaveBosses(const StageWaveDefinition& wave)
{
    if(std::find(m_spawnedBossWaveMinutes.begin(), m_spawnedBossWaveMinutes.end(), wave.minute) !=
       m_spawnedBossWaveMinutes.end())
    {
        return;
    }

    for(const std::string& bossId : wave.bosses)
    {
        if(EnemyBase* boss = SpawnWaveEnemy(bossId))
        {
            m_bossEnemies.insert(boss);
        }
    }

    m_spawnedBossWaveMinutes.push_back(wave.minute);
}

const StageWaveDefinition* GameState::GetCurrentStageWave() const
{
    if(!m_activeStageWaves || m_activeStageWaves->empty())
    {
        return nullptr;
    }

    const int elapsedMinute = static_cast<int>(m_stageElapsed / 60.0f);
    const StageWaveDefinition* selectedWave = nullptr;
    for(const StageWaveDefinition& wave : *m_activeStageWaves)
    {
        if(wave.minute <= elapsedMinute)
        {
            selectedWave = &wave;
        }
    }

    return selectedWave ? selectedWave : &m_activeStageWaves->front();
}

sf::Vector2f GameState::GetWaveSpawnPosition(int spawnIndex) const
{
    const sf::FloatRect bounds = GetViewBounds();
    const float padding = 80.0f;
    const float xT = static_cast<float>((spawnIndex * 97) % 1000) / 1000.0f;
    const float yT = static_cast<float>((spawnIndex * 193) % 1000) / 1000.0f;
    const std::string spawnType = m_currentWave ? m_currentWave->spawnType : "STANDARD";
    const int side =
        spawnType == "HORIZONTAL" ? (spawnIndex % 2 == 0 ? 3 : 1)
        : spawnType == "VERTICAL" ? (spawnIndex % 2 == 0 ? 0 : 2)
        : spawnIndex % 4;

    switch(side)
    {
        case 0:
            return sf::Vector2f(bounds.left + bounds.width * xT, bounds.top - padding);
        case 1:
            return sf::Vector2f(bounds.left + bounds.width + padding, bounds.top + bounds.height * yT);
        case 2:
            return sf::Vector2f(bounds.left + bounds.width * xT, bounds.top + bounds.height + padding);
        default:
            return sf::Vector2f(bounds.left - padding, bounds.top + bounds.height * yT);
    }
}

sf::Vector2f GameState::GetStageEventSpawnPosition(const RuntimeStageEvent& event, int spawnIndex) const
{
    const sf::FloatRect bounds = GetViewBounds();
    const int patternCount = std::max(1, event.spawnCount);
    const int patternIndex = spawnIndex % patternCount;
    const int patternCycle = spawnIndex / patternCount;

    if(event.definition.eventType == "FLOWER_WALL")
    {
        const float rotation = (patternCycle % 2 == 0) ? 0.0f : Pi / static_cast<float>(patternCount);
        const float angle =
            (2.0f * Pi * static_cast<float>(patternIndex) / static_cast<float>(patternCount)) + rotation;
        const sf::Vector2f center(
            bounds.left + bounds.width * 0.5f,
            bounds.top + bounds.height * 0.5f);
        return sf::Vector2f(
            center.x + std::cos(angle) * bounds.width * 0.56f,
            center.y + std::sin(angle) * bounds.height * 0.56f);
    }

    if(event.definition.eventType == "MEDUSA_WALL")
    {
        const float padding = 80.0f;
        const float y = bounds.top + bounds.height *
            ((static_cast<float>(patternIndex) + 0.5f) / static_cast<float>(patternCount));
        const float x = patternCycle % 2 == 0
            ? bounds.left - padding
            : bounds.left + bounds.width + padding;
        return sf::Vector2f(x, y);
    }

    return GetWaveSpawnPosition(m_waveSpawnCursor);
}

bool GameState::RollStageEventChance(float chance)
{
    const float clampedChance = std::clamp(chance, 0.0f, 100.0f);
    if(clampedChance <= 0.0f)
    {
        return false;
    }
    if(clampedChance >= 100.0f)
    {
        return true;
    }

    std::uniform_real_distribution<float> distribution(0.0f, 100.0f);
    return distribution(m_stageEventRng) < clampedChance;
}

std::string GameState::ResolveSpawnEnemyId(const std::string& requestedId) const
{
    if(m_enemyDatabase.HasDefinition(requestedId))
    {
        return requestedId;
    }

    if((requestedId == "BAT4" || requestedId == "BAT5") && m_enemyDatabase.HasDefinition("XLBAT")) return "XLBAT";
    if((requestedId == "BAT4" || requestedId == "BAT5" || StartsWith(requestedId, "XLBAT")) &&
       m_enemyDatabase.HasDefinition("BUER")) return "BUER";
    if(StartsWith(requestedId, "BAT") && m_enemyDatabase.HasDefinition("BAT1")) return "BAT1";
    if(StartsWith(requestedId, "BOSS_WEREWOLF") && m_enemyDatabase.HasDefinition("WEREWOLF")) return "WEREWOLF";
    if(StartsWith(requestedId, "BOSS_XLDEATH") && m_enemyDatabase.HasDefinition("XLMANTIS")) return "XLMANTIS";
    if(StartsWith(requestedId, "BOSS_XLDEATH") && m_enemyDatabase.HasDefinition("XLBAT")) return "XLBAT";
    if(StartsWith(requestedId, "BOSS_XLFLOWER") && m_enemyDatabase.HasDefinition("XLFLOWER")) return "XLFLOWER";
    if(StartsWith(requestedId, "BOSS_XLMANTIS") && m_enemyDatabase.HasDefinition("XLMANTIS")) return "XLMANTIS";
    if(StartsWith(requestedId, "BOSS_XLMUMMY") && m_enemyDatabase.HasDefinition("XLMUMMY")) return "XLMUMMY";
    if(StartsWith(requestedId, "BOSS_XLMUMMY") && m_enemyDatabase.HasDefinition("XLMANTIS")) return "XLMANTIS";
    if(StartsWith(requestedId, "FLOWER") && m_enemyDatabase.HasDefinition("XLFLOWER")) return "XLFLOWER";
    if(StartsWith(requestedId, "MUDMAN") && m_enemyDatabase.HasDefinition("MUDMAN1")) return "MUDMAN1";
    if(StartsWith(requestedId, "BOSS_MUD") && m_enemyDatabase.HasDefinition("MUD")) return "MUD";
    if(StartsWith(requestedId, "MUD") && m_enemyDatabase.HasDefinition("MUD")) return "MUD";
    if(StartsWith(requestedId, "GHOST") && m_enemyDatabase.HasDefinition("GHOST")) return "GHOST";
    if(StartsWith(requestedId, "SKELETON") && m_enemyDatabase.HasDefinition("SKELETON")) return "SKELETON";
    if(StartsWith(requestedId, "ZOMBIE") && m_enemyDatabase.HasDefinition("ZOMBIE")) return "ZOMBIE";
    if(StartsWith(requestedId, "BOSS_ECTO") && m_enemyDatabase.HasDefinition("ECTO1")) return "ECTO1";
    if(StartsWith(requestedId, "ECTO") && m_enemyDatabase.HasDefinition("ECTO1")) return "ECTO1";
    if(StartsWith(requestedId, "BOSS_MEDUSA") && m_enemyDatabase.HasDefinition("MEDUSA1")) return "MEDUSA1";
    if(StartsWith(requestedId, "MEDUSA") && m_enemyDatabase.HasDefinition("MEDUSA1")) return "MEDUSA1";
    if(StartsWith(requestedId, "MUMMY") && m_enemyDatabase.HasDefinition("MUMMY")) return "MUMMY";
    if(StartsWith(requestedId, "BOSS_MERMAN") && m_enemyDatabase.HasDefinition("FISHMAN_1")) return "FISHMAN_1";
    if(StartsWith(requestedId, "MERMA") && m_enemyDatabase.HasDefinition("FISHMAN_1")) return "FISHMAN_1";
    if(StartsWith(requestedId, "MILK") && m_enemyDatabase.HasDefinition("MILK")) return "MILK";
    if(StartsWith(requestedId, "FISHMAN") && m_enemyDatabase.HasDefinition("FISHMAN_1")) return "FISHMAN_1";
    if(StartsWith(requestedId, "LIZARD1") && m_enemyDatabase.HasDefinition("LIZARD1_2")) return "LIZARD1_2";
    if(StartsWith(requestedId, "LIZARD2") && m_enemyDatabase.HasDefinition("LIZARD2_3")) return "LIZARD2_3";
    if(StartsWith(requestedId, "BOSS_PILE") && m_enemyDatabase.HasDefinition("PILE3")) return "PILE3";
    if(StartsWith(requestedId, "PILE") && m_enemyDatabase.HasDefinition("PILE1")) return "PILE1";
    if(StartsWith(requestedId, "DULL") && m_enemyDatabase.HasDefinition("DULL0")) return "DULL0";
    if(StartsWith(requestedId, "BOSS_WITCH") && m_enemyDatabase.HasDefinition("WITCH2")) return "WITCH2";
    if(StartsWith(requestedId, "WITCH") && m_enemyDatabase.HasDefinition("WITCH1")) return "WITCH1";
    if(StartsWith(requestedId, "BOSS_BUER") && m_enemyDatabase.HasDefinition("BUER")) return "BUER";
    if(StartsWith(requestedId, "BUER") && m_enemyDatabase.HasDefinition("BUER")) return "BUER";
    if(StartsWith(requestedId, "SKULL") && m_enemyDatabase.HasDefinition("SKULLNOAURA")) return "SKULLNOAURA";
    if(StartsWith(requestedId, "SKULL") && m_enemyDatabase.HasDefinition("SKELETON")) return "SKELETON";
    if(StartsWith(requestedId, "SKELENIN") && m_enemyDatabase.HasDefinition("SKELENIN1")) return "SKELENIN1";
    if(StartsWith(requestedId, "JELLYFISH") && m_enemyDatabase.HasDefinition("JELLYFISH")) return "JELLYFISH";
    if(StartsWith(requestedId, "GOLEM") && m_enemyDatabase.HasDefinition("GOLEM1")) return "GOLEM1";
    if(StartsWith(requestedId, "BOSS_MIGNO") && m_enemyDatabase.HasDefinition("MIGNO1_5")) return "MIGNO1_5";
    if(StartsWith(requestedId, "MIGNO") && m_enemyDatabase.HasDefinition("MIGNO1_5")) return "MIGNO1_5";
    if(StartsWith(requestedId, "BOSS_ARMOR") && m_enemyDatabase.HasDefinition("ARMOR_6")) return "ARMOR_6";
    if(StartsWith(requestedId, "XLBAT") && m_enemyDatabase.HasDefinition("XLBAT")) return "XLBAT";
    if(StartsWith(requestedId, "XLMANTIS") && m_enemyDatabase.HasDefinition("XLMANTIS")) return "XLMANTIS";
    if(StartsWith(requestedId, "XLFLOWER") && m_enemyDatabase.HasDefinition("XLFLOWER")) return "XLFLOWER";
    if(StartsWith(requestedId, "BOSS_XLMEDUSA") && m_enemyDatabase.HasDefinition("XLMEDUSA")) return "XLMEDUSA";
    if(StartsWith(requestedId, "XLMEDUSA") && m_enemyDatabase.HasDefinition("XLMEDUSA")) return "XLMEDUSA";
    if(StartsWith(requestedId, "XLMUMMY") && m_enemyDatabase.HasDefinition("XLMUMMY")) return "XLMUMMY";
    if(StartsWith(requestedId, "XLMUMMY") && m_enemyDatabase.HasDefinition("XLMANTIS")) return "XLMANTIS";
    if(StartsWith(requestedId, "BOSS_XLTRITON") && m_enemyDatabase.HasDefinition("XLTRITON")) return "XLTRITON";
    if(StartsWith(requestedId, "XLTRITON") && m_enemyDatabase.HasDefinition("XLTRITON")) return "XLTRITON";
    if(StartsWith(requestedId, "BOSS_XLCOCKATRICE") && m_enemyDatabase.HasDefinition("XLCOCKATRICE")) return "XLCOCKATRICE";
    if(StartsWith(requestedId, "XLCOCKATRICE") && m_enemyDatabase.HasDefinition("XLCOCKATRICE")) return "XLCOCKATRICE";
    if(StartsWith(requestedId, "BOSS_XLGOLEM") && m_enemyDatabase.HasDefinition("XLGOLEM1")) return "XLGOLEM1";
    if(StartsWith(requestedId, "XLGOLEM") && m_enemyDatabase.HasDefinition("XLGOLEM1")) return "XLGOLEM1";
    if(StartsWith(requestedId, "BOSS_XLARMOR") && m_enemyDatabase.HasDefinition("XLARMOR_SWORD")) return "XLARMOR_SWORD";
    if(StartsWith(requestedId, "XLARMOR") && m_enemyDatabase.HasDefinition("XLARMOR_SWORD")) return "XLARMOR_SWORD";

    std::cerr << "GameState: missing enemy definition for wave id " << requestedId << std::endl;
    return "";
}

void GameState::UpdateStageTimer(float dt)
{
    m_stageElapsed += dt * GetStageClockSpeed();

    const int timeLimitSeconds = GetStageTimeLimitSeconds();
    if(timeLimitSeconds > 0 && m_stageElapsed >= static_cast<float>(timeLimitSeconds))
    {
        m_stageElapsed = static_cast<float>(timeLimitSeconds);
        UpdateStageTimerText();
        FinishRun(RunState::Completed);
        return;
    }

    UpdateStageTimerText();
}

void GameState::UpdateStageTimerText()
{
    if(m_stageTimerText.getString().isEmpty() && !m_stageTimerText.getFont())
    {
        return;
    }

    const int timeLimitSeconds = GetStageTimeLimitSeconds();
    const int displaySeconds = static_cast<int>(m_stageElapsed);
    const std::string timerString = FormatStageTime(displaySeconds);
    m_stageTimerText.setString(timerString);
    m_stageTimerShadowText.setString(timerString);

    std::string infoString = m_activeStageInfo ? m_activeStageInfo->stageName : GetStageName(m_currentStage);
    if(timeLimitSeconds > 0)
    {
        infoString += " / " + FormatStageTime(timeLimitSeconds);
    }
    m_stageInfoText.setString(infoString);

    const sf::FloatRect timerBounds = m_stageTimerText.getLocalBounds();
    m_stageTimerText.setOrigin(timerBounds.left + timerBounds.width / 2.0f, timerBounds.top);
    m_stageTimerText.setPosition(ViewWidth / 2.0f, 24.0f);

    m_stageTimerShadowText.setOrigin(m_stageTimerText.getOrigin());
    m_stageTimerShadowText.setPosition(m_stageTimerText.getPosition() + sf::Vector2f(3.0f, 3.0f));

    const sf::FloatRect infoBounds = m_stageInfoText.getLocalBounds();
    m_stageInfoText.setOrigin(infoBounds.left + infoBounds.width / 2.0f, infoBounds.top);
    m_stageInfoText.setPosition(ViewWidth / 2.0f, 74.0f);

    const float backingWidth = std::max(timerBounds.width, infoBounds.width) + 58.0f;
    m_stageTimerBacking.setSize(sf::Vector2f(backingWidth, 82.0f));
    m_stageTimerBacking.setOrigin(backingWidth / 2.0f, 0.0f);
    m_stageTimerBacking.setPosition(ViewWidth / 2.0f, 13.0f);
}

void GameState::DrawStageTimer(sf::RenderTarget& target) const
{
    if(!m_stageTimerText.getFont())
    {
        return;
    }

    target.draw(m_stageTimerBacking);
    target.draw(m_stageTimerShadowText);
    target.draw(m_stageTimerText);
    target.draw(m_stageInfoText);
}

std::string GameState::FormatStageTime(int totalSeconds) const
{
    if(totalSeconds < 0)
    {
        totalSeconds = 0;
    }

    const int minutes = totalSeconds / 60;
    const int seconds = totalSeconds % 60;
    return std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds);
}

float GameState::GetStageClockSpeed() const
{
    return m_activeStageInfo ? std::max(0.0f, m_activeStageInfo->clockSpeed) : 1.0f;
}

int GameState::GetStageTimeLimitSeconds() const
{
    return m_activeStageInfo ? m_activeStageInfo->timeLimitSeconds : 1800;
}

void GameState::FinishRun(RunState outcome)
{
    if(outcome != RunState::Completed && outcome != RunState::Defeated)
    {
        return;
    }
    if(IsGameOverVisible())
    {
        return;
    }

    if(outcome == RunState::Completed)
    {
        const int timeLimitSeconds = GetStageTimeLimitSeconds();
        if(timeLimitSeconds > 0)
        {
            m_stageElapsed = static_cast<float>(timeLimitSeconds);
        }
    }

    m_runState = outcome;
    m_defeatAnimationTimer = 0.0f;
    m_isPaused = false;
    UpdateStageTimerText();

    // Switch to game over BGM
    m_context.audio.PlayMusic(BgmID::GameOver, false, 1.0f);

    if(m_gameOverView)
    {
        m_gameOverView->Show();
    }
}

void GameState::UpdateDefeatAnimation(float dt)
{
    if(m_runState != RunState::DefeatAnimating)
    {
        return;
    }

    const float elapsed = std::max(0.0f, dt);
    for (auto& p : m_players)
    {
        if (p) p->Update(elapsed);
    }

    m_defeatAnimationTimer += elapsed;
    if(m_defeatAnimationTimer >= DefeatAnimationSeconds)
    {
        FinishRun(RunState::Defeated);
    }
}

bool GameState::IsGameOverVisible() const
{
    return m_runState == RunState::Completed || m_runState == RunState::Defeated;
}

float GameState::GetStageXpBonus() const
{
    return m_activeStageInfo ? std::max(0.0f, m_activeStageInfo->xpBonus) : 1.0f;
}

EnemyStats GameState::ApplyStageEnemyModifiers(const EnemyStats& stats) const
{
    EnemyStats adjustedStats = stats;
    if(!m_activeStageInfo)
    {
        return adjustedStats;
    }

    adjustedStats.speed *= std::max(0.0f, m_activeStageInfo->enemySpeedMultiplier);
    adjustedStats.maxHealth *= std::max(0.0f, m_activeStageInfo->enemyHealthMultiplier);
    return adjustedStats;
}

void GameState::TogglePause()
{
    m_isPaused = !m_isPaused;
}

void GameState::ReturnToMainMenu()
{
    m_isPaused = false;
    BankRunGold();
    m_context.stateManager.ChangeStateWithTransition(
        std::make_unique<MainMenuState>(m_context, m_mapManager),
        0.35f,
        sf::Color::Black);
}

void GameState::AddRunGold(int amount)
{
    if(amount <= 0 || m_runGoldBanked)
    {
        return;
    }

    m_runGold += amount;
    if(m_runGoldDisplay)
    {
        m_runGoldDisplay->SetGold(m_runGold);
    }
}

void GameState::BankRunGold()
{
    if(m_runGoldBanked)
    {
        return;
    }

    m_runGoldBanked = true;
    if(m_runGold <= 0)
    {
        return;
    }

    m_context.progressionData.AddGold(m_runGold);
    m_context.progressionData.Save();
}

void GameState::ApplyEnemyContactDamage()
{
    if(AreAllPlayersDead())
    {
        return;
    }

    for (auto& player : m_players)
    {
        if(!player || player->IsDead()) continue;

        const sf::Vector2f playerPosition = player->GetPosition();
        const float playerRadius = player->GetCollisionRadius();
        float contactDamage = 0.0f;

        for(EnemyBase* enemy : m_enemyPool.GetActiveEnemies())
        {
            if(!enemy || !enemy->IsAlive()) continue;

            if(Collision::CircleIntersectsCircle(
                   playerPosition,
                   playerRadius,
                   enemy->GetCollisionCenter(),
                   enemy->GetCollisionRadius()) &&
               enemy->GetDamage() > contactDamage)
            {
                contactDamage = enemy->GetDamage();
            }
        }

        if(contactDamage > 0.0f)
        {
            player->TakeDamage(contactDamage);
            m_context.audio.PlaySfxAt(SfxID::PlayerHit, player->GetPosition(), 1.0f, AudioPriority::High);
        }
    }
}

void GameState::ApplyCameraToView() {
    m_worldView.setCenter(std::round(m_cameraCenter.x), std::round(m_cameraCenter.y));
}

sf::FloatRect GameState::GetViewBounds() const {
    const sf::Vector2f viewSize = m_worldView.getSize();
    const sf::Vector2f viewCenter = m_worldView.getCenter();
    return sf::FloatRect(viewCenter.x - viewSize.x / 2.0f, viewCenter.y - viewSize.y / 2.0f,
                         viewSize.x, viewSize.y);
}

void GameState::DrawHitboxes(sf::RenderTarget &target) {
    sf::RectangleShape obstacleHitbox;
    obstacleHitbox.setFillColor(sf::Color(255, 40, 40, 35));
    obstacleHitbox.setOutlineColor(sf::Color(255, 40, 40, 220));
    obstacleHitbox.setOutlineThickness(1.0f);

    if (m_tileMap) {
        for (const sf::FloatRect &rect : m_tileMap->GetEnemyCollisionRectsInArea(GetViewBounds())) {
            obstacleHitbox.setPosition(rect.left, rect.top);
            obstacleHitbox.setSize(sf::Vector2f(rect.width, rect.height));
            target.draw(obstacleHitbox);
        }
    }

    m_enemyPool.DrawDebug(target);

    for (const auto& player : m_players)
    {
        if(player && !player->IsDead())
        {
            const float radius = player->GetCollisionRadius();
            sf::CircleShape playerHitbox(radius);
            playerHitbox.setOrigin(radius, radius);
            playerHitbox.setPosition(player->GetPosition());
            playerHitbox.setFillColor(sf::Color(80, 170, 255, 35));
            playerHitbox.setOutlineColor(sf::Color(80, 170, 255, 220));
            playerHitbox.setOutlineThickness(1.0f);
            target.draw(playerHitbox);
        }
    }
}

bool GameState::AreAllPlayersDead() const
{
    if (m_players.empty()) return true;
    for (const auto& p : m_players)
    {
        if (p && !p->IsDead()) return false;
    }
    return true;
}

Player* GameState::GetFirstAlivePlayer()
{
    for (auto& p : m_players)
    {
        if (p && !p->IsDead()) return p.get();
    }
    return m_players.empty() ? nullptr : m_players[0].get();
}

void GameState::AddSharedExperience(float amount)
{
    if (amount <= 0.0f) return;

    float growthMultiplier = 1.0f;
    if (m_sharedLevel == 20 || m_sharedLevel == 40)
    {
        growthMultiplier += 1.0f;
    }

    m_sharedExperience += amount * growthMultiplier;

    float targetExp = GetSharedTargetExperience();
    while (m_sharedExperience >= targetExp)
    {
        m_sharedExperience -= targetExp;
        m_sharedLevel++;

        if (s_disableLevelUpUI)
        {
            continue;
        }

        // Audio: Level up SFX
        PlaySoundOptions lvlOpts;
        lvlOpts.priority = AudioPriority::Critical;
        m_context.audio.PlaySfx(SfxID::LevelUpOpen, lvlOpts);

        m_levelUpController.QueueLevelUp();
        if (!m_levelUpController.IsSessionActive() && !m_players.empty())
        {
            m_levelUpController.StartNextLevelUp(m_players, m_sharedLevel, m_context.weaponData);
            if (m_levelUpView && m_levelUpController.IsSessionActive())
            {
                m_levelUpView->UpdateChoices(
                    m_levelUpController.GetCurrentChoices(),
                    m_levelUpController.GetPendingLevelUpCount(),
                    m_levelUpController.GetRerollCharges(),
                    m_levelUpController.GetSkipCharges(),
                    m_levelUpController.GetBanishCharges()
                );
            }
        }
        targetExp = GetSharedTargetExperience();
    }
}

float GameState::GetSharedTargetExperience() const
{
    int lvl = m_sharedLevel;
    if (lvl <= 1) return 5.0f;
    if (lvl < 20) return 5.0f + 10.0f * (lvl - 1);
    if (lvl == 20) return 5.0f + 10.0f * 19 + 600.0f;

    float xp20 = 5.0f + 10.0f * 19 + 600.0f;
    if (lvl < 40) return xp20 + 13.0f * (lvl - 20);
    if (lvl == 40) return (xp20 + 13.0f * 20) + 2400.0f;

    float xp40 = (xp20 + 13.0f * 20) + 2400.0f;
    return xp40 + 16.0f * (lvl - 40);
}

float GameState::GetSharedExpProgressRatio() const
{
    float target = GetSharedTargetExperience();
    if (target <= 0.0f) return 0.0f;
    float ratio = m_sharedExperience / target;
    return std::clamp(ratio, 0.0f, 1.0f);
}

void GameState::BroadcastWeaponAdd(const std::string& weaponId)
{
    for (auto& p : m_players)
    {
        if (p && !p->GetWeaponInventory().IsFull())
        {
            auto w = m_weaponFactory.Create(weaponId);
            if (w)
            {
                p->GetWeaponInventory().AddWeapon(std::move(w));
            }
        }
    }
}

void GameState::BroadcastWeaponLevelUp(const std::string& weaponId)
{
    for (auto& p : m_players)
    {
        if (p && p->GetWeaponInventory().HasWeapon(weaponId))
        {
            p->GetWeaponInventory().LevelUpWeapon(weaponId);
        }
    }
}

WeaponInventory GameState::BuildMergedInventoryView() const
{
    WeaponInventory merged;
    for (const auto& p : m_players)
    {
        if (!p) continue;
        for (const auto& w : p->GetWeaponInventory().GetWeapons())
        {
            if (w && !merged.HasWeapon(w->GetProfile().GetId()))
            {
                auto copy = m_weaponFactory.Create(w->GetProfile().GetId());
                if (copy)
                {
                    // Match level
                    for (int l = 1; l < w->GetProfile().GetCurrentLevel(); ++l)
                    {
                        copy->LevelUp();
                    }
                    merged.AddWeapon(std::move(copy));
                }
            }
        }
    }
    return merged;
}
