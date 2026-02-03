#include <GL/glew.h>       
#include <GLFW/glfw3.h>

#include "../Header/renderer.hpp"



void Renderer::drawRect(unsigned int shader, unsigned int VAO, float opacity) {
    glUseProgram(shader);
    glUniform1f(glGetUniformLocation(shader, "opacity"), opacity);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Renderer::drawSquare(Shader* shader, unsigned int VAO, glm::mat4 squareMatrix, float red, float green, float blue, float opacity)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader->use();

    // Postavi uniforms za BOJU (bez teksture)
    shader->setBool("uUseTexture", false);
    shader->setVec3("uObjectColor", red, green, blue); // plava
    shader->setFloat("uAlpha", opacity); // 30% providno
    shader->setMat4("uM", squareMatrix);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Renderer::drawTexturedRect(Shader* shader, unsigned int VAO, unsigned int texture, bool open) {
    shader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    /*glUniform1i(glGetUniformLocation(shader, "texture0"), 0);
    glUniform1f(glGetUniformLocation(shader, "uS"), 1.0f);
    glUniform1f(glGetUniformLocation(shader, "uX"), 0.0f);
    glUniform1f(glGetUniformLocation(shader, "uY"), 0.0f);
    glUniform1f(glGetUniformLocation(shader, "flipped"), false);
    glUniform1f(glGetUniformLocation(shader, "open"), open);*/
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Renderer::drawTexturedCube(Shader* shader, unsigned int VAO, glm::mat4 squareMatrix, unsigned int texture, bool open) {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader->use();

    shader->setMat4("uM", squareMatrix);
    shader->setBool("uUseTexture", true);
    //shader->setVec4("uObjectColor", color);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    shader->setInt("uDiffMap1", 0);
    //shader->setInt("uTexture", texture);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

}

unsigned int Renderer::createVAO(float* vertices, size_t size, int posSize, int texSize, int colSize) {
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    if (colSize == 0) {
        int stride = ( posSize + texSize ) * sizeof(float);
        glVertexAttribPointer(0, posSize, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(0);

        if (texSize > 0) {
            glVertexAttribPointer(1, texSize, GL_FLOAT, GL_FALSE, stride, (void*)( posSize * sizeof(float) ));
            glEnableVertexAttribArray(1);
        }
    }
    else {
        int stride = ( posSize + colSize ) * sizeof(float);
        glVertexAttribPointer(0, posSize, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, colSize, GL_FLOAT, GL_FALSE, stride, (void*)( posSize * sizeof(float) ));
        glEnableVertexAttribArray(1);
    }

    return VAO;
}

unsigned int Renderer::createTextureVAO(float* vertices, size_t size, int posSize, int texSize, int colSize) {
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    if (colSize == 0) {
        // [pos | normal | uv]
        // =========================

        int stride = ( posSize + 3 + texSize ) * sizeof(float);
        int offset = 0;

        // position → location 0
        glVertexAttribPointer(0, posSize, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(0);
        offset += posSize;

        // normal → location 1
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
            (void*)( offset * sizeof(float) ));
        glEnableVertexAttribArray(1);
        offset += 3;

        // uv → location 2
        if (texSize > 0) {
            glVertexAttribPointer(2, texSize, GL_FLOAT, GL_FALSE, stride,
                (void*)( offset * sizeof(float) ));
            glEnableVertexAttribArray(2);
        }
    }
    else {
        int stride = ( posSize + colSize ) * sizeof(float);
        glVertexAttribPointer(0, posSize, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, colSize, GL_FLOAT, GL_FALSE, stride, (void*)( posSize * sizeof(float) ));
        glEnableVertexAttribArray(1);
    }

    return VAO;
}

unsigned Renderer::createIndexedVAO(const std::vector<float>& vertices, const std::vector<int>& indices) {
    unsigned VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // VERTICES
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW
    );

    // INDICES
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned),
        indices.data(),
        GL_STATIC_DRAW
    );

    // layout: vec3 pos, vec3 normal, vec2 uv
    int stride = ( 3 + 3 + 2 ) * sizeof(float);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)( 3 * sizeof(float) ));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)( 6 * sizeof(float) ));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    return VAO;
}

void Renderer::drawIndexed(unsigned VAO, size_t indexCount) {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
