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
    circle1.init(4);
    circle2.init(5);
    
    int simW = 240, simH = 135;
    float diffusion = 0.5, viscosity = 0.05, dt = 1.0;
    FluidSim *fluidsim = new FluidSim(simW, simH, diffusion, viscosity, dt);

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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float widthScale = float(simW)/screenW, heightScale = float(simH)/screenH;

        if (BUTTONDOWN) {
            if (buttonClicked == LEFT_MOUSE_BUTTON) {
                addCircle(fluidsim, widthScale, heightScale, circle1);
            }
            else if (buttonClicked == RIGHT_MOUSE_BUTTON) {
                // When clicked, add a circle of velocity fields that attract/pushes away
                addForce(fluidsim, widthScale, heightScale, circle2, PUSH, 1, 1);
            }
            else if (buttonClicked == S_KEY) {
                addForce(fluidsim, widthScale, heightScale, circle2, PULL, 1, 1);
            }
            else if (buttonClicked == G_KEY) {
                // lobsided
                addForce(fluidsim, widthScale, heightScale, circle2, PUSH, 0, 1);
            }
            else if (buttonClicked == H_KEY) {
                addForce(fluidsim, widthScale, heightScale, circle2, PUSH, -1, 1);
            }
            else if (buttonClicked == J_KEY) {
                addForce(fluidsim, widthScale, heightScale, circle2, PUSH, 1, 0);
            }
            else if (buttonClicked == K_KEY) {
                addForce(fluidsim, widthScale, heightScale, circle2, PUSH, 1, -1);
            }
            
            else if (buttonClicked == R_KEY) {
                // reset
                for (int i = 0; i < fluidsim->size; i++) {
                    fluidsim->dens[i] = 0.0f;
                    fluidsim->u[i] = 0.0f;
                    fluidsim->v[i] = 0.0f;
                }
            }
            else if(false){
                // gravity?
                // for (int i = 0; i < fluidsim->size; i++) {
                //     fluidsim->v[i] += 0.8f;
                // }
            }
        }

        fluidsim->update();
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

