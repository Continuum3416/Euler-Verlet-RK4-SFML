#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "wall.h"

#define HAVE_SFML
#include "../utils/math.h"
#include "../utils/constants.h"

using namespace mathematical;
using namespace earth;

constexpr float SCALE                = 100.f;    // 1 meter = 100 pixels
constexpr float RESTITUTION          = 0.8f;     // Energy retention coefficient/response coefficient (0-1)
constexpr float FRICTION_COEFFICIENT = 0.5f;     // Friction coefficient for floor contact
constexpr float EPSILON              = 1e-4f;    // tolerance
constexpr float ENERGY_LOST_FACTOR   = 1.f - RESTITUTION * RESTITUTION;
const sf::Vector2f ACCELERATION      = SCALE * sf::Vector2f{0.f, g_f};


// Abstract class
class Ball {
protected:
    sf::CircleShape circleObject;
    float deltaTime;
    sf::VertexArray path;

    Ball(float radius, sf::Vector2f init_position, float init_speed, float angle): 
        radius(radius),
        position(init_position),
        velocity(std::cos(angle) * init_speed * SCALE, std::sin(angle) * init_speed * SCALE)
    {
        circleObject.setRadius(radius);
        circleObject.setOrigin(radius, radius);
        circleObject.setPosition(init_position);
        setColor();
        setStepSize(1.f / 120.f);
    }
public:
    const float  radius;
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration{ACCELERATION};

    void setColor() { circleObject.setFillColor(sf::Color(0, 176, 255));}
    void setColor(const sf::Color& color) {circleObject.setFillColor(color);}
    void setStepSize() {deltaTime = 1.f / 60.f;}
    void setStepSize(const float& dt) {deltaTime = dt;}

    virtual void updatePosition() = 0;

    [[nodiscard]] virtual sf::Vector2f getPosition() const
    {
        return circleObject.getPosition();
    }
    
    [[nodiscard]] float getStepSize() const
    {
        return deltaTime;
    }

    void drawPath(sf::RenderWindow& window) 
    {
        const sf::Color pathColor = circleObject.getFillColor();
        path.setPrimitiveType(sf::Lines);
        path.append(sf::Vertex(circleObject.getPosition(), pathColor)); // add current position to the path
        window.draw(path);
    }

    void draw(sf::RenderWindow& window) const 
    {
        window.draw(circleObject);
    }

};

template<typename T> class CollisionSolver{CollisionSolver() = default;};