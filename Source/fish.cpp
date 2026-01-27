#include "./Header/fish.hpp"
#include <algorithm>

Fish::Fish(float x, float y, float speed, float scale, float minX, float maxX, float minY, float maxY, unsigned int texture)
    : x(0.0f), y(0.0f), scale(scale), speed(speed), flipped(false),
    minX(minX), maxX(maxX), minY(minY), maxY(maxY), texture(texture) {}

void Fish::moveLeft() {
    x -= speed;
    flipped = false;
}

void Fish::moveRight() {
    x += speed;
    flipped = true;
}

void Fish::moveUp() {
    y += speed;
}

void Fish::moveDown() {
    y -= speed;
}

void Fish::checkBoundaries(float topBound, float bottomBound, float leftBound, float rightBound) {
    float leftX = minX + x;
    float rightX = maxX + x;
    float topY = ( maxY * scale ) + y;
    float bottomY = ( minY * scale ) + y;

    if (leftX < leftBound) {
        x = leftBound - minX;
    }
    if (rightX > rightBound) {
        x = rightBound - maxX;
    }
    if (topY > topBound) {
        y = topBound - ( maxY * scale );
    }
    if (bottomY < bottomBound) {
        y = bottomBound - ( minY * scale );
    }
}

void Fish::checkChestCollision(bool isChestOpen) {
    const float chestMinX = 0.0f;
    const float chestMaxX = 0.4f;
    const float chestMinY = -0.85f;
    const float chestMaxY = isChestOpen ? -0.85f + ( 0.4f * 1.5f ) : -0.45f;

    float fishLeftX = minX + x;
    float fishRightX = maxX + x;
    float fishBottomY = ( minY * scale ) + y;
    float fishTopY = ( maxY * scale ) + y;

    bool collisionX = fishRightX >= chestMinX && fishLeftX <= chestMaxX;
    bool collisionY = fishTopY >= chestMinY && fishBottomY <= chestMaxY;

    if (collisionX && collisionY) {
        float overlapLeft = fishRightX - chestMinX;
        float overlapRight = chestMaxX - fishLeftX;
        float overlapBottom = fishTopY - chestMinY;
        float overlapTop = chestMaxY - fishBottomY;


        float minOverlapX = std::min(overlapLeft, overlapRight);
        float minOverlapY = std::min(overlapBottom, overlapTop);


        if (minOverlapX < minOverlapY) {
            if (overlapLeft < overlapRight) {
                x = chestMinX - maxX;
            }
            else {
                x = chestMaxX - minX;
            }
        }
        else {
            if (overlapBottom < overlapTop) {
                y = chestMinY - ( maxY * scale );
            }
            else {
                y = chestMaxY - ( minY * scale );
            }
        }
    }
}

void Fish::eat() {
    scale += 0.1f;
    if (scale > 2.0f) scale = 2.0f;
}

void Fish::draw(unsigned int shader, unsigned int VAO, unsigned int texture) {
    glUseProgram(shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shader, "texture0"), 0);
    glUniform1f(glGetUniformLocation(shader, "uS"), scale);
    glUniform1f(glGetUniformLocation(shader, "uX"), x);
    glUniform1f(glGetUniformLocation(shader, "uY"), y);
    glUniform1f(glGetUniformLocation(shader, "flipped"), flipped);
    glUniform1f(glGetUniformLocation(shader, "open"), false);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}