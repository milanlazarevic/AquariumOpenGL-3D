#pragma once
#include "../Header/util.hpp"
#include <iostream>
#include "../Header/renderer.hpp"
#include "../Header/model.hpp"



class Aquarium {
public: 
    Aquarium(GLFWwindow* window, float wallWidth, float screenWidth, float screenHeight);

    bool initialize();
    void run();
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
    void loadTextures();
    void setup();
    void createSandMash(float bottomWidth, float bottomLength);
    void processInput(GLFWwindow* window, int key, int scancode, int action, int mods);
    void processTexture(unsigned& texture, const char* filepath);
    void createShaders();
    void createVAOs();
    void update();
    void render();
    void handleMovement();


    GLFWwindow* window;
    float screenWidth, screenHeight;

    unsigned int grassTexture, grassRightTexture, sandTexture;
    unsigned int chestClosedTexture, chestOpenTexture;
    glm::mat4 goldenFishModelMatrix, clownFishModelMatrix;
    glm::mat4 farPlaneRectangle, bottomRectangle, leftWall, rightWall, frontWall = 0;
    glm::mat4 rightTopEdge, rightBottomEdge, leftTopEdge, leftBottomEdge;
    unsigned int bubbleTexture, foodTexture;
    unsigned int signatureTexture;

    Model* goldenFish;

    Shader* unifiedShader; 
    Shader* textureShader;
    unsigned int VAOFarGlass;


    unsigned int VAOrect, VAOrectRight, VAOrectBottom;
    unsigned int VAOglassOverlay, VAOsandOverlay;
    unsigned int VAOgrassOverlay, VAOgrass2Overlay;
    unsigned int VAOchest, VAOclownFish, VAOgoldenFish;
    unsigned int VAObubble, VAOclownBubble, VAOfoodParticle;
    unsigned int VAOsignature;


    unsigned VAOsand = 0;
    size_t sandIndexCount = 0;
    glm::mat4 sand = 0;

    bool isChestOpen;

    float wallWidth;
};