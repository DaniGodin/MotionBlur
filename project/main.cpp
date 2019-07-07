#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <zconf.h>
#include <cmath>
#include "Shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, float *offset_x, float *offset_y,bool *motion);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

void init_vertices(unsigned int* VBO,unsigned int* VAO,unsigned int* EBO){

    float vertices[] = {
            -0.25f,  0.25f, 0.0f, 0.0f, 1.0f, // top left
            -0.25f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom left
            0.25f, 0.0f, 0.0f,  1.0f, 0.0f,// bottom right
            0.25f, 0.25f, 0.0f, 1.0f, 1.0f // top right

    };

    unsigned int indices[] = {  // note that we start from 0!
            0, 1, 3,  // first Triangle
            1, 2, 3   // second Triangle
    };

    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);
    glGenBuffers(1, EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(*VAO);

    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


}


void draw_scene(unsigned int texture, bool motion_activated, unsigned int VAO, Shader FboShader, glm::mat4 m_trans, glm::mat4 trans){

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    unsigned int transformLoc = glGetUniformLocation(FboShader.ID, "trans");

    if (motion_activated)
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(m_trans));
    else
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

}

void set_fbo(unsigned int *framebuffer, unsigned int *textureColorbuffer){

    glGenFramebuffers(1, framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, *framebuffer);
    // create a color attachment texture
    glGenTextures(1, textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, *textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *textureColorbuffer, 0);



    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}




int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "MotionBlur", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------
    Shader ScreenShader("../vertex.shd", "../fragment.shd");

    Shader FboShader("../vertex_fbo.shd", "../fragment_fbo.shd");


    unsigned int VBO, VAO, EBO;

    init_vertices(&VBO, &VAO, &EBO);


    //init screen
    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
            1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
            1.0f, -1.0f,  1.0f, 0.0f,
            1.0f,  1.0f,  1.0f, 1.0f
    };
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


    //loading texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *data = stbi_load("../texture/container.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);


    //creation of first framebuffer texture for the motion blur

    unsigned int framebuffer1;
    unsigned int textureColorbuffer1;
    set_fbo(&framebuffer1, &textureColorbuffer1);

    //creation of second framebuffer texture for the motion blur

    unsigned int framebuffer2;
    unsigned int textureColorbuffer2;
    set_fbo(&framebuffer2, &textureColorbuffer2);

    //creation of second framebuffer texture for the motion blur

    unsigned int framebuffer3;
    unsigned int textureColorbuffer3;
    set_fbo(&framebuffer3, &textureColorbuffer3);


    //creation of second framebuffer texture for the motion blur

    unsigned int framebuffer4;
    unsigned int textureColorbuffer4;
    set_fbo(&framebuffer4, &textureColorbuffer4);


    //creation of second framebuffer texture for the motion blur

    unsigned int framebuffer5;
    unsigned int textureColorbuffer5;
    set_fbo(&framebuffer5, &textureColorbuffer5);



    //initialisation of shaders
    FboShader.use();
    FboShader.setInt("Texture", 0);

    ScreenShader.use();
    ScreenShader.setInt("TextureFBO1", 0);
    ScreenShader.setInt("TextureFBO2", 1);
    ScreenShader.setInt("TextureFBO3", 2);
    ScreenShader.setInt("TextureFBO4", 3);
    ScreenShader.setInt("TextureFBO5", 4);

    // a little transformation
    glm::mat4 trans = glm::mat4(1.0f);
    glm::mat4 trans5 = glm::mat4(1.0f);
    glm::mat4 trans4 = glm::mat4(1.0f);
    glm::mat4 trans3 = glm::mat4(1.0f);
    glm::mat4 trans2 = glm::mat4(1.0f);
    glm::mat4 trans1 = glm::mat4(1.0f);



    trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0, 0.0, 1.0));
    // render loop
    // -----------
    float pos_x =  1.0f;
    float pos_y = 0.5f;
    bool sense_x = 1;
    bool sense_y= 1;
    float offset_x = 0.0004f;
    float offset_y = 0.0004f;
    int frame = 0;
    bool motion_activated = false;

    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window, &offset_x, &offset_y,&motion_activated);


        // ------


        if (pos_x < -1.0f || pos_x > 1.0f){
            sense_x = (sense_x + 1)%2;
        }

        if (pos_y < -1.0f || pos_y > 1.0f){
            sense_y = (sense_y + 1)%2;
        }



        glm::mat4 trans = glm::mat4(1.0f);

        if(sense_x == 1) {
            pos_x = pos_x - offset_x;
        } else {
            pos_x = pos_x + offset_x;
        }

        if(sense_y == 1) {
            pos_y = pos_y - offset_y;
        } else {
            pos_y = pos_y + offset_y;
        }



        trans = glm::translate(trans, glm::vec3(pos_x, pos_y, 0.0f));
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

        // render first step
        if (frame < 5) {
            if (frame % 5 == 4) {
                trans5 = trans4;
            }
            if (frame % 5 == 3) {
                trans4 = trans3;
            }
            if (frame % 5 == 2) {
                trans3 = trans2;
            }
            if (frame % 5 == 1) {
                trans2 = trans1;
            }
            if (frame % 5 == 0) {
                trans1 = trans;
            }
        } else {
            trans5 = trans4;
            trans4 = trans3;
            trans3 = trans2;
            trans2 = trans1;
            trans1 = trans;
        }

        frame+=1;

        FboShader.use();


        // render each frame
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer5);
        draw_scene(texture, motion_activated, VAO, FboShader, trans5, trans);


        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer4);
        draw_scene(texture, motion_activated, VAO, FboShader, trans4, trans);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer3);
        draw_scene(texture, motion_activated, VAO, FboShader, trans3, trans);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer2);
        draw_scene(texture, motion_activated, VAO, FboShader, trans2, trans);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer1);
        draw_scene(texture, motion_activated, VAO, FboShader, trans1, trans);


        // Back main framebuffer

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // clear all relevant buffers
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
        glClear(GL_COLOR_BUFFER_BIT);

        ScreenShader.use();

        glBindVertexArray(quadVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer5);



        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwPollEvents();

        sleep(0.5);

    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, float *offset_x, float *offset_y,bool *motion)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        *offset_x += 0.0001;
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        *offset_x -= 0.0001;
    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        *offset_y += 0.0001;
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        *offset_y -= 0.0001;
    if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
        *motion = true;
    if(glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS)
        *motion = false;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}