#include "../Header/fish.hpp"
#include <algorithm>

Fish::Fish(Model* model, float x, float y, float z, float scale, float speed, float baseRotation)
    : fishModel(model), x(x), y(y), z(z), scale(scale), speed(speed), flipped(false), currentRotationY(0.0f), currentRotationZ(0.0f), currentRotationX(0.0f), baseRotation(baseRotation)
{
    calculateBoundingBox();
}

void Fish::moveLeft() {
    x += speed;
    flipped = false;
    currentRotationY = 180.0f; 
    currentRotationZ = 0.0f;
    currentRotationX = 0.0f;
}

void Fish::moveRight() {
    x -= speed;
    flipped = true;
    currentRotationY = 0.0f; 
    currentRotationZ = 0.0f;
    currentRotationX = 0.0f;
}

void Fish::moveUp() {
    y += speed;
    currentRotationX = -90.0f;
    //currentRotationY = 0.0f;
    //currentRotationZ = 0.0f;
}

void Fish::moveDown() {
    y -= speed;
  
    currentRotationX = 90.0f;
    //currentRotationY = 0.0f;
    //currentRotationZ = 0.0f;
}

void Fish::moveBack() {
    z -= speed;
    currentRotationY = 0.0f;
    currentRotationZ = -90.0f; 
    currentRotationX = 0.0f;
}

void Fish::moveFront() {
    z += speed;
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

//void Fish::checkChestCollision(bool isChestOpen) {
//    const float chestMinX = 0.0f;
//    const float chestMaxX = 0.4f;
//    const float chestMinY = -0.85f;
//    const float chestMaxY = isChestOpen ? -0.85f + ( 0.4f * 1.5f ) : -0.45f;
//
//    float fishLeftX = minX + x;
//    float fishRightX = maxX + x;
//    float fishBottomY = ( minY * scale ) + y;
//    float fishTopY = ( maxY * scale ) + y;
//
//    bool collisionX = fishRightX >= chestMinX && fishLeftX <= chestMaxX;
//    bool collisionY = fishTopY >= chestMinY && fishBottomY <= chestMaxY;
//
//    if (collisionX && collisionY) {
//        float overlapLeft = fishRightX - chestMinX;
//        float overlapRight = chestMaxX - fishLeftX;
//        float overlapBottom = fishTopY - chestMinY;
//        float overlapTop = chestMaxY - fishBottomY;
//
//
//        float minOverlapX = std::min(overlapLeft, overlapRight);
//        float minOverlapY = std::min(overlapBottom, overlapTop);
//
//
//        if (minOverlapX < minOverlapY) {
//            if (overlapLeft < overlapRight) {
//                x = chestMinX - maxX;
//            }
//            else {
//                x = chestMaxX - minX;
//            }
//        }
//        else {
//            if (overlapBottom < overlapTop) {
//                y = chestMinY - ( maxY * scale );
//            }
//            else {
//                y = chestMaxY - ( minY * scale );
//            }
//        }
//    }
//}

void Fish::eat() {
    scale += 0.1f;
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