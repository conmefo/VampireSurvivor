#include "main.h"

#include <SFML/Graphics.hpp>

int runSfmlTest() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Test");
    window.setFramerateLimit(60);

    sf::CircleShape ball(50.f);
    ball.setFillColor(sf::Color(90, 200, 120));
    ball.setOrigin(ball.getRadius(), ball.getRadius());

    sf::Vector2f position(400.f, 300.f);
    sf::Vector2f velocity(220.f, 160.f);
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }
        }

        const float dt = clock.restart().asSeconds();
        position += velocity * dt;

        const float radius = ball.getRadius();
        const sf::Vector2u size = window.getSize();

        if (position.x - radius < 0.f || position.x + radius > size.x) {
            velocity.x = -velocity.x;
        }

        if (position.y - radius < 0.f || position.y + radius > size.y) {
            velocity.y = -velocity.y;
        }

        ball.setPosition(position);

        window.clear(sf::Color(35, 40, 50));
        window.draw(ball);
        window.display();
    }

    return 0;
}

int main() { return runSfmlTest(); }
