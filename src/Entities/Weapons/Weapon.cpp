#include "Weapon.h"

Weapon::Weapon(const WeaponProfile& profile)
    : m_profile(profile)
    , m_cooldownTimer(0.0f)
    , m_projectilesPending(0)
    , m_repeatTimer(0.0f)
{
}

void Weapon::Update(float dt, ProjectileManager& projManager, TextureAtlas& atlas, sf::Vector2f playerPosition, sf::Vector2f playerDirection, sf::Vector2f targetPosition)
{
    // Handle sequential firing queue
    if(m_projectilesPending > 0)
    {
        m_repeatTimer -= dt;
        if(m_repeatTimer <= 0.0f)
        {
            int amount = m_profile.GetAmount();
            int currentIndex = amount - m_projectilesPending;
            FireOne(projManager, atlas, playerPosition, playerDirection, targetPosition, currentIndex);
            
            m_projectilesPending--;
            m_repeatTimer = static_cast<float>(m_profile.GetRepeatInterval()) / 1000.0f;
        }
    }

    if(m_cooldownTimer > 0.0f)
    {
        m_cooldownTimer -= dt;
    }

    if(m_cooldownTimer <= 0.0f && m_projectilesPending == 0)
    {
        int amount = m_profile.GetAmount();
        if(amount > 0)
        {
            float repeatSec = static_cast<float>(m_profile.GetRepeatInterval()) / 1000.0f;
            if(repeatSec <= 0.0f)
            {
                // Fire all immediately
                for(int i = 0; i < amount; ++i)
                {
                    FireOne(projManager, atlas, playerPosition, playerDirection, targetPosition, i);
                }
            }
            else
            {
                // Fire first immediately, queue rest
                FireOne(projManager, atlas, playerPosition, playerDirection, targetPosition, 0);
                if(amount > 1)
                {
                    m_projectilesPending = amount - 1;
                    m_repeatTimer = repeatSec;
                }
            }
        }
        
        m_cooldownTimer = static_cast<float>(m_profile.GetInterval()) / 1000.0f;
    }
}

const WeaponProfile& Weapon::GetProfile() const
{
    return m_profile;
}
