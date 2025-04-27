#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "main.h"
#include "shaders.h"
#include "FluidSim.h"

#include <iostream>
#include <stdio.h>
// #include <cmath>
// #include <vector>




// int argc, char* argv[]
int main() {
    int simW = 1120, simH = 630;
    float diffusion = 1.0, viscosity = 1.0, dt = 0.03;
    FluidSim FluidSim(simW, simH, diffusion, viscosity, dt);
    int size = FluidSim.size;
    for (int i = 0; i < size; ++i) {
        FluidSim.dens[i] = 0.0f;  // Simple gradient from 0 to 1
    }
    
    // Init window
    GLFWwindow* window = init_window();
    if (!window) { return -1; }
    
    // Build and compile shaders
    Shader myShader("vertexShader330.vs", "fragmentShader330.fs");

    // Quad
    unsigned int VBO, VAO, EBO;
    setupQuad(&VBO, &VAO, &EBO);
    
    // Texture
    unsigned int texture;
    setupTexture(&texture, FluidSim.w, FluidSim.h);


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

