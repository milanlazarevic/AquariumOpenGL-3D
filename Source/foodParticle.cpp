#include "../Header/foodParticle.hpp"
#include <GL/glew.h>


const float FOOD_WIDTH = 0.05f;
const float FOOD_HEIGHT = 0.09f;

FoodParticle::FoodParticle(Model* model, float speed, float x, float y, float z)
    : x(x), y(y),z(z),particleModel(model), speed(speed), eaten(false), active(true) {
    calculateBoundingBox();
}

void FoodParticle::update(float bottomBound) {
    if (active && !eaten) {
        y += ( y < bottomBound ) ? 0.0f : speed;
    }
}
void FoodParticle::calculateBoundingBox() {
    minX = minY = minZ = std::numeric_limits<float>::max();
    maxX = maxY = maxZ = std::numeric_limits<float>::lowest();

    for (const auto& mesh : particleModel->meshes) {
        for (const auto& vertex : mesh.vertices) {
            minX = std::min(minX, vertex.Position.x);
            maxX = std::max(maxX, vertex.Position.x);
            minY = std::min(minY, vertex.Position.y);
            maxY = std::max(maxY, vertex.Position.y);
            minZ = std::min(minZ, vertex.Position.z);
            maxZ = std::max(maxZ, vertex.Position.z);
        }
    }
}

void FoodParticle::draw(Shader* shader) {
    if (!active || eaten) return;

    if (!active) return;

    particleMatrix = glm::mat4(1.0f);
    particleMatrix = glm::translate(particleMatrix, glm::vec3(x, y, z));

    particleMatrix = glm::scale(particleMatrix, glm::vec3(0.01f, 0.01f, 0.01f));
    shader->use();

    shader->setBool("uUseTexture", true);
    shader->setMat4("uM", particleMatrix);
    particleModel->Draw(*shader);
}

bool FoodParticle::checkCollision(float fishXMin, float fishXMax,
                                   float fishYMin, float fishYMax,
                                   float fishZMin, float fishZMax) {
    glm::vec3 scale(0.01f, 0.01f, 0.01f);
    glm::vec3 position(x, y, z);

    // Transform the bounding box
    float foodXMin = minX * scale.x + position.x;
    float foodXMax = maxX * scale.x + position.x;
    float foodYMin = minY * scale.y + position.y;
    float foodYMax = maxY * scale.y + position.y;
    float foodZMin = minZ * scale.z + position.z;
    float foodZMax = maxZ * scale.z + position.z;

    // Check overlap on all three axes
    return ( fishXMin <= foodXMax && fishXMax >= foodXMin ) &&
        ( fishYMin <= foodYMax && fishYMax >= foodYMin ) &&
        ( fishZMin <= foodZMax && fishZMax >= foodZMin );
}
