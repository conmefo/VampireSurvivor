#include "GameState.h"
#include "PauseMenuView.h"
#include "../../Entities/Projectiles/RunetracerProjectile.h"
#include "../StateManager.h"
#include "../Menu/MainMenuState.h"
#include "../../Core/WindowSettings.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>
#include "../../Core/Data/CharacterDataManager.h"
#include "../../Core/Data/WeaponDataManager.h"
#include "../../Entities/Weapons/WhipWeapon.h"
#include "../../Entities/Weapons/MagicMissileWeapon.h"
#include "../../Entities/Weapons/FireballWeapon.h"
#include "../../Entities/Weapons/RunetracerWeapon.h"
#include "../../Entities/Weapons/KnifeWeapon.h"
#include "../../Entities/Weapons/AxeWeapon.h"
#include "../../Core/Math/MathUtils.h"
#include "../../Core/Physics/Collision.h"

namespace {
constexpr std::size_t MaxRuntimeEnemies = 80;
constexpr int MaxOpeningSpawns = 12;
constexpr int MaxSpawnBatchPerTick = 3;
constexpr float MinWaveSpawnIntervalSeconds = 0.15f;

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
} // namespace

GameState::GameState(StateContext context, TileMapManager& mapManager, const std::string& selectedCharacterId)
    : BaseState(std::move(context)), m_mapManager(mapManager), m_enemyPool(m_enemyDatabase), m_selectedCharacterId(selectedCharacterId) {}

GameState::~GameState() = default;

void GameState::Init() {
    std::cout << "GameState Init" << std::endl;

    m_vfxManager.Initialize(m_context.atlas);
    m_particleManager.Initialize(&m_context.atlas, &m_context.particleData);
    m_projectileManager.Initialize(&m_particleManager);

    m_worldView.setSize(ViewWidth / WorldZoom, ViewHeight / WorldZoom);

    if(!m_stageWaveData.LoadData("assets/Data/STAGE_DATA.json"))
    {
        std::cerr << "GameState: failed to load stage wave data" << std::endl;
    }

    LoadStage(1);

    const CharacterProfile& profile = m_context.characterData.GetCharacterById(m_selectedCharacterId);

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
        m_player = std::make_unique<Player>(profile, *texture, frames);
        m_player->SetPosition(m_cameraCenter);

        m_player->SetOnHitVfxCallback([this](const std::string& vfxName, sf::Vector2f pos) {
            const HitVfxProfile& vfxProfile = m_context.hitVfxData.GetVfxByName(vfxName);
            if(vfxProfile.GetId() != -1) {
                m_vfxManager.PlayVfx(vfxProfile, pos);
            }
            // Temporarily disabled for clean tuning!
            // // Spawn blood tear particles as a burst!
            // auto burstConfig = m_context.particleData.GetConfig("bloodTear");
            // burstConfig.looping = false;
            // burstConfig.duration = 0.1f;
            // m_particleManager.SpawnEmitter(burstConfig, pos);
        });

        m_playerHUD = std::make_unique<PlayerHUD>(*m_player);

        // Add Starting Weapon
        const WeaponProfile& wp = m_context.weaponData.GetWeaponById(profile.GetStartingWeaponId());
        if(wp.GetId() == "WHIP")
        {
            m_player->GetWeaponInventory().AddWeapon(std::make_unique<WhipWeapon>(wp));
        }
        else if(wp.GetId() == "MAGIC_MISSILE")
        {
            m_player->GetWeaponInventory().AddWeapon(std::make_unique<MagicMissileWeapon>(wp));
        }
        else if(wp.GetId() == "FIREBALL")
        {
            m_player->GetWeaponInventory().AddWeapon(std::make_unique<FireballWeapon>(wp));
        }
        else if(wp.GetId() == "KNIFE")
        {
            m_player->GetWeaponInventory().AddWeapon(std::make_unique<KnifeWeapon>(wp));
        }
        else if(wp.GetId() == "AXE")
        {
            m_player->GetWeaponInventory().AddWeapon(std::make_unique<AxeWeapon>(wp));
        }

        // FOR TESTING: Always grant the Axe
        const WeaponProfile& testWp = m_context.weaponData.GetWeaponById("AXE");
        m_player->GetWeaponInventory().AddWeapon(std::make_unique<AxeWeapon>(testWp));
    }
    else
    {
        std::cerr << "Failed to find texture data for player sprite: " << profile.GetSpriteName() << std::endl;
    }

    const sf::Font* font = m_context.fonts.GetPtr(FontID::Main);
    const sf::Font* boldFont = m_context.fonts.GetPtr(FontID::Bold);
    if(font && boldFont)
    {
        m_pauseMenu = std::make_unique<PauseMenuView>(
            m_context.atlas,
            *font,
            *boldFont,
            profile,
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
    }

    if(boldFont && font)
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
    }

    // Initialize Particle UI
    m_testParticleConfig = m_context.particleData.GetConfig("bloodTear");
    m_testParticleConfig.looping = true; // Temporary loop for tuning
    m_testParticleConfig.duration = 9999.0f;
    m_testParticleConfig.weaponScaleX = 1.0f;
    m_testParticleConfig.weaponScaleY = 1.0f;
    m_testParticleConfig.trailWidth = 15.0f;
    m_testParticleConfig.trailFadeStart = 0.5f;
    m_testParticleConfig.trailLength = 0.8f;
    m_testParticleConfig.colorR = 0.0f;
    m_testParticleConfig.colorG = 255.0f;
    m_testParticleConfig.colorB = 255.0f;
    m_testParticleConfig.colorA = 255.0f;

    // Attach Tuning UI for Axe testing
    m_tuningUI = std::make_unique<ParticleTuningUI>(m_context.atlas, m_context.fonts.Get(FontID::Main), m_testParticleConfig);

    ApplyCameraToView();
}

