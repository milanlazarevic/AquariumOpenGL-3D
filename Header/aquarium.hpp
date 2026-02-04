#pragma once
#include "../Header/util.hpp"
#include <iostream>
#include "../Header/renderer.hpp"
#include "../Header/model.hpp"
#include "../Header/fish.hpp"
#include "../Header/bubble.hpp"
#include "../Header/foodManager.hpp"



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
    void spawnBubbles(Bubble(&bubbles)[3], float fishX, float fishY, float fishZ);
    void createVAOs();
    void update();
    void render();
    void handleMovement();


    GLFWwindow* window;
    float screenWidth, screenHeight;
    glm::vec3 cameraPos;

    unsigned int grassTexture, grassRightTexture, sandTexture, woodTexture;
    unsigned int chestClosedTexture, chestOpenTexture;
    glm::mat4 goldenFishModelMatrix, clownFishModelMatrix, pinkCoralMatrix, seaGrassMatrix, chestBottomMatrix, chestLidMatrix;
    glm::mat4 farPlaneRectangle, bottomRectangle, leftWall, rightWall, frontWall = 0;
    glm::mat4 rightTopEdge, rightBottomEdge, leftTopEdge, leftBottomEdge, baseCoinMatrix;
    unsigned int bubbleTexture, foodTexture;
    unsigned int signatureTexture;
    float height, width, depth;

    Model* goldenFishModel;
    Model* clownFishModel;
    Model* pinkCoral;
    Model* seaGrass;
    Model* coinModel;
    Model* bubbleModel = new Model("res/bubble/bubble.obj");
    Model* fishFoodModel = new Model("res/food/10211_Watermelon_v1-L3.obj");

    Fish* clownFish;
    Fish* goldenFish;

    FoodManager* foodManager;

    Shader* unifiedShader; 
    Shader* textureShader;
    unsigned int VAOFarGlass;

    Bubble goldenBubbles[3] = {
           Bubble(bubbleModel,0.01f),
           Bubble(bubbleModel,0.02f),
           Bubble(bubbleModel,0.03f)
    };
    Bubble clownBubbles[3] = {
            Bubble(bubbleModel,0.01f),
            Bubble(bubbleModel,0.02f),
            Bubble(bubbleModel,0.03f)
    };

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