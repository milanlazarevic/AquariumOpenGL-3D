#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>



#include "../Header/shader.hpp"
#include "../Header/mesh.hpp"

#include "../Header/aquarium.hpp"
#include "../Header/stb_image.h"
#include <thread>

const int FPS = 75;

void limitFPS(double& lastTimeForRefresh)
{
    double now = glfwGetTime();
    double targetFrameTime = 1.0 / FPS;
    double remaining = ( lastTimeForRefresh + targetFrameTime ) - now;

    if (remaining > 0.0)
    {
        std::this_thread::sleep_for(std::chrono::duration<double>(remaining));
    }

    lastTimeForRefresh = glfwGetTime();
}

int main()
{
    if (!glfwInit())
    {
        std::cout << "GLFW fail!\n" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Aquarium3D", primary, NULL);
    if (window == NULL)
    {
        std::cout << "Window fail!\n" << std::endl;
        glfwTerminate();
        return -2;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW fail! :(\n" << std::endl;
        return -3;
    }

    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    float wallWidth = 10.0f / ( mode->width / 2.0f );

    Aquarium aquarium(window, wallWidth, mode->width, mode->height);

    if (!aquarium.initialize()) {
        std::cerr << "Failed to initialize aquarium" << std::endl;
        return -1;
    }

    
    // Enable depth test and blending ONCE
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    double lastTimeForRefresh = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
  
        aquarium.run();

        glfwSwapBuffers(window);
        limitFPS(lastTimeForRefresh);
    }

    glfwTerminate();
    return 0;
}