void GameState::HandleInput(sf::Event &event, sf::RenderWindow &window) {
    (void)window;

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
        if (m_player && !m_player->IsDead()) {
            m_player->TakeDamage(99999.0f);
        }
    } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::L) {
        if (m_player && m_player->IsDead()) {
            m_player->Revive();
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
}

void GameState::Update(float dt) {
    if(m_isPaused)
    {
        if(m_pauseMenu)
        {
            m_pauseMenu->Update(dt);
        }
        return;
    }

    if(m_player)
    {
        m_player->Update(dt);
        m_cameraCenter = m_player->GetPosition();
        ApplyCameraToView();

        m_player->GetWeaponInventory().Update(dt, m_projectileManager, m_context.atlas, *m_player, m_enemyPool);

        if (m_playerHUD) {
            m_playerHUD->Update(dt);
        }
    }
    UpdateStageTimer(dt);
    UpdateStageSpawner(dt);
    m_enemyPool.Update(dt, m_cameraCenter);
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
            sf::FloatRect enemyBounds(
                enemy->GetPosition().x - radius,
                enemy->GetPosition().y - radius,
                radius * 2.0f,
                radius * 2.0f
            );
            collisionTargets.push_back({ enemyBounds, static_cast<void*>(enemy) });
        }
    }

    auto hits = m_projectileManager.CheckCollisions(collisionTargets);
    for(auto& hit : hits)
    {
        EnemyBase* enemy = static_cast<EnemyBase*>(hit.second);
        Projectile* proj = hit.first;
        if(enemy && proj && m_player)
        {
            sf::Vector2f originalPos = enemy->GetPosition();

            sf::Vector2f diff = originalPos - m_player->GetPosition();
            float len = std::sqrt(diff.x * diff.x + diff.y * diff.y);
            sf::Vector2f knockbackDir = (len > 0.0f) ? (diff / len) : sf::Vector2f(1.0f, 0.0f);
            const bool killed = enemy->TakeDamage(proj->GetPower(), knockbackDir);

            constexpr float KNOCKBACK_FORCE = 15.0f;
            if(!killed)
            {
                enemy->ApplyKnockback(knockbackDir * KNOCKBACK_FORCE);
            }
        }
    }

    m_vfxManager.Update(dt);
    m_particleManager.Update(dt);

    if (m_tileMap) {
        const std::vector<sf::FloatRect> obstacles =
            m_tileMap->GetEnemyCollisionRectsInArea(GetViewBounds());
        m_enemyPool.ResolveObstacleCollisions(obstacles);
    }
    m_enemyPool.ResolveEnemyCollisions();
    ApplyEnemyContactDamage();

    // Update tuning UI to match absolute screen position
    if (m_tuningUI) {
        m_tuningUI->SetPosition(sf::Vector2f(20.0f, 100.0f));
        m_tuningUI->Update(dt);
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

    m_particleManager.Draw(window);
    m_projectileManager.Draw(window);
    m_vfxManager.Draw(window);

    if(m_player)
    {
        m_player->Draw(window);
        if (m_playerHUD) {
            m_playerHUD->Draw(window);
        }
    }

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

    DrawStageTimer(window);

    window.setView(previousView);

    if (m_tuningUI) {
        // Draw in a fixed UI view so it is not affected by camera zoom
        sf::View tuningUiView(sf::FloatRect(0.0f, 0.0f, ViewWidth, ViewHeight));
        window.setView(tuningUiView);
        m_tuningUI->Draw(window);
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
        if(m_player)
        {
            m_pauseMenu->SetPlayerPosition(m_player->GetPosition(), worldSize);
        }
        m_pauseMenu->Draw(window);
        window.setView(previousView);
    }
}

