#pragma once
#include "ball.h"

class ImplicitEulerBall : public Ball {
private:
    const float gravity = acceleration.y; // Constant gravitational acceleration in pixels/s²
public:
    using Ball::Ball;

    void updatePosition(const int& windowWidth, const int& windowHeight) override {
        // Implicitly update velocity
        sf::Vector2f velocityNext = velocity + acceleration * deltaTime;

        // Update position using the new velocity
        sf::Vector2f position = getPosition();
        sf::Vector2f newPosition = position + velocityNext * deltaTime;

        if (newPosition.x + radius >= windowWidth) {
            velocityNext.x *= - RESTITUTION;
            newPosition.x = windowWidth - radius;
        } else if (newPosition.x - radius <= 0) {
            velocityNext.x *= - RESTITUTION;
            newPosition.x = radius;
        }

        if (newPosition.y + radius >= windowHeight) {
            velocityNext.y *= - RESTITUTION;
            velocityNext.x *= FRICTION_COEFFICIENT;
            newPosition.y = windowHeight - radius;
        } else if (newPosition.y - radius <= 0) {
            velocityNext.y *= - RESTITUTION;
            newPosition.y = radius;
        }

        // Step 4: Set the position and velocity
        circleObject.setPosition(newPosition);
        velocity = velocityNext; // Update velocity to the new velocity
    }
};
