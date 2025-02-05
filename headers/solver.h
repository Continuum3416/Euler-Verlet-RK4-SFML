#pragma once
#include "verlet.h"
#include "explicit_euler.h"
#include "rk4.h"
#include "wall.h"

const int width = 1000;
const int height = 1000;

class Solver{
private:
    Solver() = default;
    static const uint16_t MAX_ITERATIONS = 1;
public:
    template <typename T> 
    static void resolveCollisions(std::vector<T>& balls, std::vector<Wall>& walls) {
        for(size_t n{0}; n < MAX_ITERATIONS; ++n){
            for (size_t i{0}; i < balls.size(); ++i) {
                // Resolve border collisions
                CollisionSolver<T>::handleBorderCollision(balls[i], width, height);

                // Resolve ball-ball collisions
                for (size_t j{i + 1}; j < balls.size(); ++j) {
                    CollisionSolver<T>::resolvePairCollision(balls[i], balls[j]);
                }

                // Resolve ball-wall collisions
                for (size_t j{0}; j < walls.size(); ++j) {
                    CollisionSolver<T>::resolveWallCollision(balls[i], walls[j]);
                }
            }
        }
    }

    template <typename T> 
    static void resolveCollisions(std::vector<T>& balls) {
        for(size_t n{0}; n < MAX_ITERATIONS; ++n){
            for (size_t i{0}; i < balls.size(); ++i) {
                // Resolve border collisions
                CollisionSolver<T>::handleBorderCollision(balls[i], width, height);

                // Resolve ball-ball collisions
                for (size_t j{i + 1}; j < balls.size(); ++j) {
                    CollisionSolver<T>::resolvePairCollision(balls[i], balls[j]);
                }
            }
        }
    }
};