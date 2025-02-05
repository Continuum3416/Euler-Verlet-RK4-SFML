#pragma once
#include "ball.h"
#define HAVE_SFML
#include "../utils/math.h"
#include "../utils/constants.h"

using namespace mathematical;

class Wall {
private:
    sf::RectangleShape rectangle;
    sf::Vector2f starting_position;
    sf::Vector2f unit_normal;
    float angle; // incline in radians
    float width;
    float length;
public:
    Wall(sf::Vector2f starting_position, float length, float width, float angle_degrees)
        : starting_position(starting_position),
        length(length),
        width(width),
        angle(angle_degrees * PI_f / 180.f) // Convert once here to radians
    {
        unit_normal = sf::Vector2f(std::cos(angle + PI_f / 2), std::sin(angle + PI_f / 2));
        rectangle.setSize(sf::Vector2f(length, width));
        rectangle.setPosition(starting_position);
        rectangle.setRotation(angle_degrees);
        setColor();

    }

    const float WALL_FRICTION = 0.98f;

    void setColor(sf::Color color = sf::Color::White) {
        rectangle.setFillColor(color);
    }

    [[nodiscard]] sf::Vector2f getUnitNormal() { return unit_normal;}
    [[nodiscard]] sf::Vector2f getStartingPoint(){ return starting_position;}
    [[nodiscard]] sf::Vector2f getEndingPoint(){ return starting_position + length * sf::Vector2f(std::cos(angle), std::sin(angle));}
    [[nodiscard]] float getIncline()   const { return angle;}
    [[nodiscard]] float getLength() const { return length;}
    [[nodiscard]] float getWidth() const { return width;}

    void draw(sf::RenderWindow& window) const 
    {
        window.draw(rectangle);
    }
};

template<typename T>
sf::Vector2f closestPointToWall(T& ball, Wall& wall){
    sf::Vector2f BallToWallStart = wall.getStartingPoint() - ball.getPosition();
    sf::Vector2f WallUnitVec = utils::normalize(wall.getEndingPoint() - wall.getStartingPoint());
    if(utils::dot(WallUnitVec, BallToWallStart) > 0){
        return wall.getStartingPoint();
    }

    sf::Vector2f WallEndToBall = ball.getPosition() - wall.getEndingPoint();
    if(utils::dot(WallUnitVec, WallEndToBall) > 0){
        return wall.getEndingPoint();
    }

    float closest_dist = utils::dot(WallUnitVec, BallToWallStart);
    sf::Vector2f ClosestVec = WallUnitVec * closest_dist;
    return (wall.getStartingPoint() - ClosestVec);
}