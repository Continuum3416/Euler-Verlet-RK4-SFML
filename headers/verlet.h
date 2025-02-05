#pragma once
#include "ball.h"
#include "wall.h"

class VerletBall : public Ball {
private:
    sf::Vector2f previous_position;
public:
    // Constructor delegation
    VerletBall(float radius, sf::Vector2f init_position, float init_speed, float angle)
        : Ball(radius, init_position, init_speed, angle) 
    {
        previous_position = position - velocity * deltaTime;
    }

    void updatePosition() override {
        // Verlet Integration: derived from the second derivative
        // x(n+1) = 2 * x(n) - x(n-1) + a * dt^2
        sf::Vector2f temp_position = position;
        position = 2.f * position - previous_position + acceleration * (deltaTime * deltaTime);
        previous_position = temp_position;
        circleObject.setPosition(position);
    }

    [[nodiscard]] sf::Vector2f getVelocity() const 
    {
        return (position - previous_position) / deltaTime;
    }

    [[nodiscard]] float getSpeed() const noexcept
    {
        sf::Vector2f v = getVelocity();
        return utils::norm2f(v);
    }

   friend class CollisionSolver<VerletBall>;
};



template<>
class CollisionSolver<VerletBall> {
    CollisionSolver() = default;
public:
    static void handleBorderCollision(VerletBall& ball, const int& windowWidth, const int& windowHeight) {
        sf::Vector2f& position          = ball.position;
        sf::Vector2f& previous_position = ball.previous_position;
        sf::Vector2f  current_velocity  = position - previous_position;
        auto& radius = ball.radius;

        // Handle wall collisions
        if (position.x + radius > windowWidth) {
            position.x = windowWidth - radius;
            previous_position.x = position.x + RESTITUTION * current_velocity.x;
        } else if (position.x - radius < 0) {
            position.x = radius;
            previous_position.x = position.x + RESTITUTION * current_velocity.x;
        }

        // Handle floor collision
        if (position.y + radius > windowHeight) {
            position.y = windowHeight - radius;
            previous_position.y = position.y + RESTITUTION * current_velocity.y;

            // Apply friction on the ground
            previous_position.x += FRICTION_COEFFICIENT * current_velocity.x;

        } else if (position.y - radius < 0) {
            position.y = radius;
            previous_position.y = position.y + RESTITUTION * current_velocity.y;
        }

        ball.circleObject.setPosition(position);
    }

    // Position-based collision
    static void resolvePairCollision(VerletBall& ballA, VerletBall& ballB) {
        sf::Vector2f& posA = ballA.position;
        sf::Vector2f& posB = ballB.position;

        sf::Vector2f delta = posB - posA;
        float dist2        = delta.x * delta.x + delta.y * delta.y;
        float min_dist     = ballA.radius + ballB.radius;

        // Check if there is overlap
        if (dist2 < min_dist * min_dist) {
            float dist          = std::sqrt(dist2);
            float overlap       = min_dist - dist;
            sf::Vector2f normal = delta / dist;

            const float mass_ratioA = ballA.radius / min_dist;
            const float mass_ratioB = ballB.radius / min_dist;

            sf::Vector2f correction = normal * RESTITUTION * overlap;
            posA -= correction * mass_ratioB;
            posB += correction * mass_ratioA;
            
            ballA.circleObject.setPosition(posA);
            ballB.circleObject.setPosition(posB);
        }
    }

    static void resolveWallCollision(VerletBall& ball, Wall& wall) {
        sf::Vector2f closest_point   = closestPointToWall(ball, wall);
        sf::Vector2f ball_to_closest = closest_point - ball.position;
        float dist    = utils::norm2f(ball_to_closest);
        float overlap = ball.radius - dist;

        if (dist < ball.radius) {
            // Calculate the normal vector from the ball to the closest point on the wall
            sf::Vector2f normal = utils::normalize(ball_to_closest);

            // Calculate the correction vector based on the overlap
            sf::Vector2f correction = normal * overlap;

            // Adjust the ball's position to resolve the collision
            ball.position -= correction;
            ball.circleObject.setPosition(ball.position);

            // Adjust the ball's velocity to reflect the bounce off the wall
            sf::Vector2f current_position = ball.position;
            sf::Vector2f current_velocity = current_position - ball.previous_position;
            sf::Vector2f reflected_velocity = current_velocity - 2.f * utils::dot(current_velocity, normal) * normal;
            ball.previous_position = ball.position - reflected_velocity;

        }
    }
};