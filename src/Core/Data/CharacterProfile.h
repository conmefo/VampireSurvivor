#pragma once

#include <string>
#include <unordered_map>

class CharacterProfile
{
private:
    std::string id;
    std::string name;
    std::string description;
    std::string portraitTextureId;
    std::string startingWeaponId;
    int basePrice;
    std::unordered_map<std::string, float> stats;
    std::string spriteName;
    int walkingFrames;

    bool isAlwaysHidden;
    bool isBought;

public:
    CharacterProfile(
        const std::string& id,
        const std::string& name,
        const std::string& description,
        const std::string& portraitTextureId,
        const std::string& startingWeaponId,
        int basePrice,
        const std::unordered_map<std::string, float>& stats,
        const std::string& spriteName = "",
        int walkingFrames = 1,
        bool isAlwaysHidden = false,
        bool isBought = false)
        : id(id), 
          name(name), 
          description(description), 
          portraitTextureId(portraitTextureId), 
          startingWeaponId(startingWeaponId), 
          basePrice(basePrice), 
          stats(stats),
          spriteName(spriteName),
          walkingFrames(walkingFrames),
          isAlwaysHidden(isAlwaysHidden),
          isBought(isBought)
    {
    }

    const std::string& GetId() const
    {
        return id;
    }

    const std::string& GetName() const
    {
        return name;
    }

    const std::string& GetDescription() const
    {
        return description;
    }

    const std::string& GetPortraitTextureId() const
    {
        return portraitTextureId;
    }

    const std::string& GetStartingWeaponId() const
    {
        return startingWeaponId;
    }

    int GetBasePrice() const
    {
        return basePrice;
    }

    const std::unordered_map<std::string, float>& GetStats() const
    {
        return stats;
    }

    float GetStat(const std::string& statName) const
    {
        auto it = stats.find(statName);
        if(it != stats.end())
        {
            return it->second;
        }
        return 0.0f;
    }

    const std::string& GetSpriteName() const
    {
        return spriteName;
    }

    int GetWalkingFrames() const
    {
        return walkingFrames;
    }

    bool IsAlwaysHidden() const
    {
        return isAlwaysHidden;
    }

    bool IsBought() const
    {
        return isBought;
    }
};
