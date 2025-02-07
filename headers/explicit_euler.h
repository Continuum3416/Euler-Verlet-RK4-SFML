#pragma once
#include "ball.h"


class EulerBall : public Ball{
private:
    const float gravity = acceleration.y; // Constant gravitational acceleration in pixels/s²
public:
    EulerBall(float radius, sf::Vector2f init_position, float init_speed, float angle)
        : Ball(radius, init_position, init_speed, angle)
    {}

    void updatePosition() {
        // Explicit Euler Integration: Update velocity first
        // v' = v + a dt
        // x' = x + v dt
        velocity += acceleration * deltaTime;
        position += velocity * deltaTime;
        circleObject.setPosition(position);
    }

    [[nodiscard]] sf::Vector2f getPosition() const
    {
        return position;
    }

    [[nodiscard]] sf::Vector2f getVelocity() const
    {
        return velocity;
    }

    [[nodiscard]] float getSpeed() const noexcept
    {
        return utils::norm2f(velocity);
    }

    void drawVelocityVector(sf::RenderWindow& window)
    {
        float length = getSpeed() / 5.f;
        sf::RectangleShape line;
        line.setSize(sf::Vector2f(length, 2.f));
        line.setPosition(position);
        line.setFillColor(sf::Color::Red);
        float angle = std::atan2(velocity.y, velocity.x);
        line.setRotation(angle * 180.f / PI_f);
        window.draw(line);
    }
    
    friend class CollisionSolver<EulerBall>;
};


template<>
class CollisionSolver<EulerBall> {
    CollisionSolver() = default;

public:
    static void handleBorderCollision(EulerBall& ball, const int windowWidth, const int windowHeight) 
    {
        auto& position = ball.position;
        auto& velocity = ball.velocity;
        const float radius = ball.radius;

        if (position.x + radius >= windowWidth) {
            position.x = windowWidth - radius;
            velocity.x *= -RESTITUTION;
        } else if (position.x - radius <= 0) {
            position.x = radius;
            velocity.x *= -RESTITUTION;
        }

        if (position.y + radius >= windowHeight) {
            position.y = windowHeight - radius;
            velocity.y *= -RESTITUTION;
            velocity.x *= FRICTION_COEFFICIENT;  // Apply friction on ground
        } else if (position.y - radius <= 0) {
            position.y = radius;
            velocity.y *= -RESTITUTION;
        }

        // Apply corrections to both circleObject and ball.position
        ball.circleObject.setPosition(position);
    }

    // Impulse based collision
    static void resolvePairCollision(EulerBall& ballA, EulerBall& ballB) 
    {
        sf::Vector2f& posA = ballA.position;
        sf::Vector2f& posB = ballB.position;
        sf::Vector2f& velA = ballA.velocity;
        sf::Vector2f& velB = ballB.velocity;

        sf::Vector2f delta = posB - posA;
        float dist2        = delta.x * delta.x + delta.y * delta.y;
        float min_dist     = ballA.radius + ballB.radius;

        if (dist2 < min_dist * min_dist) {
            float dist = std::sqrt(dist2);
            float overlap = min_dist - dist;
            sf::Vector2f normal = delta / dist;

            const float mass_ratioA = ballA.radius / min_dist;
            const float mass_ratioB = ballB.radius / min_dist;

            sf::Vector2f correction = normal * overlap;
            posA -= correction * mass_ratioB;
            posB += correction * mass_ratioA;

            sf::Vector2f relative_velocity = velB - velA;
            float velocity_along_normal = utils::dot(relative_velocity, normal);

            // Only resolve if moving towards each other
            if (velocity_along_normal < 0)
            {
                float impulse_scalar = -(1.f + RESTITUTION) * velocity_along_normal;
                sf::Vector2f impulse = impulse_scalar * normal;
                velA -= impulse * mass_ratioB;
                velB += impulse * mass_ratioA;
                
            }

            ballA.circleObject.setPosition(posA);
            ballB.circleObject.setPosition(posB);
        }

    }

    static void resolveWallCollision(EulerBall& ball, Wall& wall) 
    {
        sf::Vector2f closest_point = closestPointToWall(ball, wall);
        sf::Vector2f ball_to_closest = closest_point - ball.position;
        float dist = utils::norm2f(ball_to_closest);
        sf::Vector2f normal = utils::normalize(ball_to_closest);

        float overlap = ball.radius - dist;
        if (dist < ball.radius) {
            // Position correction (push ball out of wall)
            ball.position -= normal * overlap;
            ball.circleObject.setPosition(ball.position);

            // Reflect velocity using proper restitution
            float velocity_along_normal = utils::dot(ball.velocity, normal);
            ball.velocity -= (1.f + RESTITUTION) * velocity_along_normal * normal;
            ball.velocity *= wall.WALL_FRICTION;
        }
    }
};
