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
#include "model.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, float *offset_x, float *offset_y,bool *motion);

// settings
const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 1000;

void draw_scene_model(Model ourModel, bool motion_activated, Shader FboShader, glm::mat4 m_trans, glm::mat4 trans){

    glClearColor(0.2f, 0.5f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
    FboShader.setMat4("model", model);
    ourModel.Draw(FboShader);

    unsigned int transformLoc = glGetUniformLocation(FboShader.ID, "trans");

    if (motion_activated)
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(m_trans));
    else
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

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


    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "MotionBlur", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader ScreenShader("../vertex.shd", "../fragment.shd");

    Shader FboShader("../vertex_fbo.shd", "../fragment_fbo.shd");


    unsigned int VBO, VAO, EBO;

    float quadVertices[] = { 
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


    //loading model

    Model ourModel("../texture/untitled.obj");

    //creation of framebuffers texture for the motion blur

    unsigned int framebuffer1;
    unsigned int textureColorbuffer1;
    set_fbo(&framebuffer1, &textureColorbuffer1);

    unsigned int framebuffer2;
    unsigned int textureColorbuffer2;
    set_fbo(&framebuffer2, &textureColorbuffer2);

    unsigned int framebuffer3;
    unsigned int textureColorbuffer3;
    set_fbo(&framebuffer3, &textureColorbuffer3);


    unsigned int framebuffer4;
    unsigned int textureColorbuffer4;
    set_fbo(&framebuffer4, &textureColorbuffer4);


    unsigned int framebuffer5;
    unsigned int textureColorbuffer5;
    set_fbo(&framebuffer5, &textureColorbuffer5);

    unsigned int framebuffer6;
    unsigned int textureColorbuffer6;
    set_fbo(&framebuffer6, &textureColorbuffer6);

    unsigned int framebuffer7;
    unsigned int textureColorbuffer7;
    set_fbo(&framebuffer7, &textureColorbuffer7);

    unsigned int framebuffer8;
    unsigned int textureColorbuffer8;
    set_fbo(&framebuffer8, &textureColorbuffer8);



    //initialisation of shaders
    FboShader.use();
    FboShader.setInt("Texture", 0);

    ScreenShader.use();
    ScreenShader.setInt("TextureFBO1", 0);
    ScreenShader.setInt("TextureFBO2", 1);
    ScreenShader.setInt("TextureFBO3", 2);
    ScreenShader.setInt("TextureFBO4", 3);
    ScreenShader.setInt("TextureFBO5", 4);
    ScreenShader.setInt("TextureFBO6", 5);
    ScreenShader.setInt("TextureFBO7", 6);
    ScreenShader.setInt("TextureFBO8", 7);


    // a little transformation
    glm::mat4 trans = glm::mat4(1.0f);
    glm::mat4 trans8 = glm::mat4(1.0f);
    glm::mat4 trans7 = glm::mat4(1.0f);
    glm::mat4 trans6 = glm::mat4(1.0f);
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

        //moving the leaf

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

        //get position of object for each frame

        // render first step
        if (frame < 8) {
            if (frame % 8 == 7) {
                trans8 = trans7;
            }
            if (frame % 8 == 6) {
                trans7 = trans6;
            }
            if (frame % 8 == 5) {
                trans6 = trans5;
            }

            if (frame % 8 == 4) {
                trans5 = trans4;
            }
            if (frame % 8 == 3) {
                trans4 = trans3;
            }
            if (frame % 8 == 2) {
                trans3 = trans2;
            }
            if (frame % 8 == 1) {
                trans2 = trans1;
            }
            if (frame % 8 == 0) {
                trans1 = trans;
            }
        } else {

            trans8 = trans7;
            trans7 = trans6;
            trans6 = trans5;
            trans5 = trans4;
            trans4 = trans3;
            trans3 = trans2;
            trans2 = trans1;
            trans1 = trans;
        }

        frame+=1;

        FboShader.use();

        // render each frame
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer8);
        draw_scene_model(ourModel, motion_activated, FboShader, trans8, trans);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer7);
        draw_scene_model(ourModel, motion_activated, FboShader, trans7, trans);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer6);
        draw_scene_model(ourModel, motion_activated, FboShader, trans6, trans);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer5);
        draw_scene_model(ourModel, motion_activated, FboShader, trans5, trans);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer4);
        draw_scene_model(ourModel, motion_activated, FboShader, trans4, trans);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer3);
        draw_scene_model(ourModel, motion_activated, FboShader, trans3, trans);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer2);
        draw_scene_model(ourModel, motion_activated, FboShader, trans2, trans);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer1);
        draw_scene_model(ourModel, motion_activated, FboShader, trans1, trans);

        // Back main framebuffer

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // clear all relevant buffers
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer6);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer7);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer8);



        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);

        glfwPollEvents();

    }

    glfwTerminate();
    return 0;
}

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}