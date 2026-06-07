#include "main.h"
#include "States/StateManager.h"
#include "States/Intro/LoadingState.h"
#include "Core/Resources/ResourceManager.h"
#include "Core/Resources/ResourceIdentifiers.h"
#include "States/StateContext.h"
#include <SFML/Graphics.hpp>
#include <iostream>

int runSfmlTest()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Vampire Survivors Clone");
    window.setFramerateLimit(60);

    ResourceManager<sf::Texture, TextureID> textureManager;
    ResourceManager<sf::Font, FontID> fontManager;
    
    try
    {
        textureManager.Load(TextureID::Background, "Assets/background.png");
        textureManager.Load(TextureID::Title, "Assets/title.png");
        textureManager.Load(TextureID::Prompt, "Assets/press_start.png");
        
        textureManager.Load(TextureID::CharacterDecoration_0, "Assets/char_0.png");
        textureManager.Load(TextureID::CharacterDecoration_1, "Assets/char_1.png");
        textureManager.Load(TextureID::CharacterDecoration_2, "Assets/char_2.png");
    }
    catch (const std::exception& e)
    {
        std::cerr << "Initialization Error: " << e.what() << "\n";
    }

    StateManager stateManager;
    StateContext context(stateManager, textureManager, fontManager);

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

            stateManager.HandleInput(event);
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
