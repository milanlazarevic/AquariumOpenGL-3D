#include "../Header/bubble.hpp"
#include <GL/glew.h>

Bubble::Bubble(Model* bubble, float speed) :bubbleModel(bubble), x(0.0f), y(0.0f), z(0.0f), speed(speed), active(false) {}
Bubble::Bubble() : x(0.0f), y(0.0f), z(0.0f), speed(0.5f), active(false) {}

void Bubble::spawn(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->active = true;
}

void Bubble::update(float maxHeight) {
    std::cout << "Height" << y << "Max height" << maxHeight << endl;
    if (active) {
        y += speed;
        if (y > maxHeight) {
            active = false;
        }
    }
}

void Bubble::draw(Shader* shader, unsigned int VAO, unsigned int texture) {
    if (!active) return;

    bubbleMatrix = glm::mat4(1.0f);
    bubbleMatrix = glm::translate(bubbleMatrix, glm::vec3(x, y, z));

    bubbleMatrix = glm::scale(bubbleMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
    shader->use();

    shader->setBool("uUseTexture", false);
    shader->setVec3("uObjectColor", glm::vec3(0.0f, 0.0f, 0.3f)); // blago plava
    shader->setFloat("uAlpha", 0.3f);
    shader->setMat4("uM", bubbleMatrix);
    bubbleModel->Draw(*shader);
}
