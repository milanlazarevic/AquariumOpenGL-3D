#include "../Header/aquarium.hpp"
#include "../Header/shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Aquarium::Aquarium(GLFWwindow* window, float wallWidth, float screenWidth, float screenHeight)
    : window(window),
    screenWidth(screenWidth), screenHeight(screenHeight),
    grassTexture(0), grassRightTexture(0), sandTexture(0),
    chestClosedTexture(0), chestOpenTexture(0),
    farPlaneRectangle(0), bottomRectangle(0),
    bubbleTexture(0), foodTexture(0), signatureTexture(0),
    VAOrect(0), VAOrectRight(0), VAOrectBottom(0),
    VAOFarGlass(0), frontWall(0), rightTopEdge(0),leftBottomEdge(0),
    goldenFish(0), leftTopEdge(0), leftWall(0), rightBottomEdge(0),
	VAOglassOverlay(0), VAOsandOverlay(0), rightWall(0), textureShader(0), unifiedShader(0),
	VAOgrassOverlay(0), VAOgrass2Overlay(0),
	VAOchest(0), VAOclownFish(0), VAOgoldenFish(0),
	VAObubble(0), VAOclownBubble(0), VAOfoodParticle(0),
	VAOsignature(0),
	isChestOpen(true),
	wallWidth(wallWidth) 
{
};

void Aquarium::processTexture(unsigned& texture, const char* filepath) {
    texture = loadImageToTexture(filepath); // Učitavanje teksture
    if (texture == 0) {
        std::cout << "GRESKA: Tekstura nije ucitana!" << "\n";
        return;
    }

    std::cout << "Tekstura uspesno ucitana, ID: " << texture << std::endl;
    glBindTexture(GL_TEXTURE_2D, texture); // Vezujemo se za teksturu kako bismo je podesili

    // Generisanje mipmapa - predefinisani različiti formati za lakše skaliranje po potrebi (npr. da postoji 32 x 32 verzija slike, ali i 16 x 16, 256 x 256...)
    glGenerateMipmap(GL_TEXTURE_2D);

    // Podešavanje strategija za wrap-ovanje - šta da radi kada se dimenzije teksture i poligona ne poklapaju
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // S - tekseli po x-osi
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // T - tekseli po y-osi

    // Podešavanje algoritma za smanjivanje i povećavanje rezolucije: nearest - bira najbliži piksel, linear - usrednjava okolne piksele
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Aquarium::loadTextures() {
    goldenFishModel = new Model("res/goldFish/12265_Fish_v1_L2.obj");
    clownFishModel = new Model("res/clownFish/13006_Blue_Tang_v1_l3.obj");
    pinkCoral = new Model("res/pinkCoral/21488_Tree_Coral_v2_NEW.obj");
    seaGrass = new Model("res/seaGrass/10010_Coral_v1_L3.obj");
    coinModel = new Model("res/treasure/13457_Pile_of_Treasure_v1_L1.obj");
    processTexture(signatureTexture, "res/MilanLazarevicSV2-2022.png");
    processTexture(sandTexture, "res/sandTexture.jpg");
    processTexture(woodTexture, "res/woodTexture.jpg");
}

void Aquarium::processInput(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        isChestOpen = !isChestOpen;
    }
    if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
        spawnBubbles(goldenBubbles, goldenFish->isFlipped() ? (goldenFish->minX ) * goldenFish->getScale() + goldenFish->x : ( goldenFish->maxX) * goldenFish->getScale() + goldenFish->x, ( goldenFish->minY ) * goldenFish->getScale() + goldenFish->y, ( goldenFish->maxZ  ) *goldenFish->getScale() + goldenFish->z);
    }
    if (key == GLFW_KEY_I && action == GLFW_PRESS) {
        spawnBubbles(clownBubbles, clownFish->isFlipped() ? ( clownFish->minX ) * clownFish->getScale() + clownFish->x : ( clownFish->maxX ) * clownFish->getScale() + clownFish->x, ( clownFish->maxY / 2.0f ) * clownFish->getScale() + clownFish->y, ( clownFish->maxZ ) * clownFish->getScale() + clownFish->z);
    }
    //if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
    //    foodManager->spawnFood();
    //}
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void::Aquarium::createShaders() {
     textureShader = new Shader("Shaders/texture.vert", "Shaders/texture.frag");
     textureShader->use();
     textureShader->setInt("uTexture", 0);
     unifiedShader = new Shader("Shaders/basic.vert", "Shaders/basic.frag");

     // Setup 3D shader
     unifiedShader->use();
     unifiedShader->setVec3("uLightPos", 0, 1, 3);
     unifiedShader->setVec3("uViewPos", 0, 0, 5);
     unifiedShader->setVec3("uLightColor", 1, 1, 1);
     glm::mat4 projection = glm::perspective(glm::radians(45.0f), screenWidth / screenHeight, 0.1f, 100.0f);
     unifiedShader->setMat4("uP", projection);
     glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
     unifiedShader->setMat4("uV", view);
}

