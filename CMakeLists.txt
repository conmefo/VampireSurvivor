cmake_minimum_required(VERSION 3.20)

project(VampireSurvivors LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

include(FetchContent)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "Build SFML audio module" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "Build SFML network module" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "Build SFML examples" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "Build SFML documentation" FORCE)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.2
    GIT_SHALLOW TRUE
)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(SFML nlohmann_json)

add_executable(VampireSurvivors
    src/Entities/Agent.cpp
    src/Entities/Enemy/Bat1.cpp
    src/Entities/Enemy/EnemyBase.cpp
    src/Entities/Enemy/EnemyFactory.cpp
    src/Entities/Enemy/EnemyPool.cpp
    src/Entities/Enemy/Skeleton.cpp
    src/UI/UIManager.cpp
    src/States/Menu/MainMenuState.cpp
    src/main.cpp
    src/States/StateManager.cpp
    src/States/Intro/LoadingState.cpp
    src/States/Intro/WarningState.cpp
    src/States/Menu/MainMenuState.cpp
    src/States/Menu/PowerUpState.cpp
    src/States/Game/GameState.cpp
    src/World/TileMap.cpp
    src/Core/Math/MathUtils.cpp
    src/Core/Animation/Tweener.cpp
    src/Core/Animation/Spring.cpp
    src/UI/Core/UIElement.cpp
    src/UI/Panels/UIPanel.cpp
    src/UI/Elements/UIButton.cpp
    src/UI/Components/FaderComponent.cpp
    src/UI/Components/NineSliceComponent.cpp
    src/UI/Components/NineSliceComponent.h
    src/UI/Components/PowerUpCard.cpp
    src/UI/Components/PowerUpCard.h
    src/UI/Components/UIGridLayout.cpp
    src/UI/Components/UIGridLayout.h
    src/UI/Components/UIScrollView.cpp
    src/UI/Components/UIScrollView.h
    src/UI/Components/UIScrollbar.cpp
    src/UI/Components/UIScrollbar.h
    src/UI/Components/UIDetailPanel.cpp
    src/UI/Components/UIDetailPanel.h
    src/Core/Data/PowerUpData.h
    src/Core/UI/ScreenTransitionManager.cpp
    src/States/Title/TitleState.cpp
    src/Core/Resources/TextureAtlas.cpp
    src/Core/Animation/AnimationLibrary.cpp
    src/Core/Animation/SpriteAnimator.cpp
    src/UI/Elements/GoldDisplayWidget.cpp
    src/UI/Elements/CharacterCardWidget.cpp
    src/Core/Data/CharacterDataManager.cpp
    src/UI/Panels/MainBoardPanel.cpp
    src/UI/Panels/RosterGridPanel.cpp
    src/UI/Panels/StatsPanel.cpp
    src/UI/Views/CharacterSelectionView.cpp
)

target_include_directories(VampireSurvivors PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(VampireSurvivors PRIVATE 
    sfml-graphics 
    sfml-window 
    sfml-system
    sfml-main
    nlohmann_json::nlohmann_json
)

add_custom_command(TARGET VampireSurvivors POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Assets"
        "$<TARGET_FILE_DIR:VampireSurvivors>/Assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:VampireSurvivors>/assets"
    COMMENT "Copying Assets directory to build output..."
)
