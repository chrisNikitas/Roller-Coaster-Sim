//This Class was made with help from:
//https://learnopengl.com/Advanced-Lighting/Normal-Mapping
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Model.h"
#include "RayCast.h"
#include "Camera.h"
#include "Track.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/ext.hpp"
#include "App.h"
#include "stb_image/stb_image.h"

//GLOBALS
float SCR_WIDTH = 640;
float SCR_HEIGHT = 480;
Camera freeCamera;
Camera POVcamera;
Camera* activeCamera = &freeCamera;
bool freePOV = false;
Track track;
Object light(false, 0.2f);
Object groundPlane(false);

//Scene scene;


int main(void){

    GLFWwindow* window;
    /* Initialize the library */
    /* Create a windowed mode window and its OpenGL context */
    bool retflag;
    int retval = init(window, retflag);
    if (retflag) return retval;

    track.init();

    //Declare Shaders
    Shader shaderProgram("src\\shaders\\basicVertexShaderFile.shader", "src\\shaders\\basicFragmentShaderFile.shader");
    Shader splinesShaderProgram("src\\shaders\\splinesVertexShaderFile.shader", "src\\shaders\\uniformColorFragment.shader");
    Shader primitivesShaderProgram("src\\shaders\\primitivesVertexShaderFile.shader", "src\\shaders\\primitivesFragmentShaderFile.shader");
    Shader trackShaderProgram("src\\shaders\\trackVertexShaderFile.shader", "src\\shaders\\trackFragmentShaderFile.shader");
    Shader gizmosShaderProgram("src\\shaders\\gizmosVertexShaderFile.shader", "src\\shaders\\uniformColorFragment.shader");
    Shader textureShaderProgram("src\\shaders\\textureVertexShaderFile.shader", "src\\shaders\\textureFragmentShaderFile.shader");
    Shader skyboxShaderProgram("src\\shaders\\skyboxVertexShaderFile.shader", "src\\shaders\\skyboxFragmentShaderFile.shader");

    //Skybox Creation
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    VertexArray skyboxVAO;
    skyboxVAO.init();
    VertexBuffer skyboxVBO(skyboxVertices, sizeof(skyboxVertices));
    BufferLayout skyboxBL;
    skyboxBL.addAttribute<float>(3);
    skyboxVAO.addBufferWithLayout(skyboxVBO, skyboxBL);
    unsigned int sky;
    glGenTextures(1, &sky);
    glBindTexture(GL_TEXTURE_CUBE_MAP, sky);
    int width, height, nrComponents;
    unsigned char* data;
    std::string path;

    loadSkyboxTextures(path, data, width, height, nrComponents);

    //Shadows
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
    unsigned int depthMap;
    createDepthMapTexture(depthMap, SHADOW_WIDTH, SHADOW_HEIGHT, depthMapFBO);

    //Light
    float light_vertices[] = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    };
    glm::vec3 lightPos = glm::vec3(0, 5, 1);
    int lightAttributes[] = { 3 };
    light.addData(lightPos, light_vertices, (sizeof light_vertices / sizeof light_vertices[0]), 1, lightAttributes);
    light.setShape(GL_TRIANGLES);
    light.setScale(.2);


 
    //Camera
        //Normal
    glm::mat4 projection;//= glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glm::vec3 eye(0, 1, 5);
    glm::vec3 direction(0, 0, -1);
    direction = glm::normalize(direction);
    glm::vec3 up(0, 1, 0);
    freeCamera.init(projection, eye, direction, up);
    glm::mat4 model = glm::mat4(1.0f); 
    glm::vec3 last_direction = direction;
        //POV
    //projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(85.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    eye = glm::vec3(0, 0, 0);
    direction = glm::vec3(0, 0, -1);
    direction = glm::normalize(direction);
    POVcamera.init(projection, eye, direction, up);
    model = glm::mat4(1.0f);
    last_direction = direction;


    // Ground
    // positions
    glm::vec3 pos1(-1.0f, 0.0f, -1.0f);
    glm::vec3 pos2(-1.0f, 0.0f, 1.0f );
    glm::vec3 pos3( 1.0f, 0.0f, 1.0f );
    glm::vec3 pos4( 1.0f, 0.0f, -1.0f);
    // texture coordinates
    glm::vec2 uv1(0.0f, 0.0f);
    glm::vec2 uv2(0.0f, 8.0f);
    glm::vec2 uv3(8.0f, 8.0f);
    glm::vec2 uv4(8.0f, 0.0f);
    // normal vector
    glm::vec3 nm(0.0f, 1.0f, 0.0f);

    // tangent/bitangent
    glm::vec3 tangent1, bitangent1;
    glm::vec3 tangent2, bitangent2;
    // triangle 1
    glm::vec3 edge1 = pos2 - pos1;
    glm::vec3 edge2 = pos3 - pos1;
    glm::vec2 deltaUV1 = uv2 - uv1;
    glm::vec2 deltaUV2 = uv3 - uv1;

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

    // triangle 2
    edge1 = pos3 - pos1;
    edge2 = pos4 - pos1;
    deltaUV1 = uv3 - uv1;
    deltaUV2 = uv4 - uv1;

    f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);


    bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);


    float quadVertices[] = {
        // positions            // normal         // texcoords  // tangent                          // bitangent
        pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
        pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
        pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

        pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
        pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
        pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
    };
    // 
    //Ground Tex
    unsigned int groundTexture;
    path = "textures\\ground\\Concrete_018_BaseColor.jpg";
    loadTexture(groundTexture, width, height, path);

    //Ground Tex Norm

    unsigned int groundTextureNorm;
    path = "textures\\ground\\Concrete_018_Normal.jpg";
    loadTexture(groundTextureNorm, width, height, path);

    int groundAttributes[5] = { 3, 3, 2, 3, 3};
    groundPlane.addData(glm::vec3(0, 0, 0), quadVertices, sizeof(quadVertices) / sizeof(quadVertices[0]), sizeof(groundAttributes) / sizeof(groundAttributes[0]), groundAttributes);
    groundPlane.setShape(GL_TRIANGLES);
    groundPlane.setObjPos(glm::vec3(0, 0, 0));
    groundPlane.setScale(20);
    //groundPlane.addIndexBuffer(Qindices, sizeof(Qindices) / sizeof(unsigned int));

    /* Loop until the user closes the window */
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    while (!glfwWindowShouldClose(window))
    {

  

        //// Measure speed
        //double currentTime = glfwGetTime();
        //nbFrames++;
        //if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
        //    // printf and reset timer
        //    printf("%f ms/frame\n", 1000.0 / double(nbFrames));
        //    nbFrames = 0;
        //    lastTime += 1.0;
        //}
            
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

        //START
        glm::mat4 lightProjection, lightView;
        float near_plane = 0.0f, far_plane = 10.5f;
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(light.getObjPos(), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 lightProjectionViewMatrix = lightProjection * lightView;

        createSM(SHADOW_WIDTH, SHADOW_HEIGHT, depthMapFBO, shaderProgram, gizmosShaderProgram, lightProjectionViewMatrix, depthMap);
        //END

        //scene
        drawSkybox(skyboxShaderProgram, skyboxVBO, skyboxVAO, sky);

        
        //drawGround(textureShaderProgram, gizmosShaderProgram, lightProjectionViewMatrix, depthMap, groundTexture, groundTextureNorm);

        drawTrack(trackShaderProgram, lightProjectionViewMatrix, splinesShaderProgram, gizmosShaderProgram);
        
        light.draw(primitivesShaderProgram, gizmosShaderProgram, *activeCamera);

        glm::vec3 cartPos = track.getCartPos();
        POVcamera.setPos(cartPos/* + glm::vec3(0.0f, 0.25f, 0.0f)*/);
        POVcamera.setFOV(track.getCartPos().y, SCR_WIDTH, SCR_HEIGHT);
        if (!freePOV && activeCamera == &POVcamera)
            POVcamera.setDir(track.getCartDir());

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void drawTrack(Shader& trackShaderProgram, const glm::mat4& lightProjectionViewMatrix, const Shader& splinesShaderProgram, const Shader& gizmosShaderProgram)
{
    trackShaderProgram.bind();
    trackShaderProgram.setUniformMatrix4fv("lightProjectionViewMatrix", lightProjectionViewMatrix);
    track.draw(splinesShaderProgram, trackShaderProgram, gizmosShaderProgram, *activeCamera, light);
}

void createDepthMapTexture(unsigned int& depthMap, const unsigned int& SHADOW_WIDTH, const unsigned int& SHADOW_HEIGHT, unsigned int depthMapFBO)
{
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void loadSkyboxTextures(std::string& path, unsigned char*& data, int& width, int& height, int& nrComponents)
{
    int i = 0;
    path = "textures\\skybox\\";
    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        data = stbi_load(&entry.path().string().c_str()[0], &width, &height, &nrComponents, 0);
        if (!data) {
            std::cout << "Cubemap tex failed to load at path: " << entry.path() << std::endl;
            exit(-1);
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        i++;
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}



void loadTexture(unsigned int& groundTexture, int& width, int& height, string path)
{
    glGenTextures(1, &groundTexture);
    glBindTexture(GL_TEXTURE_2D, groundTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    //Concrete_018_Normal
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
}
void createSM(const unsigned int& SHADOW_WIDTH, const unsigned int& SHADOW_HEIGHT, unsigned int depthMapFBO, const Shader& shaderProgram, const Shader& gizmosShaderProgram, const glm::mat4& lightProjectionViewMatrix, int depthMap)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    //glCullFace(GL_FRONT);
    groundPlane.draw(shaderProgram, gizmosShaderProgram, lightProjectionViewMatrix);
    track.draw(shaderProgram, lightProjectionViewMatrix);
    //glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void drawScene(Shader& skyboxShaderProgram, VertexBuffer& skyboxVBO, VertexArray& skyboxVAO, unsigned int sky, const Shader& gizmosShaderProgram, Shader& splinesShaderProgram, Shader& trackShaderProgram, Shader& primitivesShaderProgram)
{
    //drawSkybox(skyboxShaderProgram, skyboxVBO, skyboxVAO, sky);
    //drawGround(trackShaderProgram, gizmosShaderProgram);

    //Track

    //lighting cube
}

void drawGround(Shader& groundShaderProgram, const Shader& gizmosShaderProgram, glm::mat4 lightProjectionViewMatrix, int depthMap, int groundTexture, int groundTextureNorm)
{
    groundShaderProgram.bind();
    groundShaderProgram.setUniformMatrix4fv("lightProjectionViewMatrix", lightProjectionViewMatrix);
    groundShaderProgram.setInt("shadowMap", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    groundShaderProgram.setInt("baseTexture", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, groundTexture);
    groundShaderProgram.setInt("normalMap", 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, groundTextureNorm);
    //groundShaderProgram.bind();
    groundShaderProgram.setUniform4fv("lightColor", light.getObjColor());
    groundShaderProgram.setUniform3fv("lightOrigin", light.getObjPos());
    groundShaderProgram.setUniform3fv("camOrigin", activeCamera->getPos());

    groundPlane.draw(groundShaderProgram, gizmosShaderProgram, *activeCamera);
    groundShaderProgram.unbind();
}

void drawSkybox(Shader& skyboxShaderProgram, VertexBuffer& skyboxVBO, VertexArray& skyboxVAO, unsigned int sky)
{
    glDepthMask(GL_FALSE);
    skyboxShaderProgram.bind();
    skyboxVBO.bind();
    skyboxVAO.bind();
    glm::mat4 view = glm::mat4(glm::mat3(activeCamera->getViewMatrix()));
    skyboxShaderProgram.setUniformMatrix4fv("view", view);
    skyboxShaderProgram.setUniformMatrix4fv("projection", activeCamera->getProjectionMatrix());
    glBindTexture(GL_TEXTURE_CUBE_MAP, sky);
    skyboxShaderProgram.setUniformf("skyboxTexture", 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
}

int init(GLFWwindow*& window, bool& retflag)
{
    retflag = true;
    if (!glfwInit())
    {
        return -1;

    }
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cout << "Can't glew init: " << glewGetErrorString(err) << std::endl;
        exit(-1);
    }
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    //glEnable(GL_CULL_FACE);
    int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    }
    printWelcomeMessage();
    retflag = false;
    return {};
}

void printWelcomeMessage()
{
    std::cout << "RollerCoaster Simulator" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "Left click and drag to rotate camera" << std::endl;
    std::cout << "Right click and drag to move camera left and right" << std::endl;
    std::cout << "Scroll to zoom in and out" << std::endl;
    std::cout <<  std::endl;
    std::cout << "Left click to select" << std::endl;
    std::cout << "Middle click to add track point" << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

}

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
void APIENTRY glDebugOutput(GLenum source,
    GLenum type,
    unsigned int id,
    GLenum severity,
    GLsizei length,
    const char* message,
    const void* userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;

}

//### Call-backs ###
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (!activeCamera->isFrozen())
        activeCamera->processCursorMovement(window, xpos, ypos);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    xpos = ((xpos / width) * 2) - 1;
    ypos = -(((ypos / height) * 2) - 1);

    //plane creation (normal and distance from origin)
    glm::vec3 n(0, 1, -0);
    n = glm::normalize(n);
    float d = 0;

    RayCast rayCast(activeCamera->getPos(), activeCamera->getViewMatrix(), activeCamera->getProjectionMatrix(), xpos, ypos);
    track.cursorMovement(window, rayCast);
    light.cursorMovement(window, rayCast);
    groundPlane.cursorMovement(window, rayCast);
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    double xpos, ypos;
    //getting cursor position
    glfwGetCursorPos(window, &xpos, &ypos);

    //normalise screen coords
    xpos = ((xpos / width) * 2) - 1;
    ypos = -(((ypos / height) * 2) - 1);

    //plane creation (normal and distance from origin)
    glm::vec3 n(0, 1, 0);
    n = glm::normalize(n);
    float d = 0;

    RayCast rayCast(activeCamera->getPos(), activeCamera->getViewMatrix(), activeCamera->getProjectionMatrix(), xpos, ypos);

    //point creation
    float t = 0.0f;
    rayCast.planeCollision(glm::vec3(0, 0.5, 0), n, &t);

    if (activeCamera == &POVcamera)
        freePOV = true;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {

        if (t > 0 && glm::distance(rayCast.calculateRayPoint(t), glm::vec3(0, 0.5, 0)) < 20) {
            glm::vec3 p = rayCast.calculateRayPoint(t);
            glm::vec3 point(p.x, p.y, p.z);
            //points.push_back(point);
            track.addTrackPoint(point);
            //std::cout << "Cursor Pressed Position at " << xpos << " : " << ypos << std::endl;
            //points.push_back(glm::vec2(ray.x, ray.y));

        }
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        //track.leftMousePress(rayCast);
        if (track.leftMouseClick(rayCast, glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) or light.leftMouseClick(rayCast) or groundPlane.leftMouseClick(rayCast)) {
            freeCamera.freezeCam();
        }


    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {

        track.leftMouseRelease(rayCast);
        light.leftMouseRelease(rayCast);
        groundPlane.leftMouseRelease(rayCast);
        freeCamera.unfreezeCam();

    }
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        track.restartCart();
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        track.addChainLink();
    }
    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        track.loopTrack();
    }
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        track.changeOrientation(+10);
    }
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        track.changeOrientation(-10);
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        track.changeGizmoMode(Mode::rotate);
    }
    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        track.changeGizmoMode(Mode::translate);
    }
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        if (activeCamera == &POVcamera)
            activeCamera = &freeCamera;
        else
        {
            activeCamera = &POVcamera;
            freePOV = false;
        }

    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
        track.resetTrack();

    else if (key == GLFW_KEY_U && action == GLFW_PRESS) {

        track.removeLastPoint();
    }


}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    freeCamera.processScrollWheelMovement(yoffset);
}
