#pragma once
#include "../Header/model.hpp"
#include <glm/glm.hpp>

class Bubble {
public:
    Bubble(Model* model, float speed);
    Bubble();
    float getX() const { return x; }
    float getY() const { return y; }
    float getZ() const { return z; }
    void spawn(float x, float y, float z);
    void update(float maxHeight);
    void draw(Shader* shader, unsigned int VAO, unsigned int texture);
    bool isActive() const { return active; }

private:
    glm::mat4 bubbleMatrix;
    Model* bubbleModel;
    float x, y, z;
    float speed;
    bool active;
};
