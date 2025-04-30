#ifndef MAIN_H
#define MAIN_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "FluidSim.h"

#include <iostream>
#include <stdio.h>
#include <cmath>

union Point {
  struct {
    float xF, yF;
  };
  struct {
    int xI, yI;
  };
};

#define PI 3.14159265

/*
//////////////////////////////// Global Values /////////////////////////////////////////////////
*/

// Updated by callback function when changes
int screenW = 800;
int screenH = 600;
// Updated by callback function when changes
Point mouseLoc = {0.0f, 0.0f};
bool MOUSEDOWN = false;
// Class for caching directions to go around the circle center to form a circle
class Circle {
public:
    Point *v;
    int capacity;
    int size;

    Circle() { v = nullptr; size = 0; }
    ~Circle() { delete[] v; }
    void init(int capacity) {
        capacity = capacity;
        v = new Point[capacity];
    }
    void append(Point point) { v[size++] = point; }
};
Circle circle;

/*
//////////////////////////////// Function Declarations /////////////////////////////////////////////////
*/
void addCircle(FluidSim *fluidsim, float widthScale, float heightScale);
void initCircle(int radius);
GLFWwindow* init_window();
void setupQuad(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO);
void setupTexture(unsigned int& texture, int w, int h);

void trackMouse(GLFWwindow *window);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_pos_callback(GLFWwindow* window, double x, double y);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);  

/*
//////////////////////////////// Function Definitions /////////////////////////////////////////////////
*/

/*
convert screen dimension to sim dimensions
screen coord: screenH, screenW
density map: simW, simH
Ex. screenW = 500   simW = 250      widthScale = 1/2
mouseLoc: 400   => 400* 1/2 => simLoc = 200
*/
void addCircle(FluidSim *fluidsim, float widthScale, float heightScale) {
    Point center = {mouseLoc.xF * widthScale, mouseLoc.yF * heightScale};
    center.xI = int(center.xF);
    center.yI = int(center.yF);

    for (int i = 0; i < circle.size; i++) {
        int newX = center.xI + circle.v[i].xI;
        int newY = center.yI + circle.v[i].yI;
        
        // Add to area inside the grid boundry only.
        int w = fluidsim->w-1, h = fluidsim->h-1;
        if (newX >= 1 && newX < w && newY >= 1 && newY < h) {
            fluidsim->addDensity(newX, newY);
        }
        
    }
}

/* Generate the point as vectors to encompass a circle around its center
0 0 0 0 0                           0 0 1 0 0
0 0 0 0 0                           0 1 1 1 0
0 0 x 0 0   X marks the center ->   1 1 x 1 1
0 0 0 0 0                           0 1 1 1 0
0 0 0 0 0                           0 0 1 0 0
*/
void initCircle(int radius) {
    int squareSize = (2*radius+1) * (2*radius+1);
    circle.init(squareSize);

    for (int i = -radius; i <= radius; i++) {
        for (int j = -radius; j <= radius; j++) {
            if (sqrt(i*i + j*j) <= radius+0.5) {
                circle.append(Point{.xI=i, .yI=j});
            }
        }
    }
}

GLFWwindow* init_window() {
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
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    
    // Load OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    // Set window to maximized.
    glfwMaximizeWindow(window);
    glfwGetWindowSize(window, &screenW, &screenH);
    glViewport(0, 0, screenW, screenH);

    return window;
}

void setupQuad(unsigned int* VBO, unsigned int* VAO, unsigned int* EBO) {
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

    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);
    glGenBuffers(1, EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(*VAO);

    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

}

void setupTexture(unsigned int* texture, int w, int h) {
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);

    // texture settings
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Allocate texture (Later updates don't reallocate again)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_FLOAT, nullptr);
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
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    glfwGetWindowSize(window, &screenW, &screenH);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (MOUSEDOWN) { trackMouse(window); }
}

#endif // Main