#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shaders.h"
#include "FluidSim.h"

#include <iostream>
#include <stdio.h>
// #include <cmath>
// #include <vector>

#define PI 3.14159265

int screenW = 800;
int screenH = 600;

// struct Point {
//     float x, y;
// };

Point mouseLoc = {0.0f, 0.0f};
bool MOUSEDOWN = false;

void trackMouse(GLFWwindow *window);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_pos_callback(GLFWwindow* window, double x, double y);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// int argc, char* argv[]
int main() {

    int simW = 1120, simH = 630;

    float diffusion = 1.0, viscosity = 1.0, dt = 0.005;
    FluidSim FluidSim(simW, simH, diffusion, viscosity, dt);

    int size = FluidSim.size;
    for (int i = 0; i < size; ++i) {
        FluidSim.dens[i] = 0.5f;  // Simple gradient from 0 to 1
    }


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
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    
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

    // Rectangle Screen that we bind the texture to
    float quad[] = {
        // positions   // texCoords
        -1.0f, -1.0f,  0.0f, 1.0f,  // bottom-left      array top-right
         1.0f, -1.0f,  1.0f, 1.0f,  // bottom-right     array bottem-right
         1.0f,  1.0f,  1.0f, 0.0f,  // top-right        array bottem-left
        -1.0f,  1.0f,  0.0f, 0.0f   // top-left         array top-left
    };
    
    unsigned int quadIndices[] = {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // texture settings
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Allocate texture (Later updates don't reallocate again)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, FluidSim.w, FluidSim.h, 0, GL_RED, GL_FLOAT, nullptr);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, simWidth, simHeight, 0, GL_RED, GL_FLOAT, nullptr);


    // render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (MOUSEDOWN) {
            // Create circle and add to density;
            int radius = 10;
            int n_points = radius * radius;
            float widthScale = float(simW)/screenW, heightScale = float(simH)/screenH;
            // std::cout << simW << ' ' << screenW << std::endl;
            /*
            convert screen dimension to sim dimensions
            screen coord: screenH, screenW
            density map: simW, simH
            Ex. screenW = 500   simW = 250      widthScale = 1/2
            mouseLoc: 400   => 400* 1/2 => simLoc = 200
            */
            Point center = {mouseLoc.xF * widthScale, mouseLoc.yF * heightScale};
            center.xI = int(center.xF);
            center.yI = int(center.yF);
            // Point circle[n_points];

            // FluidSim.dens[center.xI] = 1.0f;
            // printf("%d \n", center.xI);
            for (int i = 0; i < 10; i++) {
                for (int j = 0; j < 10; j++) {
                    // FluidSim.dens[FluidSim.IX(center.xI+i, center.yI+j)] = 1.0f;
                    FluidSim.addDensity(0, center.xI+i, center.yI+j);

                }
            }
            // FluidSim.addDensity(center);
        }

        // Bind texture & load with density
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, FluidSim.w, FluidSim.h, GL_RED, GL_FLOAT, FluidSim.dens);
        // Draw the rectangle with density as texture
        myShader.use();
        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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

// Save mouse coords to mouseLoc.
void trackMouse(GLFWwindow *window) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    mouseLoc.xF = x;
    mouseLoc.yF = y;
    // printf("%f %f\n", x, y);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            MOUSEDOWN = true;
        }
        else if (action == GLFW_RELEASE) {
            MOUSEDOWN = false;
        }
    }
}

void cursor_pos_callback(GLFWwindow* window, double x, double y) {
    // glfwGetWindowSize(window, &screenW, &screenH);
    // float normX = 2.0f * x / screenW - 1.0f;
    // float normY = 1 - 2.0f * y / screenH;
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    glfwGetWindowSize(window, &screenW, &screenH);
}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (MOUSEDOWN) { trackMouse(window); }
}
