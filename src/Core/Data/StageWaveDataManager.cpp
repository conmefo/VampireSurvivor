#include "StageWaveDataManager.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

namespace
{
std::vector<std::string> ReadStringArray(const nlohmann::json& jsonData, const char* key)
{
    std::vector<std::string> values;
    if(!jsonData.contains(key) || !jsonData[key].is_array())
    {
        return values;
    }

    for(const auto& value : jsonData[key])
    {
        if(value.is_string())
        {
            values.push_back(value.get<std::string>());
        }
    }

    return values;
}

std::string ReadStringField(const nlohmann::json& jsonData, const char* key, const std::string& fallback)
{
    if(!jsonData.contains(key) || !jsonData[key].is_string())
    {
        return fallback;
    }

    return jsonData[key].get<std::string>();
}

float ReadFloatField(const nlohmann::json& jsonData, const char* key, float fallback)
{
    if(!jsonData.contains(key) || !jsonData[key].is_number())
    {
        return fallback;
    }

    return jsonData[key].get<float>();
}

int ReadIntField(const nlohmann::json& jsonData, const char* key, int fallback)
{
    if(!jsonData.contains(key) || !jsonData[key].is_number())
    {
        return fallback;
    }

    return jsonData[key].get<int>();
}

StageWaveEvent ReadEvent(const nlohmann::json& eventJson)
{
    StageWaveEvent event;
    event.eventType = ReadStringField(eventJson, "eventType", "");
    event.delayMs = ReadFloatField(eventJson, "delay", 0.0f);
    event.repeat = ReadIntField(eventJson, "repeat", 1);
    event.chance = ReadFloatField(eventJson, "chance", 100.0f);
    event.moreX = ReadIntField(eventJson, "moreX", 0);
    event.moreY = ReadStringField(eventJson, "moreY", "");
    event.durationMs = ReadFloatField(eventJson, "duration", 0.0f);
    return event;
}

std::vector<StageWaveEvent> ReadEvents(const nlohmann::json& waveJson)
{
    std::vector<StageWaveEvent> events;
    if(!waveJson.contains("events"))
    {
        return events;
    }

    const auto& eventsJson = waveJson["events"];
    if(eventsJson.is_array())
    {
        for(const auto& eventJson : eventsJson)
        {
            if(eventJson.is_object())
            {
                events.push_back(ReadEvent(eventJson));
            }
        }
    }
    else if(eventsJson.is_object())
    {
        events.push_back(ReadEvent(eventsJson));
    }

    return events;
}

StageInfo ReadStageInfo(const std::string& stageKey, const nlohmann::json& stageJson)
{
    StageInfo info;
    info.stageKey = stageKey;
    info.stageName = ReadStringField(stageJson, "stageName", stageKey);
    info.stageNumber = ReadStringField(stageJson, "stageNumber", "");

    if(stageJson.contains("mods") && stageJson["mods"].is_object())
    {
        const auto& mods = stageJson["mods"];
        info.timeLimitSeconds = ReadIntField(mods, "TimeLimit", info.timeLimitSeconds);
        info.clockSpeed = ReadFloatField(mods, "ClockSpeed", info.clockSpeed);
        info.xpBonus = ReadFloatField(mods, "XPBonus", info.xpBonus);
    }

    return info;
}
}

bool StageWaveDataManager::LoadData(const std::string& configFilePath)
{
    std::ifstream file(configFilePath);
    if(!file.is_open())
    {
        std::cerr << "Failed to open stage wave data file: " << configFilePath << "\n";
        return false;
    }

    nlohmann::json jsonData;
    try
    {
        file >> jsonData;
    }
    catch(const nlohmann::json::parse_error& e)
    {
        std::cerr << "JSON parse error in file " << configFilePath << ": " << e.what() << "\n";
        return false;
    }

    m_stageWaves.clear();
    m_stageInfo.clear();

    for(const auto& stage : jsonData.items())
    {
        if(!stage.value().is_array())
        {
            continue;
        }

        std::vector<StageWaveDefinition> waves;
        std::string activeSpawnType = "STANDARD";
        for(const auto& waveJson : stage.value())
        {
            if(!waveJson.is_object() || !waveJson.contains("minute"))
            {
                continue;
            }

            StageWaveDefinition wave;
            wave.minute = ReadIntField(waveJson, "minute", 0);
            wave.minimum = ReadIntField(waveJson, "minimum", 0);
            wave.frequencyMs = ReadFloatField(waveJson, "frequency", 1000.0f);
            wave.startingSpawns = ReadIntField(waveJson, "startingSpawns", 0);
            activeSpawnType = ReadStringField(waveJson, "spawnType", activeSpawnType);
            wave.spawnType = activeSpawnType;
            wave.enemies = ReadStringArray(waveJson, "enemies");
            wave.bosses = ReadStringArray(waveJson, "bosses");
            wave.events = ReadEvents(waveJson);
            waves.push_back(wave);
        }

        if(!waves.empty())
        {
            m_stageWaves.insert({stage.key(), std::move(waves)});
            if(stage.value().front().is_object())
            {
                m_stageInfo.insert({stage.key(), ReadStageInfo(stage.key(), stage.value().front())});
            }
        }
    }

    return !m_stageWaves.empty();
}

const std::vector<StageWaveDefinition>* StageWaveDataManager::GetStageWaves(const std::string& stageKey) const
{
    auto it = m_stageWaves.find(stageKey);
    if(it == m_stageWaves.end())
    {
        return nullptr;
    }

    return &it->second;
}

const StageInfo* StageWaveDataManager::GetStageInfo(const std::string& stageKey) const
{
    auto it = m_stageInfo.find(stageKey);
    if(it == m_stageInfo.end())
    {
        return nullptr;
    }

    return &it->second;
}
