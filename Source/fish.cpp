#include "../Header/fish.hpp"
#include <algorithm>

Fish::Fish(Model* model, float x, float y, float z, float scale, float speed, float baseRotation)
    : fishModel(model), currentDirection(FishDirection::RIGHT), x(x), y(y), z(z), scale(scale), speed(speed), currentRotationY(0.0f), currentRotationZ(0.0f), currentRotationX(0.0f), baseRotation(baseRotation)
{
    calculateBoundingBox();
}

void Fish::moveLeft() {
    x += speed;
    currentDirection = FishDirection::LEFT;
    currentRotationY = 180.0f; 
    currentRotationZ = 0.0f;
    currentRotationX = 0.0f;
}

void Fish::moveRight() {
    x -= speed;
    currentDirection = FishDirection::RIGHT;
    currentRotationY = 0.0f; 
    currentRotationZ = 0.0f;
    currentRotationX = 0.0f;
}

void Fish::moveUp() {
    y += speed;
    currentDirection = FishDirection::UP;
    currentRotationX = -90.0f;
    //currentRotationY = 0.0f;
    //currentRotationZ = 0.0f;
}

void Fish::moveDown() {
    y -= speed;
    currentDirection = FishDirection::DOWN;
    currentRotationX = 90.0f;
    //currentRotationY = 0.0f;
    //currentRotationZ = 0.0f;
}

void Fish::moveBack() {
    z -= speed;
    currentDirection = FishDirection::BACK;
    currentRotationY = 0.0f;
    currentRotationZ = -90.0f; 
    currentRotationX = 0.0f;
}

void Fish::moveFront() {
    z += speed;
    currentDirection = FishDirection::FRONT;
    currentRotationY = 0.0f;
    currentRotationZ = 90.0f; 
    currentRotationX = 0.0f;
}

void Fish::calculateBoundingBox() {
    minX = minY = minZ = std::numeric_limits<float>::max();
    maxX = maxY = maxZ = std::numeric_limits<float>::lowest();

    for (const auto& mesh : fishModel->meshes) {
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

void Fish::checkBoundaries(float topBound, float bottomBound, float leftBound, float rightBound, float frontBound, float backBound) {
    float radius = std::max({
        std::abs(maxX - minX),
        std::abs(maxY - minY),
        std::abs(maxZ - minZ)
    }) * scale * 0.5f;

    radius *= 0.77f; 

    if (x - radius < leftBound) {
        x = leftBound + radius;
    }
    if (x + radius > rightBound) {
        x = rightBound - radius;
    }

    if (y - radius < bottomBound) {
        y = bottomBound + radius;
    }
    if (y + radius > topBound) {
        y = topBound - radius;
    }

    if (z - radius < backBound) {
        z = backBound + radius;
    }
    if (z + radius > frontBound) {
        z = frontBound - radius;
    }
}

glm::vec3 Fish::getBubbleSpawnPosition() {
    float bubbleX, bubbleY, bubbleZ;

    // Y is always at the middle
    bubbleY = ( ( minY + maxY ) / 2.0f ) * scale + y;

    // X and Z depend on direction (spawn bubbles at the "mouth" - opposite of movement)
    switch (currentDirection) {
    case FishDirection::LEFT:
        bubbleX = maxX * scale + x;  // Left side
        bubbleZ = maxZ * scale + z;  // Front
        break;
    case FishDirection::RIGHT:
        bubbleX = minX * scale + x;  // Right side
        bubbleZ = maxZ * scale + z;  // Front
        break;
    case FishDirection::UP:
        bubbleX = ( ( minX + maxX ) / 2.0f ) * scale + x;  // Middle
        bubbleZ = maxZ * scale + z;  // Front
        break;
    case FishDirection::DOWN:
        bubbleX = ( ( minX + maxX ) / 2.0f ) * scale + x;  // Middle
        bubbleZ = minZ * scale + z;  // Front
        break;
    case FishDirection::FRONT:
        bubbleX = ( ( minX + maxX ) / 2.0f ) * scale + x;  // Middle
        bubbleZ = maxZ * scale + z;  // Front
        break;
    case FishDirection::BACK:
        bubbleX = ( ( minX + maxX ) / 2.0f ) * scale + x;  // Middle
        bubbleZ = minZ * scale + z;  // Back
        break;
    }


    return glm::vec3(bubbleX, bubbleY, bubbleZ);
}

bool Fish::checkCollisionWithObject(float objMinX, float objMaxX,
                                     float objMinY, float objMaxY,
                                     float objMinZ, float objMaxZ) {
    float fishWorldMinX = minX * scale + x;
    float fishWorldMaxX = maxX * scale + x;
    float fishWorldMinY = minY * scale + y;
    float fishWorldMaxY = maxY * scale + y;
    float fishWorldMinZ = minZ * scale + z;
    float fishWorldMaxZ = maxZ * scale + z;

    return ( fishWorldMinX <= objMaxX && fishWorldMaxX >= objMinX ) &&
        ( fishWorldMinY <= objMaxY && fishWorldMaxY >= objMinY ) &&
        ( fishWorldMinZ <= objMaxZ && fishWorldMaxZ >= objMinZ );
}

void Fish::eat() {
    scale += 0.001f;
    if (scale > 2.0f) scale = 2.0f;
}

void Fish::draw(Shader* shader) {
    fishMatrix = glm::mat4(1.0f);
    fishMatrix = glm::translate(fishMatrix, glm::vec3(x, y, z));
    fishMatrix = glm::rotate(fishMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Bazna rotacija
    if (baseRotation != 0.0f) {
        fishMatrix = glm::rotate(fishMatrix, glm::radians(baseRotation), glm::vec3(0.0f, 0.0f, 1.0f));
    }
 
    fishMatrix = glm::rotate(fishMatrix, glm::radians(currentRotationY), glm::vec3(0.0f, 0.0f, 1.0f)); 
    fishMatrix = glm::rotate(fishMatrix, glm::radians(currentRotationZ), glm::vec3(0.0f, 0.0f, 1.0f)); 
    if (baseRotation != 0.0f) {
        fishMatrix = glm::rotate(fishMatrix, glm::radians(currentRotationX), glm::vec3(1.0f, 0.0f, 0.0f));
    }
    else {
        fishMatrix = glm::rotate(fishMatrix, glm::radians(-currentRotationX), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    fishMatrix = glm::scale(fishMatrix, glm::vec3(scale, scale, scale));

    shader->setBool("uUseTexture", true);
    shader->setMat4("uM", fishMatrix);
    fishModel->Draw(*shader);
}