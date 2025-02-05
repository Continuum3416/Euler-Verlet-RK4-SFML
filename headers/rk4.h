#pragma once
#include "ball.h"

struct State {
    sf::Vector2f position;
    sf::Vector2f velocity;
};

struct Derivative {
    sf::Vector2f dPosition;     // dx/dt = velocity
    sf::Vector2f dVelocity;     // dv/dt = acceleration
};

class RK4Ball : public Ball {
private:
    State state;
    Derivative Function(const State& initial, float dt, const Derivative& derivative) {
        State state;
        state.position = initial.position + derivative.dPosition * dt;
        state.velocity = initial.velocity + derivative.dVelocity * dt;

        Derivative output;
        output.dPosition = state.velocity;
        output.dVelocity = acceleration; // Constant acceleration due to gravity
        return output;
    }
public:
    RK4Ball(float radius, sf::Vector2f init_position, float init_speed, float angle)
        : Ball(radius, init_position, init_speed, angle) 
    {
        state.position = position;
        state.velocity = velocity;
    }

    [[nodiscard]] sf::Vector2f getPosition() const override 
    {
        return state.position;
    }
    
    void updatePosition() override 
    {
        Derivative a = Function(state, 0.f, Derivative());
        Derivative b = Function(state, deltaTime * 0.5f, a);
        Derivative c = Function(state, deltaTime * 0.5f, b);
        Derivative d = Function(state, deltaTime, c);

        sf::Vector2f dxdt = (a.dPosition + 2.f * (b.dPosition + c.dPosition) + d.dPosition) / 6.f;
        sf::Vector2f dvdt = (a.dVelocity + 2.f * (b.dVelocity + c.dVelocity) + d.dVelocity) / 6.f;

        state.position += dxdt * deltaTime;
        state.velocity += dvdt * deltaTime;
        
        circleObject.setPosition(state.position);
    }

    [[nodiscard]] sf::Vector2f getVelocity() const
    {
        return state.velocity;
    }

    [[nodiscard]] float getSpeed() const noexcept
    {
        return utils::norm2f(state.velocity);
    }

    friend class CollisionSolver<RK4Ball>;
};




template<>
class CollisionSolver<RK4Ball> {
CollisionSolver() = default;

public:
    static void handleBorderCollision(RK4Ball& ball, const int& windowWidth, const int& windowHeight){
        auto& state  = ball.state;
        auto& radius = ball.radius;

        if (state.position.x + radius > windowWidth) {
            state.position.x = windowWidth - radius;
            state.velocity.x *= -RESTITUTION;
        } else if (state.position.x - radius < 0) {
            state.position.x = radius;
            state.velocity.x *= -RESTITUTION;
        }

        if (state.position.y + radius > windowHeight) {
            state.position.y = windowHeight - radius;
            state.velocity.y *= -RESTITUTION;
            state.velocity.x *= FRICTION_COEFFICIENT;   // Apply friction on ground
        } else if (state.position.y - radius < 0) {
            state.position.y = radius;
            state.velocity.y *= -RESTITUTION;
        }
        ball.circleObject.setPosition(state.position);
    }

    static void resolvePairCollision(RK4Ball& ballA, RK4Ball& ballB) {
        sf::Vector2f& posA = ballA.state.position;
        sf::Vector2f& posB = ballB.state.position;
        sf::Vector2f& velA = ballA.state.velocity;
        sf::Vector2f& velB = ballB.state.velocity;

        sf::Vector2f delta = posB - posA;
        float dist2        = delta.x * delta.x + delta.y * delta.y;
        float min_dist     = ballA.radius + ballB.radius;

        if (dist2 < min_dist * min_dist) {
            float dist = std::sqrt(dist2);
            float overlap = min_dist - dist;
            sf::Vector2f normal = delta / dist;

            const float mass_ratioA = ballA.radius / min_dist;
            const float mass_ratioB = ballB.radius / min_dist;

            // sf::Vector2f correction = normal * RESTITUTION * overlap;
            sf::Vector2f correction = normal * overlap;

            posA -= correction * mass_ratioB;
            posB += correction * mass_ratioA;

            ballA.circleObject.setPosition(posA);
            ballB.circleObject.setPosition(posB);

            // Velocity calculation
            sf::Vector2f relative_velocity = velB - velA;
            float velocity_along_normal = utils::dot(relative_velocity, normal);

            // Only resolve if moving towards each other
            if (velocity_along_normal > 0) return;

            // Impulse scalar with energy loss
            float impulseScalar = -(1.f + RESTITUTION) * velocity_along_normal;
            sf::Vector2f impulse = normal * impulseScalar;

            velA -= impulse * mass_ratioB;
            velB += impulse * mass_ratioA;

        }
    }

    static void resolveWallCollision(RK4Ball& ball, Wall& wall) {
        sf::Vector2f closest_point = closestPointToWall(ball, wall);
        sf::Vector2f ball_to_closest = closest_point - ball.state.position;
        float dist = utils::norm2f(ball_to_closest);
        sf::Vector2f normal = utils::normalize(ball_to_closest);

        float overlap = ball.radius - dist;
        if (dist < ball.radius) {
            // Position correction (push ball out of wall)
            ball.state.position -= normal * overlap;
            ball.circleObject.setPosition(ball.state.position);

            // Reflect velocity using proper restitution
            float velocity_along_normal = utils::dot(ball.state.velocity, normal);
            ball.state.velocity -= (1.f + RESTITUTION) * velocity_along_normal * normal;
            ball.state.velocity *= wall.WALL_FRICTION;
        }
    }
};