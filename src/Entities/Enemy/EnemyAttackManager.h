#pragma once

#include <SFML/Graphics.hpp>

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class EnemyBase;
class Player;
class TextureAtlas;

// Owns enemy-owned attacks separately from the player projectile pipeline.
// This keeps enemy shots from damaging enemies or entering the XP/death flow.
class EnemyAttackManager
{
public:
    void Initialize(const TextureAtlas& atlas);
    void Clear();

    void Update(
        float dt,
        const std::vector<EnemyBase*>& enemies,
        const std::vector<std::unique_ptr<Player>>& players,
        const std::unordered_set<EnemyBase*>& bossEnemies);

    void Draw(sf::RenderTarget& target) const;

    std::size_t GetActiveProjectileCount() const { return m_projectiles.size(); }

private:
    struct AttackState
    {
        float cooldown = 0.35f;
        float telegraphRemaining = 0.0f;
        sf::Vector2f aimPosition{0.0f, 0.0f};
        bool bossBurst = false;
    };

    struct Projectile
    {
        sf::Vector2f position{0.0f, 0.0f};
        sf::Vector2f velocity{0.0f, 0.0f};
        float damage = 1.0f;
        float radius = 7.0f;
        float lifetime = 6.0f;
        float age = 0.0f;
        sf::CircleShape visual;
        sf::Sprite sprite;
        bool usesSprite = false;
    };

    static float Length(const sf::Vector2f& value);
    static sf::Vector2f Normalize(const sf::Vector2f& value);
    static Player* FindNearestPlayer(
        const sf::Vector2f& position,
        const std::vector<std::unique_ptr<Player>>& players);

    void FireSingleProjectile(const EnemyBase& enemy, const AttackState& state);
    void FireBossBurst(const EnemyBase& enemy);
    void SpawnProjectile(
        const sf::Vector2f& position,
        const sf::Vector2f& velocity,
        float damage,
        float radius,
        float lifetime,
        const sf::Color& color,
        const sf::Texture* texture,
        const sf::IntRect& textureRect);

    std::unordered_map<EnemyBase*, AttackState> m_attackStates;
    std::vector<Projectile> m_projectiles;
    sf::Texture m_authenticEnemyProjectileTexture;
    bool m_authenticEnemyProjectileTextureReady = false;
    sf::IntRect m_authenticEnemyProjectileRect{704, 243, 15, 15};
    const sf::Texture* m_normalProjectileTexture = nullptr;
    sf::IntRect m_normalProjectileRect;
    const sf::Texture* m_bossProjectileTexture = nullptr;
    sf::IntRect m_bossProjectileRect;
};
