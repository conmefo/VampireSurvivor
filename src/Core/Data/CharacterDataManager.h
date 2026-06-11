#pragma once

#include "CharacterProfile.h"
#include <string>
#include <unordered_map>

class CharacterDataManager
{
private:
    std::unordered_map<std::string, CharacterProfile> m_characters;
    CharacterProfile m_fallbackProfile;

public:
    CharacterDataManager();

    bool LoadData(const std::string& configFilePath);

    const CharacterProfile& GetCharacterById(const std::string& id) const;

    const std::unordered_map<std::string, CharacterProfile>& GetAllCharacters() const;
};
