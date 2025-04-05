#pragma once
#include <GLFW/glfw3.h>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/glm.hpp"
#include "glm/ext.hpp"


class Camera
{
    float vel = 0;
    float lastX = 400;
    float lastY = 300;
    float pitch = 0;
    float yaw = -90;
    float orientation = 0;
    glm::vec3 eye;
    glm::vec3 up;

    glm::mat4 viewMat;
    glm::mat4 projectionMat;

    glm::vec3 direction;
    bool frozen = false;
public:
    Camera();
    Camera(glm::mat4 projectionMat, glm::vec3 eye, glm::vec3 direction, glm::vec3 up);
    void init(glm::mat4 projectionMat, glm::vec3 eye, glm::vec3 direction, glm::vec3 up);

    void updateDirection();

    void processCursorMovement(GLFWwindow* window, double xpos, double ypos);
    void processScrollWheelMovement(double yoffset);

    void freezeCam();
    void unfreezeCam();

    glm::mat4 getViewMatrix() const;
    void setProjectionMatrix(glm::mat4 projectionMat);
    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getProjectionViewMatrix() const;

    void setViewMatrix(glm::mat4 newViewMat);


    glm::vec3 getPos() const;
    void setPos(glm::vec3 pos);
    void setDir(std::tuple<float, float, float> directionAngles);
    void setFOV(float vel, float width, float height);
    bool isFrozen();
};

