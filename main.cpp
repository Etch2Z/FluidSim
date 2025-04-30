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
    initCircle(3);
    
    int simW = 240, simH = 135;
    float diffusion = 0.01, viscosity = 0.0, dt = 10.0;
    FluidSim *fluidsim = new FluidSim(simW, simH, diffusion, viscosity, dt);
    
    // for (int i = 0; i < fluidsim->size; ++i) {
    //     // fluidsim->dens_prev[i] = 1.0f;  // Simple gradient from 0 to 1
    //     printf("%f ", fluidsim->dens[i]);
    // }

    // printf("\n---------------------------------\n");
    // // fluidsim->update();
    // for (int i = 0; i < fluidsim->w; i++) {
    //     for (int j = 0; j < fluidsim->h; j++) {
    //         // printf("%f ", fluidsim->dens[fluidsim->IX(j,i)]);
    //         printf("%d ", fluidsim->IX(i, j));
    //     }
    //     printf("\n");
    // }

    // return 0;

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
    setupTexture(&texture, fluidsim->w, fluidsim->h);

    // int count = 0;
    // render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float widthScale = float(simW)/screenW, heightScale = float(simH)/screenH;

        if (BUTTONDOWN) {
            if (buttonClicked == LEFT_MOUSE_BUTTON) {
                printf("L\n");
                addCircle(fluidsim, widthScale, heightScale);
            }
            else if (buttonClicked == RIGHT_MOUSE_BUTTON) {
                printf("R\n");
            }
            else if (buttonClicked == G_KEY) {
                printf("G\n");
            }
            else if (buttonClicked == S_KEY) {
                printf("S\n");
            }
        }

        fluidsim->update();
        // if (count == 50) {
        //     printf("-----------------------\n");
        //     for (int i = 0; i < fluidsim->h; i++) {
        //         for (int j = 0; j < fluidsim->w; j++) {
        //             printf("%f ", fluidsim->dens[fluidsim->IX(j,i)]);
        //             // printf("%d ", fluidsim->IX(i, j));
        //         }
        //         printf("\n");
        //     }
        //     count = 0;
        // }
        // count++;

        // Bind texture & load with density
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, fluidsim->w, fluidsim->h, GL_RED, GL_FLOAT, fluidsim->dens);
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
    glDeleteBuffers(1, &EBO); 

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

