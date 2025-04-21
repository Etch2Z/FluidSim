#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shaders.h"
#include <iostream>
#include <vector>

struct MouseTrailPoint {
    float x, y;  // position
    float r, g, b, a;  // color and alpha
};

std::vector<MouseTrailPoint> trailPoints;

GLuint VAO, VBO;
GLuint shaderProgram;

void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void renderTrail();
void setupBuffers();
void updateTrailPoints(GLFWwindow* window, double xpos, double ypos);

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Mouse Trail", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        return -1;
    }

    // Set the mouse callback
    glfwSetCursorPosCallback(window, mouse_callback);

    // Set up OpenGL buffers and shaders
    setupBuffers();
    Shader myShader("vertexShader330.vs", "fragmentShader330.fs");
    shaderProgram = myShader.ID;

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Update and render the trail
        renderTrail();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    updateTrailPoints(window, xpos, ypos);
}

void updateTrailPoints(GLFWwindow* window, double xpos, double ypos) {
    // Store mouse position in the trail array
    MouseTrailPoint point = { (float)xpos, (float)ypos, 1.0f, 0.0f, 0.0f, 1.0f };  // Red color and full alpha
    trailPoints.push_back(point);

    // Optionally limit the trail size
    if (trailPoints.size() > 100) {
        trailPoints.erase(trailPoints.begin());
    }

    // Update the VBO with the new trail points
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, trailPoints.size() * sizeof(MouseTrailPoint), trailPoints.data(), GL_DYNAMIC_DRAW);
}

void setupBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, trailPoints.size() * sizeof(MouseTrailPoint), trailPoints.data(), GL_DYNAMIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(MouseTrailPoint), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(MouseTrailPoint), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void renderTrail() {
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, trailPoints.size());  // Render each mouse trail point as a point
    glBindVertexArray(0);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
