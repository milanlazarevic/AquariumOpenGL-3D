#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../Header/model.hpp"

enum class FishDirection {
    LEFT,
    RIGHT,
    UP,
    DOWN,
    FRONT,
    BACK
};

class Fish {
public:
    Fish(Model* model, float x, float y, float z, float scale, float speed, float baseRotation);

    float getScale() const { return scale; }
    bool checkCollisionWithObject(float objMinX, float objMaxX,
                                     float objMinY, float objMaxY,
                                     float objMinZ, float objMaxZ);
    void grow();

    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    void moveBack();
    void moveFront();

    void checkBoundaries(float topBound, float bottomBound, float leftBound, float rightBound, float frontBound, float backBound);
    glm::vec3 getBubbleSpawnPosition();
    FishDirection getCurrentDirection() const { return currentDirection; }
    void calculateBoundingBox();
    void eat();
    void draw(Shader* shader);
    float minX, maxX; 
    float minY, maxY;  
    float minZ, maxZ;
    float x, y, z;

private:
    FishDirection currentDirection;
    float currentRotationX;
    float currentRotationY; 
    float currentRotationZ;
    float baseRotation;
    glm::mat4 fishMatrix;
    Model* fishModel;
    float scale;
    float speed;
};