void GameState::LoadStage(int stageNumber) {
    m_currentStage = stageNumber;
    m_enemyPool.Clear();

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

    if (m_player) {
        m_player->SetPosition(m_cameraCenter);
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

void GameState::SpawnWaveEnemy(const std::string& enemyId)
{
    const std::string resolvedEnemyId = ResolveSpawnEnemyId(enemyId);
    if(resolvedEnemyId.empty())
    {
        ++m_waveSpawnCursor;
        return;
    }

    const sf::Vector2f spawnPosition = GetWaveSpawnPosition(m_waveSpawnCursor);
    m_enemyPool.Acquire(resolvedEnemyId, spawnPosition);
    ++m_waveSpawnCursor;
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
        SpawnWaveEnemy(bossId);
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
    if(timeLimitSeconds > 0 && m_stageElapsed > static_cast<float>(timeLimitSeconds))
    {
        m_stageElapsed = static_cast<float>(timeLimitSeconds);
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

void GameState::TogglePause()
{
    m_isPaused = !m_isPaused;
}

void GameState::ReturnToMainMenu()
{
    m_isPaused = false;
    m_context.stateManager.ChangeStateWithTransition(
        std::make_unique<MainMenuState>(m_context, m_mapManager),
        0.35f,
        sf::Color::Black);
}

void GameState::ApplyEnemyContactDamage()
{
    if(!m_player || m_player->IsDead())
    {
        return;
    }

    const sf::Vector2f playerPosition = m_player->GetPosition();
    const float playerRadius = m_player->GetCollisionRadius();
    float contactDamage = 0.0f;

    for(EnemyBase* enemy : m_enemyPool.GetActiveEnemies())
    {
        if(!enemy || !enemy->IsAlive())
        {
            continue;
        }

        if(Collision::CircleIntersectsCircle(
               playerPosition,
               playerRadius,
               enemy->GetPosition(),
               enemy->GetCollisionRadius()) &&
           enemy->GetDamage() > contactDamage)
        {
            contactDamage = enemy->GetDamage();
        }
    }

    if(contactDamage > 0.0f)
    {
        m_player->TakeDamage(contactDamage);
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

    if(m_player && !m_player->IsDead())
    {
        const float radius = m_player->GetCollisionRadius();
        sf::CircleShape playerHitbox(radius);
        playerHitbox.setOrigin(radius, radius);
        playerHitbox.setPosition(m_player->GetPosition());
        playerHitbox.setFillColor(sf::Color(80, 170, 255, 35));
        playerHitbox.setOutlineColor(sf::Color(80, 170, 255, 220));
        playerHitbox.setOutlineThickness(1.0f);
        target.draw(playerHitbox);
    }
}
