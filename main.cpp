#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#define HAVE_SFML
#include "utils/random.h"
#include "headers/solver.h"
#include "event.h"

constexpr int windowWidth  = 1000;
constexpr int windowHeight = 1000;
constexpr int frameRate    = 120;

static sf::Color getRainbow(float t)
{
    const float r = sin(t);
    const float g = sin(t + 0.33f * 2.0f * PI_f);
    const float b = sin(t + 0.66f * 2.0f * PI_f);
    return {static_cast<uint8_t>(255.0f * r * r),
            static_cast<uint8_t>(255.0f * g * g),
            static_cast<uint8_t>(255.0f * b * b)};
}

int main(int argc, char* argv[]) {
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Simple Physics Engine");
    window.setFramerateLimit(frameRate);
    EventHandler HandleEvent(window);

    // Utilities
    utils::Random randomizer;

    // Define walls: starting pos, length, thickness, angle
    Wall ramp1({500.f, 350.f}, 300.f, 5.f, -45.f);
    Wall ramp2({275.f, 400.f}, 300.f, 5.f, 30.f);
    std::vector<Wall> walls{ramp1, ramp2};
    
    // Initialize ball settings
    std::vector<VerletBall> balls;
    const float spawn_delay          = 0.025f;
    const float initial_speed        = 10.f;              // Ball speed in m/s
    const sf::Vector2f spawn_position{40.f, 150.f};
    const uint32_t max_balls         = 1200;
    balls.reserve(max_balls);

    // FPS calculations
    sf::Font font;
    font.loadFromFile("fonts/cmunrm.ttf");
    float fps = 0.0f, time_per_frame = 0.0f;
    sf::Text information_text("", font, 25);

    // Clocks
    sf::Clock ball_clock, fps_clock, total_time_clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            HandleEvent.closeWindow(event);
            HandleEvent.dragAndShoot<VerletBall>(event, balls);
        }

        if (balls.size() < max_balls) {
            if (ball_clock.getElapsedTime().asSeconds() >= spawn_delay) {
                const float random_radius    = randomizer.generateRandomFloat(2.f, 25.f);
                float t = total_time_clock.getElapsedTime().asSeconds();
                const sf::Color random_color = getRainbow(t);
                balls.emplace_back(random_radius, spawn_position, initial_speed, 0.f * (PI_f / 180.f));
                balls.back().setColor(random_color);
                ball_clock.restart();
            }
        }

        window.clear(sf::Color::Black);
        HandleEvent.drawDragArrow();
        //HandleEvent.drawWall(walls);
        HandleEvent.drawBall(balls);
        Solver::resolveCollisions<VerletBall>(balls);

        // Display text
        float totalElapsedTime = total_time_clock.getElapsedTime().asSeconds();
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << totalElapsedTime;
        time_per_frame = fps_clock.restart().asSeconds(); // Get time since last frame
        fps = 1.0f / time_per_frame;
        std::string FPS           = std::to_string(static_cast<int>(fps)) + " FPS";
        std::string object_count  = std::to_string(static_cast<int>(balls.size())) + " objects";
        std::string formatted_time = oss.str() + " sec"; // Convert the formatted string to a regular string
        information_text.setString(FPS + "\n" + object_count + "\n" + formatted_time);
        window.draw(information_text);

        window.display();
    }

    return 0;
}

// cmake --build .\build\ --config Debug; .\build\Debug\main.exe
