#pragma once
#include "shader.hpp"
#include <vector>


class Renderer {
public:
    static void drawRect(unsigned int shader, unsigned int VAO, float opacity = 1.0f);
    static void drawSquare(Shader* shader, unsigned int VAO, glm::mat4 squareMatrix, float red, float green, float blue, float opacity = 1.0f);
    static void drawTexturedRect(Shader* shader, unsigned int VAO, unsigned int texture, bool open = false);
    static void drawTexturedCube(Shader* shader, unsigned int VAO, glm::mat4 squareMatrix, unsigned int texture, bool open = false);
    static unsigned int createVAO(float* vertices, size_t size, int posSize, int texSize, int colSize);
    static unsigned int createTextureVAO(float* vertices, size_t size, int posSize, int texSize, int colSize);
    static unsigned createIndexedVAO(const std::vector<float>& vertices,const std::vector<int>& indices);
    static void drawIndexed(unsigned VAO, size_t indexCount);
   

};