void::Aquarium::createSandMash(float bottomWidth, float bottomLength) {
    const int N = 100;         
    const float sizeX = bottomWidth;
    const float sizeZ = bottomLength;

    std::vector<float> vertices;
    std::vector<int> indices;

    for (int z = 0; z < N; z++) {
        for (int x = 0; x < N; x++) {
            float xpos = ( (float)x / ( N - 1 ) - 0.5f ) * sizeX;
            float zpos = ( (float)z / ( N - 1 ) - 0.5f ) * sizeZ;

            float y = ( ( rand() % 100 ) / 1000.0f ); 

            float u = (float)x / ( N - 1 );
            float v = (float)z / ( N - 1 );

            vertices.insert(vertices.end(), {
                xpos, y, zpos,
                0.0f, 1.0f, 0.0f, 
                u, v
            });
        }
    }

    for (int z = 0; z < N - 1; z++) {
        for (int x = 0; x < N - 1; x++) {
            int i0 = z * N + x;
            int i1 = i0 + 1;
            int i2 = i0 + N;
            int i3 = i2 + 1;

            indices.insert(indices.end(), { i0, i2, i1 });
            indices.insert(indices.end(), { i1, i2, i3 });
        }
    }

    sandIndexCount = indices.size();
    VAOsand = Renderer::createIndexedVAO(vertices, indices);
   
}

