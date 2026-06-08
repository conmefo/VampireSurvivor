#include "main.h"
#include "States/StateManager.h"
#include "States/Intro/LoadingState.h"
#include "Core/Resources/ResourceManager.h"
#include "Core/Resources/ResourceIdentifiers.h"
#include "Core/Resources/TextureAtlas.h"
#include "States/StateContext.h"
#include "Core/WindowSettings.h"
#include "Core/Animation/AnimationLibrary.h"
#include <SFML/Graphics.hpp>
#include <iostream>

int runSfmlTest()
{
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "Vampire Survivors Clone", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

    float screenRatio = static_cast<float>(desktop.width) / static_cast<float>(desktop.height);
    sf::View view;
    view.setSize(Core::VIRTUAL_WIDTH, Core::VIRTUAL_HEIGHT);
    view.setCenter(Core::VIRTUAL_WIDTH / 2.0f, Core::VIRTUAL_HEIGHT / 2.0f);

    if (screenRatio > Core::VIRTUAL_ASPECT_RATIO)
    {
        // Pillarbox
        float viewportWidth = Core::VIRTUAL_ASPECT_RATIO / screenRatio;
        float viewportX = (1.0f - viewportWidth) / 2.0f;
        view.setViewport(sf::FloatRect(viewportX, 0.0f, viewportWidth, 1.0f));
    }
    else
    {
        // Letterbox
        float viewportHeight = screenRatio / Core::VIRTUAL_ASPECT_RATIO;
        float viewportY = (1.0f - viewportHeight) / 2.0f;
        view.setViewport(sf::FloatRect(0.0f, viewportY, 1.0f, viewportHeight));
    }
    
    window.setView(view);

    ResourceManager<sf::Texture, std::string> textureManager;
    ResourceManager<sf::Font, FontID> fontManager;
    TextureAtlas textureAtlas(textureManager);
    
    try
    {
        textureManager.Load("UIAtlas", "Assets/Graphics/Spritesheets/UI.png");
        textureManager.Load("Background", "Assets/Graphics/Backgrounds/introBG_0.png");
        textureManager.Load("Title", "Assets/Graphics/Backgrounds/title.png");
        textureManager.Load("Prompt", "Assets/Graphics/Backgrounds/NowLoading_VS.png"); 
        
        textureManager.Load("CharacterDecoration_0", "Assets/Graphics/Spritesheets/demonSkull.png");
        textureManager.Load("CharacterDecoration_1", "Assets/Graphics/Spritesheets/demonSkull.png");
        textureManager.Load("CharacterDecoration_2", "Assets/Graphics/Spritesheets/demonSkull.png");

        textureManager.Load("Illustrations", "Assets/Graphics/Spritesheets/illustrations.png");
    }
    catch (const std::exception& e)
    {
        std::cerr << "Initialization Error: " << e.what() << "\n";
    }

    const sf::Texture* atlasTex = textureManager.GetPtr("UIAtlas");
    if (atlasTex)
    {
        textureAtlas.LoadFromFile("Assets/Data/ui_atlas.json", "UIAtlas", atlasTex->getSize().y);
    }

    const sf::Texture* illTex = textureManager.GetPtr("Illustrations");
    if (illTex)
    {
        // Notice: illustrations_atlas might not have standard inverted origin, assuming standard inverted format since it's processed similarly
        textureAtlas.LoadFromFile("Assets/Data/illustration_atlas.json", "Illustrations", illTex->getSize().y);
    }

    AnimationLibrary animLibrary(textureAtlas);
    // In a real scenario, this path would be absolute or relative to the executable.
    animLibrary.LoadFromJson("Assets/Data/animations.json");

    StateManager stateManager;
    StateContext context(stateManager, textureManager, fontManager, textureAtlas, animLibrary);

    stateManager.AddState(std::make_unique<LoadingState>(context));
    stateManager.ProcessStateChanges();

    sf::Clock clock;

    while(window.isOpen())
    {
        sf::Event event{};
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
            {
                window.close();
            }

            stateManager.HandleInput(event, window);
        }

        float dt = clock.restart().asSeconds();

        stateManager.Update(dt);
        stateManager.ProcessStateChanges();

        if(stateManager.IsEmpty())
        {
            window.close();
        }

        window.clear(sf::Color(20, 20, 30));
        stateManager.Draw(window);
        window.display();
    }

    return 0;
}

int main()
{
    return runSfmlTest();
}
