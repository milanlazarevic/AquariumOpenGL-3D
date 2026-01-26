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
    goldenFishModelMatrix(0), clownFishModelMatrix(0),
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
    goldenFish = new Model("res/low-poly-fox.obj");
    processTexture(signatureTexture, "res/MilanLazarevicSV2-2022.png");
    processTexture(sandTexture, "res/sandTexture.jpg");
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
    const int N = 100;          // gustina mreže
    const float sizeX = bottomWidth;
    const float sizeZ = bottomLength;

    std::vector<float> vertices;
    std::vector<int> indices;

    for (int z = 0; z < N; z++) {
        for (int x = 0; x < N; x++) {
            float xpos = ( (float)x / ( N - 1 ) - 0.5f ) * sizeX;
            float zpos = ( (float)z / ( N - 1 ) - 0.5f ) * sizeZ;

            float y = ( ( rand() % 100 ) / 1000.0f ); // mala neravnina

            float u = (float)x / ( N - 1 );
            float v = (float)z / ( N - 1 );

            vertices.insert(vertices.end(), {
                xpos, y, zpos,
                0.0f, 1.0f, 0.0f, // normal (ok za predmet)
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
    goldenFishModelMatrix = glm::mat4(1.0f);
    goldenFishModelMatrix = glm::translate(goldenFishModelMatrix, glm::vec3(0.0f, 0.0f, -2.0f));

    // far plane glass
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
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
    sand = glm::translate(
        sand,
        glm::vec3(
            0.0f,
            -backWallHeight / 2.0f + 0.05f,
            centerZ
        )
    );

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


    VAOsignature = Renderer::createVAO(verticesSignature, sizeof(verticesSignature), 2, 2, 0);
    VAOFarGlass = Renderer::createVAO(verticesCube, sizeof(verticesCube), 3, 3, 0);
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
    Renderer::drawTexturedRect(textureShader, VAOsignature, signatureTexture);

    // TODO ovo dole sve stavi u ofu funkciju
    Renderer::drawSquare(unifiedShader, VAOFarGlass, bottomRectangle, 0.3f, 0.3f, 0.3f, 1.0f);
    Renderer::drawSquare(unifiedShader, VAOFarGlass, rightTopEdge, 0.3f, 0.3f, 0.3f, 1.0f);
    Renderer::drawSquare(unifiedShader, VAOFarGlass, leftTopEdge, 0.3f, 0.3f, 0.3f, 1.0f);
    Renderer::drawSquare(unifiedShader, VAOFarGlass, leftBottomEdge, 0.3f, 0.3f, 0.3f, 1.0f);
    Renderer::drawSquare(unifiedShader, VAOFarGlass, rightBottomEdge, 0.3f, 0.3f, 0.3f, 1.0f);

    Renderer::drawSquare(unifiedShader, VAOFarGlass, farPlaneRectangle, 0.2f, 0.5f, 0.8f, 0.7f);
    Renderer::drawSquare(unifiedShader, VAOFarGlass, leftWall, 0.2f, 0.5f, 0.8f, 0.7f);
    Renderer::drawSquare(unifiedShader, VAOFarGlass, rightWall, 0.2f, 0.5f, 0.8f, 0.7f);
    

     //Za ribu koristi teksturu
    goldenFishModelMatrix = glm::rotate(goldenFishModelMatrix, glm::radians(0.4f), glm::vec3(0.0f, 1.0f, 0.0f));
    unifiedShader->setBool("uUseTexture", true);  // Vrati na teksturu
    unifiedShader->setMat4("uM", goldenFishModelMatrix);
    goldenFish->Draw(*unifiedShader);
    unifiedShader->use();
    unifiedShader->setBool("uUseTexture", true);
    unifiedShader->setMat4("uM", sand);

    glBindTexture(GL_TEXTURE_2D, sandTexture);
    Renderer::drawIndexed(VAOsand, sandIndexCount);
    Renderer::drawSquare(unifiedShader, VAOFarGlass, frontWall, 0.2f, 0.5f, 0.8f, 0.3f);
}