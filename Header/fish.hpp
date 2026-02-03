#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../Header/model.hpp"

class Fish {
public:
    Fish(Model* model, float x, float y, float z, float scale, float speed, float baseRotation);

    float getScale() const { return scale; }
    bool isFlipped() const { return flipped; }
    void checkChestCollision(bool isChestOpen);
    void grow();

    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    void moveBack();
    void moveFront();

    void checkBoundaries(float topBound, float bottomBound, float leftBound, float rightBound, float frontBound, float backBound);
    void calculateBoundingBox();
    void eat();
    void draw(Shader* shader);

private:
    float currentRotationX;
    float currentRotationY; 
    float currentRotationZ;
    float baseRotation;
    float minX, maxX; 
    float minY, maxY;  
    float minZ, maxZ;
    glm::mat4 fishMatrix;
    Model* fishModel;
    float scale;
    float speed;
    bool flipped;
    float x, y, z;
};