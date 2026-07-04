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
    'src/Entities/Weapons/RunetracerWeapon.cpp',
    'src/Entities/Player.cpp'
]

for file in weapon_files:
    if not os.path.exists(file): continue
    with open(file, 'r') as f:
        content = f.read()

    # Forward declare Player in headers
    if file.endswith('.h') and 'class Player;' not in content and 'WeaponInventory' not in file:
        content = content.replace('#pragma once\n', '#pragma once\n\nclass Player;\n')

    # Include Player.h in cpp files
    if file.endswith('.cpp') and 'Player.h' not in content and file != 'src/Entities/Player.cpp':
        content = content.replace('#include \"', '#include \"../Player.h\"\n#include \"', 1)

    # Change signature
    content = content.replace('sf::Vector2f playerPosition, sf::Vector2f playerDirection', 'Player& player')
    
    # Change usages
    content = content.replace('playerPosition', 'player.GetPosition()')
    content = content.replace('playerDirection', 'player.GetFacingDirection()')
    
    # In Player.cpp, fix the call
    if file == 'src/Entities/Player.cpp':
        content = content.replace('m_weaponInventory.Update(dt, projManager, atlas, m_position, m_currentDirection, enemyPool);', 'm_weaponInventory.Update(dt, projManager, atlas, *this, enemyPool);')
        content = content.replace('m_weaponInventory.Update(dt, projManager, atlas, m_position, m_facingDirection, enemyPool);', 'm_weaponInventory.Update(dt, projManager, atlas, *this, enemyPool);')
        content = content.replace('weapon->Update(dt, projManager, atlas, m_position, m_facingDirection, enemyPool);', 'weapon->Update(dt, projManager, atlas, *this, enemyPool);')
        content = content.replace('weapon->Update(dt, projManager, atlas, m_position, m_currentDirection, enemyPool);', 'weapon->Update(dt, projManager, atlas, *this, enemyPool);')

    # Fix lambda captures in Weapon.cpp
    if file == 'src/Entities/Weapons/Weapon.cpp':
        content = content.replace('[this, &projManager, &atlas, player.GetPosition(), player.GetFacingDirection(), targetPosition, i]', '[this, &projManager, &atlas, &player, targetPosition, i]')
        # Fix the lambda call
        content = content.replace('this->FireOne(projManager, atlas, player.GetPosition(), player.GetFacingDirection(), targetPosition, i);', 'this->FireOne(projManager, atlas, player, targetPosition, i);')

    # Fix lambda captures in RunetracerWeapon.cpp
    if file == 'src/Entities/Weapons/RunetracerWeapon.cpp':
        content = content.replace('[this, &projManager, &atlas, player.GetPosition(), player.GetFacingDirection(), targetPosition, i]', '[this, &projManager, &atlas, &player, targetPosition, i]')
        # Fix the lambda call
        content = content.replace('this->FireOne(projManager, atlas, player.GetPosition(), player.GetFacingDirection(), targetPosition, i);', 'this->FireOne(projManager, atlas, player, targetPosition, i);')

    with open(file, 'w') as f:
        f.write(content)

print('Done')
