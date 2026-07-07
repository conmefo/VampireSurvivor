import os

weapon_files = [
    'src/Entities/Weapons/Weapon.h',
    'src/Entities/Weapons/Weapon.cpp',
    'src/Entities/Weapons/WeaponInventory.h',
    'src/Entities/Weapons/WeaponInventory.cpp',
    'src/Entities/Weapons/WhipWeapon.h',
    'src/Entities/Weapons/WhipWeapon.cpp',
    'src/Entities/Weapons/MagicMissileWeapon.h',
    'src/Entities/Weapons/MagicMissileWeapon.cpp',
    'src/Entities/Weapons/FireballWeapon.h',
    'src/Entities/Weapons/FireballWeapon.cpp',
    'src/Entities/Weapons/RunetracerWeapon.h',
    'src/Entities/Weapons/RunetracerWeapon.cpp'
]

for file in weapon_files:
    if not os.path.exists(file): continue
    with open(file, 'r') as f:
        content = f.read()

    # Fix GetTargetPosition
    content = content.replace('GetTargetPosition(enemyPool, player.GetPosition(), player.GetFacingDirection())', 'GetTargetPosition(enemyPool, player)')
    
    # Fix FireOne
    content = content.replace('FireOne(projManager, atlas, player.GetPosition(), player.GetFacingDirection(), targetPosition, i)', 'FireOne(projManager, atlas, player, targetPosition, i)')
    content = content.replace('FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex)', 'FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex)')
    
    # Check MagicMissileWeapon.cpp since it doesn't use Player directly sometimes?
    # Wait, the signature of FireOne was replaced to:
    # FireOne(ProjectileManager& projManager, TextureAtlas& atlas, Player& player, sf::Vector2f targetPosition, int projectileIndex)
    # The previous python script replaced:
    # 'sf::Vector2f playerPosition, sf::Vector2f playerDirection' with 'Player& player'
    
    with open(file, 'w') as f:
        f.write(content)

print('Done')
