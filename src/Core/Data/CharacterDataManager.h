#pragma once

#include "CharacterProfile.h"
#include <string>
#include <unordered_map>
#include <vector>

class CharacterDataManager
{
private:
    std::unordered_map<std::string, CharacterProfile> m_characters;
    std::vector<std::string> m_characterOrder;
    CharacterProfile m_fallbackProfile;

public:
    CharacterDataManager();

    bool LoadData(const std::string& configFilePath);

    const CharacterProfile& GetCharacterById(const std::string& id) const;

    const std::unordered_map<std::string, CharacterProfile>& GetAllCharacters() const;
    const std::vector<std::string>& GetCharacterOrder() const;
};
