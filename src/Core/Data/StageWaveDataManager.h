#pragma once

#include <string>
#include <unordered_map>
#include <vector>

struct StageWaveEvent
{
    std::string eventType;
    float delayMs = 0.0f;
    int repeat = 1;
    float chance = 100.0f;
    int moreX = 0;
    std::string moreY;
    float durationMs = 0.0f;
};

struct StageWaveDefinition
{
    int minute = 0;
    int minimum = 0;
    float frequencyMs = 1000.0f;
    int startingSpawns = 0;
    std::string spawnType = "STANDARD";
    std::vector<std::string> enemies;
    std::vector<std::string> bosses;
    std::vector<StageWaveEvent> events;
};

struct StageInfo
{
    std::string stageKey;
    std::string stageName;
    std::string stageNumber;
    int timeLimitSeconds = 1800;
    float clockSpeed = 1.0f;
    float xpBonus = 1.0f;
    float enemySpeedMultiplier = 1.0f;
    float enemyHealthMultiplier = 1.0f;
};

class StageWaveDataManager
{
public:
    bool LoadData(const std::string& configFilePath);

    const std::vector<StageWaveDefinition>* GetStageWaves(const std::string& stageKey) const;
    const StageInfo* GetStageInfo(const std::string& stageKey) const;

private:
    std::unordered_map<std::string, std::vector<StageWaveDefinition>> m_stageWaves;
    std::unordered_map<std::string, StageInfo> m_stageInfo;
};
