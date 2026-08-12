#pragma once
#include "../../Entities/Enemy/EnemyDatabase.h"
#include "../../Entities/Enemy/EnemyPool.h"
#include "../../World/TileMap.h"
#include "../../World/TileMapManager.h"
#include "../../Entities/Player.h"
#include "../../Entities/VfxManager.h"
#include "../../Entities/Particles/ParticleManager.h"
#include "../../Entities/Pets/PulsePet.h"
#include "../../Entities/Pickups/ExperienceGemManager.h"
#include "../../Entities/DamageNumberManager.h"
#include "../../UI/GemTuningUI.h"
#include "../../UI/ParticleTuningUI.h"
#include "../../Entities/Projectiles/ProjectileManager.h"
#include "../../Entities/Weapons/Weapon.h"
#include "../../Entities/Weapons/WeaponFactory.h"
#include "../../Core/Data/WeaponDataManager.h"
#include "../../Core/Data/StageWaveDataManager.h"
#include "../../UI/PlayerHUD.h"
#include "../../UI/Elements/ExpBar.h"
#include "../../Core/LevelUp/LevelUpSessionController.h"
#include "../../UI/Views/SimpleTextLevelUpView.h"
#include "../BaseState.h"
#include <memory>
#include <random>
#include <string>
#include <unordered_set>
#include <vector>

class PauseMenuView;
class GameOverView;
class RunGoldDisplay;
class TreasureChestManager;
class TreasureRewardView;

class GameState : public BaseState {
  public:
    GameState(StateContext context, TileMapManager& mapManager, const std::vector<std::string>& selectedCharacterIds);
    ~GameState() override;

    void Init() override;
    void HandleInput(sf::Event &event, sf::RenderWindow &window) override;
    void Update(float dt) override;
    void Draw(sf::RenderWindow &window) override;

  private:
    enum class RunState
    {
        Playing,
        DefeatAnimating,
        Completed,
        Defeated
    };

    struct RuntimeStageEvent
    {
        StageWaveEvent definition;
        std::string enemyId;
        float elapsedMs = 0.0f;
        float nextTriggerMs = 0.0f;
        float spawnCooldown = 0.0f;
        int triggerCount = 0;
        int pendingSpawns = 0;
        int spawnCount = 0;
        int spawnSequence = 0;
    };

    void LoadStage(int stageNumber);
    void ResetStageSpawner();
    void UpdateStageSpawner(float dt);
    void ResetStageEventsForCurrentWave();
    void UpdateStageEvents(float dt);
    EnemyBase* SpawnWaveEnemy(const std::string& enemyId);
    EnemyBase* SpawnEnemyAt(const std::string& enemyId, const sf::Vector2f& position);
    void SpawnWaveBosses(const StageWaveDefinition& wave);
    const StageWaveDefinition* GetCurrentStageWave() const;
    sf::Vector2f GetWaveSpawnPosition(int spawnIndex) const;
    sf::Vector2f GetStageEventSpawnPosition(const RuntimeStageEvent& event, int spawnIndex) const;
    std::string ResolveSpawnEnemyId(const std::string& requestedId) const;
    bool RollStageEventChance(float chance);
    void UpdateStageTimer(float dt);
    void UpdateStageTimerText();
    void DrawStageTimer(sf::RenderTarget& target) const;
    std::string FormatStageTime(int totalSeconds) const;
    float GetStageClockSpeed() const;
    int GetStageTimeLimitSeconds() const;
    void FinishRun(RunState outcome);
    void UpdateDefeatAnimation(float dt);
    bool IsGameOverVisible() const;
    void ApplyCameraToView();
    void TogglePause();
    void ReturnToMainMenu();
    void AddRunGold(int amount);
    void BankRunGold();
    void ApplyEnemyContactDamage();
    float GetStageXpBonus() const;
    EnemyStats ApplyStageEnemyModifiers(const EnemyStats& stats) const;
    sf::FloatRect GetViewBounds() const;
    void DrawHitboxes(sf::RenderTarget &target);

    // 2-Player shared progression helpers
    bool AreAllPlayersDead() const;
    Player* GetFirstAlivePlayer();
    void AddSharedExperience(float amount);
    float GetSharedTargetExperience() const;
    float GetSharedExpProgressRatio() const;
    void BroadcastWeaponAdd(const std::string& weaponId);
    void BroadcastWeaponLevelUp(const std::string& weaponId);
    WeaponInventory BuildMergedInventoryView() const;

    static constexpr float CameraSpeed = 200.0f;
    static constexpr float ViewWidth = 1920.0f;
    static constexpr float ViewHeight = 1080.0f;
    static constexpr float WorldZoom = 2.2f;
    static constexpr float DefeatAnimationSeconds = 1.5f;

    TileMapManager& m_mapManager;
    TileMap* m_tileMap = nullptr;
    sf::View m_worldView;
    sf::Vector2f m_cameraCenter;
    EnemyDatabase m_enemyDatabase;
    EnemyPool m_enemyPool;
    StageWaveDataManager m_stageWaveData;
    const std::vector<StageWaveDefinition>* m_activeStageWaves = nullptr;
    const StageInfo* m_activeStageInfo = nullptr;
    const StageWaveDefinition* m_currentWave = nullptr;
    float m_stageElapsed = 0.0f;
    float m_waveSpawnTimer = 0.0f;
    int m_waveSpawnCursor = 0;
    std::vector<int> m_spawnedBossWaveMinutes;
    std::unordered_set<EnemyBase*> m_bossEnemies;
    std::vector<RuntimeStageEvent> m_runtimeStageEvents;
    std::mt19937 m_stageEventRng{std::random_device{}()};
    sf::RectangleShape m_stageTimerBacking;
    sf::Text m_stageTimerText;
    sf::Text m_stageTimerShadowText;
    sf::Text m_stageInfoText;
    std::vector<std::unique_ptr<Player>> m_players;
    std::vector<std::string> m_selectedCharacterIds;
    int m_currentStage = 1;
    RunState m_runState = RunState::Playing;
    float m_defeatAnimationTimer = 0.0f;
    bool m_showHitboxes = false;
    bool m_isPaused = false;
    VfxManager m_vfxManager;
    std::unique_ptr<PauseMenuView> m_pauseMenu;
    std::unique_ptr<GameOverView> m_gameOverView;
    std::unique_ptr<RunGoldDisplay> m_runGoldDisplay;
    int m_runGold = 0;
    bool m_runGoldBanked = false;
    vs::ParticleManager m_particleManager;
    vs::ParticleEmitterConfig m_testParticleConfig;
    std::unique_ptr<ParticleTuningUI> m_tuningUI;
    std::unique_ptr<GemTuningUI> m_gemTuningUI;
    vs::ParticleEmitterConfig m_bloodTearConfig;
    vs::ParticleEmitter* m_testEmitter = nullptr;
    ProjectileManager m_projectileManager;
    ExperienceGemManager m_experienceGems;
    DamageNumberManager m_damageNumbers;
    std::unique_ptr<TreasureChestManager> m_treasureChests;
    std::unique_ptr<TreasureRewardView> m_treasureRewardView;
    std::vector<std::unique_ptr<PlayerHUD>> m_playerHUDs;
    std::unique_ptr<ExpBar> m_expBar;
    WeaponFactory m_weaponFactory;
    LevelUpSessionController m_levelUpController;
    std::unique_ptr<SimpleTextLevelUpView> m_levelUpView;
    std::vector<std::unique_ptr<PulsePet>> m_pulsePets;

    // Shared 2-player progression state
    float m_sharedExperience = 0.0f;
    int m_sharedLevel = 1;
};
