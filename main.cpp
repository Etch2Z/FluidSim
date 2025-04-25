#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shaders.h"
#include "FluidSim.h"

#include <iostream>
#include <stdio.h>
// #include <cmath>
// #include <vector>

int screenW = 800;
int screenH = 600;

struct Point {
    float x, y;
    float r, g, b, a;
};

const int n = 200;

Point points[n];
int point_n = 0;

void cursor_pos_callback(GLFWwindow* window, double x, double y);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// int argc, char* argv[]
int main() {
    glfwInit();                                                         // Init
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                      // Configure versions & core profiles
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

// #ifdef __APPLE__
//     glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
// #endif

    // Create Window
    GLFWwindow* window = glfwCreateWindow(screenW, screenH, "FluidSim", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    
    // Load OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // Set window to maximized.
    glfwMaximizeWindow(window);
    glfwGetWindowSize(window, &screenW, &screenH);
    glViewport(0, 0, screenW, screenH);

    // Build and compile shaders
    Shader myShader("vertexShader330.vs", "fragmentShader330.fs");

    for (int i = 0; i < n; i++) {
        points[i] = {1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f};
    }

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // glPointSize(10.0f);
    // render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // render the triangle
        myShader.use();

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, n);
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

void cursor_pos_callback(GLFWwindow* window, double x, double y) {
    glfwGetWindowSize(window, &screenW, &screenH);
    float normX = 2.0f * x / screenW - 1.0f;
    float normY = 1 - 2.0f * y / screenH;

    points[point_n].x = normX;
    points[point_n].y = normY;

    point_n++;
    point_n = point_n%n;

}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
