#include "Camera.h"
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
//This Class was made with help from:
//https://learnopengl.com/Getting-started/Camera

Camera::Camera()
{

}

Camera::Camera(glm::mat4 projectionMat, glm::vec3 eye, glm::vec3 direction, glm::vec3 up)
{
    this->projectionMat = projectionMat;
    this->eye = eye;
    this->direction = direction;
    this->up = up;
}

void Camera::init(glm::mat4 projectionMat, glm::vec3 eye, glm::vec3 direction, glm::vec3 up) {
    this->projectionMat = projectionMat;
    this->eye = eye;
    this->direction = direction;
    this->up = up;
}


void Camera::updateDirection()
{
    if (pitch > 89)
        pitch = 89;
    else if (pitch < -89)
        pitch = -89;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    direction = glm::normalize(direction);
    glm::mat4 rot = glm::mat4(1);
    up = glm::rotate(glm::vec3(0, 1, 0), -orientation, direction);
}

void Camera::processCursorMovement(GLFWwindow* window, double xpos, double ypos)
{
    float xOffset, yOffset;
    xOffset = lastX - xpos;
    yOffset = ypos - lastY;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        yaw += xOffset * 0.05;
        pitch += yOffset * 0.05;

        updateDirection();
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        glm::vec3 right = glm::normalize(glm::cross(up, direction));
        glm::vec3 trueUp = glm::normalize(glm::cross(direction, right));
        eye += -right * xOffset * 0.01;
        eye += trueUp * yOffset * 0.01;
    }
    lastX = xpos;
    lastY = ypos;

    //if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    //{
    //    float positionOffset

    //    updateDirection();
    //}
}

void Camera::processScrollWheelMovement(double yoffset)
{
    if (yoffset > 0)
        eye += direction;
    else
        eye -= direction;
}

void Camera::freezeCam()
{
    frozen = true;
}

void Camera::unfreezeCam()
{
    frozen = false;
}

glm::mat4 Camera::getViewMatrix() const
{

    glm::mat4 view = glm::lookAt(eye, eye + direction, up);
    return view;
}

void Camera::setProjectionMatrix(glm::mat4 projectionMat) 
{
    this->projectionMat = projectionMat;
}

glm::mat4 Camera::getProjectionMatrix() const
{
    return projectionMat;
}
glm::mat4 Camera::getProjectionViewMatrix() const
{
    //glm::mat4 rot = glm::mat4(1);
    //rot = glm::rotate(rot, orientation, eye + direction);
    //glm::vec4 ups = glm::vec4(0, 1, 0, 1);
    //ups = rot * ups;
    //glm::vec3 ourup = glm::vec3(-ups.x, ups.y, -ups.z);
    glm::mat4 view = glm::lookAt(glm::vec3(eye.x, eye.y, eye.z), eye + direction, up);
    return projectionMat * view;
}

void Camera::setViewMatrix(glm::mat4 newViewMat)
{
    viewMat = newViewMat;
}


glm::vec3 Camera::getPos() const
{
    return eye;
}

void Camera::setPos(glm::vec3 pos)
{
    eye = pos;
}

void Camera::setDir(std::tuple<float, float, float> directionAngles)
{
    yaw = glm::degrees(-std::get<0>(directionAngles));
    pitch = glm::degrees(std::get<1>(directionAngles));
    orientation = std::get<2>(directionAngles);
    //eye = glm::vec3(eye.x + ((signbit(sin(glm::radians(yaw))) ? -1 :1) * pow(sin(glm::radians(yaw)),2) * sin(orientation)) / 6, eye.y  - (pow(sin(orientation),2) / 10), eye.z + ((signbit(cos(glm::radians(yaw))) ? 1 : -1) * pow(cos(glm::radians(yaw)), 2) * sin(orientation)) / 6);
    updateDirection();

    //yaw = -directionAngles.first;
    //pitch = directionAngles.second;
}

void Camera::setFOV(float vel, float width, float height)
{
    //prevents crashing when minimizing
    if (width == 0 or height == 0)
        return;

    this->vel = vel;
    projectionMat = glm::perspective(glm::radians(70.0f + (vel)*4), width / height, 0.1f, 100.0f);
    //std::cout << vel / 5<< std::endl;

}

bool Camera::isFrozen()
{
    return frozen;
}
