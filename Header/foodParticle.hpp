#pragma once
#include "../Header/model.hpp"
#include <glm/glm.hpp>

class FoodParticle
{
public:
    FoodParticle(Model* model, float speed, float x, float y, float z);

    float getX() const { return x; }
    float getY() const { return y; }
    float getZ() const { return z; }
    bool isActive() const { return active; }
    bool isEaten() const { return eaten; }

    void update(float bottomBound);
    void calculateBoundingBox();
    void markAsEaten() { eaten = true; active = false; }
    void draw(Shader* shader);

    bool checkCollision(float fishXMin, float fishXMax,
                        float fishYMin, float fishYMax,
                        float fishZMin, float fishZMax);
private:
    float x, y, z;
    float minX, maxX;
    float minY, maxY;
    float minZ, maxZ;
    float speed;
    bool eaten;
    bool active;
    glm::mat4 particleMatrix;
    Model* particleModel;
};

