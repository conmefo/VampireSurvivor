#pragma once

class Player;

#include "../../Core/Data/WeaponProfile.h"
#include "../../Core/Data/WeaponLevelDelta.h"
#include "../Projectiles/ProjectileManager.h"
#include "IWeaponLevelObserver.h"
#include "../../Core/Audio/AudioIdentifiers.h"
#include <SFML/System/Vector2.hpp>
#include <vector>

class EnemyPool;
class IAudioService;

class Weapon
{
public:
    explicit Weapon(const WeaponProfile& profile);
    virtual ~Weapon() = default;

    virtual void Update(float dt, ProjectileManager& projManager, TextureAtlas& atlas, Player& player, EnemyPool& enemyPool);
    virtual void Draw(sf::RenderTarget& target) const {}

    // Template Method: applies next level delta, then calls OnLevelUp() hook.
    void LevelUp();
    void LevelDown();
    bool CanLevelUp() const;
    bool CanLevelDown() const;

    void SetLevelDeltas(const std::vector<WeaponLevelDelta>& deltas);
    const std::vector<WeaponLevelDelta>& GetLevelDeltas() const { return m_levelDeltas; }
    void SetObserver(IWeaponLevelObserver* observer);

    const WeaponProfile& GetProfile() const;

    // Audio: set the SFX played when this weapon fires.
    void SetFireSfx(SfxID sfxId) { m_fireSfxId = sfxId; }
    SfxID GetFireSfx() const { return m_fireSfxId; }

    // Audio: set the audio service for this weapon to use.
    void SetAudioService(IAudioService* audio) { m_audioService = audio; }

protected:
    // Override in subclasses for weapon-specific level-up side effects.
    virtual void OnLevelUp() {}

    virtual sf::Vector2f GetTargetPosition(EnemyPool& enemyPool, Player& player);
    virtual void FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex) = 0;

    WeaponProfile m_profile;
    float m_cooldownTimer;
    SfxID m_fireSfxId = SfxID::None;
    IAudioService* m_audioService = nullptr;

private:
    std::vector<WeaponLevelDelta> m_levelDeltas;
    IWeaponLevelObserver* m_observer = nullptr;
};