void::Aquarium::setup() {
    goldenFish = new Fish(goldenFishModel, 0.0f, 0.0f, -2.0f, 0.03f, 0.05f, 0.0f);
    clownFish = new Fish(clownFishModel, -1.0f, 0.0f, -2.0f, 0.09f, 0.06f, -90.0f);

    pinkCoralMatrix = glm::mat4(1.0f);
    pinkCoralMatrix = glm::translate(pinkCoralMatrix, glm::vec3(2.5f, -2.0f, -3.8f));
    //pinkCoralMatrix = glm::rotate(pinkCoralMatrix, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    pinkCoralMatrix = glm::scale(pinkCoralMatrix, glm::vec3(0.13f, 0.13f, 0.13f));

    seaGrassMatrix = glm::mat4(1.0f);
    seaGrassMatrix = glm::translate(seaGrassMatrix, glm::vec3(-2.5f, -2.0f, -3.8f));
    seaGrassMatrix = glm::rotate(seaGrassMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    seaGrassMatrix = glm::scale(seaGrassMatrix, glm::vec3(0.05f, 0.05f, 0.05f));

    chestBottomMatrix = glm::mat4(1.0f);
    chestBottomMatrix = glm::translate(chestBottomMatrix, glm::vec3(2.5f, -2.0f, -1.5f));

    chestLidMatrix = glm::mat4(1.0f);
    chestLidMatrix = glm::translate(chestLidMatrix, glm::vec3(2.5f, -1.35f, -1.5f));
    chestLidMatrix = glm::scale(chestLidMatrix, glm::vec3(1.0f, 0.3f, 1.0f));

    baseCoinMatrix = glm::mat4(1.0f);
    baseCoinMatrix = glm::translate(baseCoinMatrix, glm::vec3(2.4f, -1.5f, -1.3f));
    baseCoinMatrix = glm::rotate(baseCoinMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    baseCoinMatrix = glm::rotate(baseCoinMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    baseCoinMatrix = glm::scale(baseCoinMatrix, glm::vec3(0.02f, 0.02f, 0.02f));

    // far plane glass
    cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
    float fov = 45.0f;
    float aspect = screenWidth / screenHeight;

    // Dubina akvarijuma
    float aquariumDepth = 10.0f;

    // Automatski kalkuliši dimenzije tako da pokriva ~90% ekrana
    float poleSize = 0.2f;
    float backWallHeight = 2.0f * aquariumDepth * tan(glm::radians(fov / 2.0f)) * 0.5f;
    float backWallWidth = ( backWallHeight * 1.05f ) * aspect * 0.9f;

    farPlaneRectangle = glm::mat4(1.0f);
    farPlaneRectangle = glm::translate(farPlaneRectangle,
                                       glm::vec3(0.0f, 0.0f, cameraPos.z - aquariumDepth));
    farPlaneRectangle = glm::scale(farPlaneRectangle,
                                   glm::vec3(backWallWidth, backWallHeight, 0.1f));

    frontWall = glm::mat4(1.0f);
    frontWall = glm::translate(frontWall, glm::vec3(0.0f, 0.0f, 0.0f));
    frontWall = glm::scale(frontWall, glm::vec3(backWallWidth, backWallHeight, 0.1f));


    // bottom rectangle
    // Camera at z=5, back wall at z=-5
    // If you want the aquarium to start at, say, z=0 (front) and go to z=-5 (back):
    float frontZ = 0.0f; // or wherever your front face should be
    float backZ = cameraPos.z - aquariumDepth; // = -5


    // Distance from camera to FRONT edge
    float distanceToFront = cameraPos.z - frontZ; // e.g., 5 - 0 = 5

    // Width at the front edge to match viewport
    float bottomWidth = 2.0f * distanceToFront * tan(glm::radians(fov / 2.0f)) * aspect;

    height = backWallHeight;
    width = bottomWidth;
    depth = backZ;
    // Length: from front to back
    float bottomLength = frontZ - backZ; // e.g., 0 - (-5) = 5

    // Position: center the bottom between front and back
    float centerZ = ( frontZ + backZ ) / 2.0f; // e.g., (0 + (-5))/2 = -2.5
    // left and right wall
    leftWall = glm::mat4(1.0f);
    leftWall = glm::translate(leftWall, glm::vec3(bottomWidth / 2.0f - poleSize / 2.0f, 0.0f, -bottomLength / 2.0f));
    leftWall = glm::rotate(leftWall, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    leftWall = glm::scale(leftWall, glm::vec3(bottomLength, backWallHeight, 0.1f));

    rightWall = glm::mat4(1.0f);
    rightWall = glm::translate(rightWall, glm::vec3(-bottomWidth / 2.0f + poleSize / 2.0f, 0.0f, -bottomLength / 2.0f));
    rightWall = glm::rotate(rightWall, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rightWall = glm::scale(rightWall, glm::vec3(bottomLength, backWallHeight, 0.1f));

    bottomRectangle = glm::mat4(1.0f);
    bottomRectangle = glm::translate(bottomRectangle, glm::vec3(0.0f, -backWallHeight / 2.0f, centerZ));
    bottomRectangle = glm::rotate(bottomRectangle, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    bottomRectangle = glm::scale(bottomRectangle, glm::vec3(bottomWidth, bottomLength, 0.1f));

    rightTopEdge = glm::mat4(1.0f);
    rightTopEdge = glm::translate(rightTopEdge, glm::vec3(-bottomWidth / 2.0f + poleSize / 2.0f, 0.0f, backZ + poleSize / 2.0f));
    rightTopEdge = glm::scale(rightTopEdge, glm::vec3(poleSize, backWallHeight, poleSize));

    leftTopEdge = glm::mat4(1.0f);
    leftTopEdge = glm::translate(leftTopEdge, glm::vec3(bottomWidth / 2.0f - poleSize / 2.0f, 0.0f, backZ + poleSize / 2.0f));
    leftTopEdge = glm::scale(leftTopEdge, glm::vec3(poleSize, backWallHeight, poleSize));

    leftBottomEdge = glm::mat4(1.0f);
    leftBottomEdge = glm::translate(leftBottomEdge, glm::vec3(bottomWidth / 2.0f - poleSize / 2.0f, 0.0f, frontZ - poleSize / 2.0f));
    leftBottomEdge = glm::scale(leftBottomEdge, glm::vec3(poleSize, backWallHeight, poleSize));

    rightBottomEdge = glm::mat4(1.0f);
    rightBottomEdge = glm::translate(rightBottomEdge, glm::vec3(-bottomWidth / 2.0f + poleSize / 2.0f, 0.0f, frontZ - poleSize / 2.0f));
    rightBottomEdge = glm::scale(rightBottomEdge, glm::vec3(poleSize, backWallHeight, poleSize));

    createSandMash(bottomWidth, bottomLength);
    sand = glm::mat4(1.0f);
    sand = glm::translate(sand,glm::vec3(0.0f,-backWallHeight / 2.0f + 0.05f,centerZ));

}

void::Aquarium::createVAOs() {

    float verticesSignature[] = {
        // Pozicija      Tekstura
      0.30f, 1.0f,   0.0f, 1.0f, // gornje levo
      0.30f, 0.7f,   0.0f, 0.0f, // donje levo
      0.65f, 0.7f,   1.0f, 0.0f, // donje desno
      0.65f, 1.0f,   1.0f, 1.0f, // gornje desno
    };
    float verticesCube[] = {
        // Pozicije (x, y, z)    Normale (nx, ny, nz)
        // Prednja strana
        -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,

        // Zadnja strana
        -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,

        // Leva strana
        -0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,

        // Desna strana
         0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,

         // Gornja strana
         -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,
          0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,
          0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,
          0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,
         -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,
         -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,

         // Donja strana
         -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,
          0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,
          0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,
          0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,
         -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,
         -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f
    }; 
    float verticesCubeTextured[] = {
        // ========================
        // Prednja strana (+Z)
        // ========================
        // pos              normal           uv
        -0.5f,-0.5f, 0.5f,  0,0,1,            0,0,
         0.5f,-0.5f, 0.5f,  0,0,1,            1,0,
         0.5f, 0.5f, 0.5f,  0,0,1,            1,1,

         0.5f, 0.5f, 0.5f,  0,0,1,            1,1,
        -0.5f, 0.5f, 0.5f,  0,0,1,            0,1,
        -0.5f,-0.5f, 0.5f,  0,0,1,            0,0,

        // ========================
        // Zadnja strana (-Z)
        // ========================
        -0.5f,-0.5f,-0.5f,  0,0,-1,           1,0,
         0.5f, 0.5f,-0.5f,  0,0,-1,           0,1,
         0.5f,-0.5f,-0.5f,  0,0,-1,           0,0,

         0.5f, 0.5f,-0.5f,  0,0,-1,           0,1,
        -0.5f,-0.5f,-0.5f,  0,0,-1,           1,0,
        -0.5f, 0.5f,-0.5f,  0,0,-1,           1,1,

        // ========================
        // Leva strana (-X)
        // ========================
        -0.5f, 0.5f, 0.5f, -1,0,0,             1,1,
        -0.5f,-0.5f,-0.5f,-1,0,0,             0,0,
        -0.5f, 0.5f,-0.5f,-1,0,0,             0,1,

        -0.5f,-0.5f,-0.5f,-1,0,0,             0,0,
        -0.5f, 0.5f, 0.5f,-1,0,0,             1,1,
        -0.5f,-0.5f, 0.5f,-1,0,0,             1,0,

        // ========================
        // Desna strana (+X)
        // ========================
         0.5f, 0.5f, 0.5f,  1,0,0,             0,1,
         0.5f, 0.5f,-0.5f,  1,0,0,             1,1,
         0.5f,-0.5f,-0.5f,  1,0,0,             1,0,

         0.5f,-0.5f,-0.5f,  1,0,0,             1,0,
         0.5f,-0.5f, 0.5f,  1,0,0,             0,0,
         0.5f, 0.5f, 0.5f,  1,0,0,             0,1,

         // ========================
         // Gornja strana (+Y)
         // ========================
         -0.5f, 0.5f,-0.5f,  0,1,0,             0,1,
          0.5f, 0.5f, 0.5f,  0,1,0,             1,0,
          0.5f, 0.5f,-0.5f,  0,1,0,             1,1,

          0.5f, 0.5f, 0.5f,  0,1,0,             1,0,
         -0.5f, 0.5f,-0.5f,  0,1,0,             0,1,
         -0.5f, 0.5f, 0.5f,  0,1,0,             0,0,

         // ========================
         // Donja strana (-Y)
         // ========================
         -0.5f,-0.5f,-0.5f,  0,-1,0,            0,1,
          0.5f,-0.5f,-0.5f,  0,-1,0,            1,1,
          0.5f,-0.5f, 0.5f,  0,-1,0,            1,0,

          0.5f,-0.5f, 0.5f,  0,-1,0,            1,0,
         -0.5f,-0.5f, 0.5f,  0,-1,0,            0,0,
         -0.5f,-0.5f,-0.5f,  0,-1,0,            0,1
    };


    VAOsignature = Renderer::createVAO(verticesSignature, sizeof(verticesSignature), 2, 2, 0);
    VAOFarGlass = Renderer::createVAO(verticesCube, sizeof(verticesCube), 3, 3, 0);
    VAOchest = Renderer::createTextureVAO(verticesCubeTextured, sizeof(verticesCubeTextured), 3, 2, 0);
}

void Aquarium::handleMovement()
{
    float leftBound = -width / 2.0f + 0.2f;   
    float rightBound = width / 2.0f - 0.2f;
    float bottomBound = -height / 2.0f;
    float topBound = height / 2.0f - 0.2f;
    float backBound = depth + 0.2f;  
    float frontBound = 0.0f - 0.2f;  


    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { goldenFish->moveRight(); }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { goldenFish->moveLeft(); }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { goldenFish->moveBack(); }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { goldenFish->moveFront(); }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { goldenFish->moveUp(); }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) { goldenFish->moveDown(); }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) { clownFish->moveRight(); }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { clownFish->moveLeft(); }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { clownFish->moveBack(); }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) { clownFish->moveFront(); }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) { clownFish->moveUp(); }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) { clownFish->moveDown(); }

    goldenFish->checkBoundaries(topBound, bottomBound, leftBound, rightBound, frontBound, backBound);
    clownFish->checkBoundaries(topBound, bottomBound, leftBound, rightBound, frontBound, backBound);
}

bool Aquarium::initialize() {
    loadTextures();
    createShaders();
    createVAOs();
    setup();
    return true;
}

void::Aquarium::run() {

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Renderer::drawTexturedRect(textureShader, VAOsignature, signatureTexture);

    Renderer::drawTexturedCube(unifiedShader, VAOchest, chestBottomMatrix, woodTexture, false);
    if (isChestOpen) {
        // adding gold light
        unifiedShader->use();
        unifiedShader->setBool("useSecondLight", true);
        unifiedShader->setVec3("uLightPos2", glm::vec3(baseCoinMatrix[3] + 0.3f));
        unifiedShader->setVec3("uLightColor2", glm::vec3(1.0f, 0.85f, 0.4f)); // zlatna
        // coin
        unifiedShader->setBool("uUseTexture", true);
        unifiedShader->setMat4("uM", baseCoinMatrix);
        coinModel->Draw(*unifiedShader);

        chestLidMatrix = glm::mat4(1.0f);
        chestLidMatrix = glm::translate(chestLidMatrix, glm::vec3(2.5f, -1.35f, -1.5f));
        chestLidMatrix = glm::translate(chestLidMatrix, glm::vec3(0.0f, 0.3f, -0.3f));
        chestLidMatrix = glm::rotate(
            chestLidMatrix,
            glm::radians(-45.0f),
            glm::vec3(1.0f, 0.0f, 0.0f)
                );

        chestLidMatrix = glm::scale(chestLidMatrix, glm::vec3(1.0f, 0.3f, 1.0f));

    }
    else {
        unifiedShader->use();
        unifiedShader->setBool("useSecondLight", false);
        chestLidMatrix = glm::mat4(1.0f);
        chestLidMatrix = glm::translate(chestLidMatrix, glm::vec3(2.5f, -1.35f, -1.5f));
        chestLidMatrix = glm::scale(chestLidMatrix, glm::vec3(1.0f, 0.3f, 1.0f));
    }
    Renderer::drawTexturedCube(unifiedShader, VAOchest, chestLidMatrix, woodTexture, isChestOpen);
   
    Renderer::drawSquare(unifiedShader, VAOFarGlass, bottomRectangle, 0.3f, 0.3f, 0.3f, 1.0f);
    Renderer::drawSquare(unifiedShader, VAOFarGlass, rightTopEdge, 0.3f, 0.3f, 0.3f, 1.0f);
    Renderer::drawSquare(unifiedShader, VAOFarGlass, leftTopEdge, 0.3f, 0.3f, 0.3f, 1.0f);
    Renderer::drawSquare(unifiedShader, VAOFarGlass, leftBottomEdge, 0.3f, 0.3f, 0.3f, 1.0f);
    Renderer::drawSquare(unifiedShader, VAOFarGlass, rightBottomEdge, 0.3f, 0.3f, 0.3f, 1.0f);
    Renderer::drawSquare(unifiedShader, VAOFarGlass, farPlaneRectangle, 0.2f, 0.5f, 0.8f, 0.7f);
    Renderer::drawSquare(unifiedShader, VAOFarGlass, leftWall, 0.2f, 0.5f, 0.8f, 0.7f);
    Renderer::drawSquare(unifiedShader, VAOFarGlass, rightWall, 0.2f, 0.5f, 0.8f, 0.7f);

    
    //coral
    unifiedShader->setBool("uUseTexture", true);  
    unifiedShader->setMat4("uM", pinkCoralMatrix);
    pinkCoral->Draw(*unifiedShader);
    // sea grass
    unifiedShader->setBool("uUseTexture", true);  
    unifiedShader->setMat4("uM", seaGrassMatrix);
    seaGrass->Draw(*unifiedShader);

    unifiedShader->use();
    unifiedShader->setBool("uUseTexture", true);
    unifiedShader->setMat4("uM", sand);
    glBindTexture(GL_TEXTURE_2D, sandTexture);
    Renderer::drawIndexed(VAOsand, sandIndexCount);
    handleMovement();
    goldenFish->draw(unifiedShader);
    clownFish->draw(unifiedShader);
    for (int i = 0; i < 3; i++) {
        if (goldenBubbles[i].isActive()) {
            
            goldenBubbles[i].update(height / 2.0f - 0.2f);
            goldenBubbles[i].draw(unifiedShader, VAObubble, bubbleTexture);
        }
    }
    for (int i = 0; i < 3; i++) {
        if (clownBubbles[i].isActive()) {

            clownBubbles[i].update(height / 2.0f - 0.2f);
            clownBubbles[i].draw(unifiedShader, VAObubble, bubbleTexture);
        }
    }
    Renderer::drawSquare(unifiedShader, VAOFarGlass, frontWall, 0.2f, 0.5f, 0.8f, 0.3f);
}

void Aquarium::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Aquarium* aquarium = static_cast<Aquarium*>( glfwGetWindowUserPointer(window) );
    if (aquarium)
        aquarium->processInput(window, key, scancode, action, mods);
}
void Aquarium::spawnBubbles(Bubble(&bubbles)[3], float fishX, float fishY, float fishZ) {
    for (Bubble& bubble : bubbles) {
        if (!bubble.isActive()) {
            bubble.spawn(fishX, fishY + 0.2f, fishZ + 0.1f);
        }
    }
}