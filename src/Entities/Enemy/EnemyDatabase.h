#pragma once

#include "EnemyDefinition.h"

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <unordered_map>

class EnemyDatabase
{
public:
    bool LoadFromFile(const std::string& filepath);

    const EnemyDefinition* GetDefinition(const std::string& id) const;
    bool HasDefinition(const std::string& id) const;

private:
    const sf::Texture* LoadTexture(const std::string& texturePath);

    std::unordered_map<std::string, EnemyDefinition> m_definitions;
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> m_textures;
};
