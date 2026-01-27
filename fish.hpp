#pragma once
#include "./Header/model.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>

class Fish {
public:
    Fish(std::string KURCINA);

    float getScale() const { return scale; }
    bool isFlipped() const { return flipped; }
    void checkChestCollision(bool isChestOpen);
    void grow();

    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();

    void checkBoundaries(float topBound, float bottomBound, float leftBound, float rightBound);
    void eat();
    void draw(Shader* shader);

private:
    glm::mat4 fishMatrix;
    Model* fishModel;
    float x, y, z;
    float scale;
    float speed;
    bool flipped;
};