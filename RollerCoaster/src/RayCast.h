#pragma once
#include <GLFW/glfw3.h>
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>


class RayCast
{
private:
	glm::vec3 calculateRay(float xpos, float ypos, glm::mat4 transfMatInv);
	glm::vec3 calculateRay(float xpos, float ypos, glm::mat4 projectionMatrix, glm::mat4 viewMatrix);
public:
	glm::vec3 rayDir;
	glm::vec3 rayOrigin;
	float mouseX;
	float mouseY;
	RayCast(glm::vec3 const& rayOrigin, glm::mat4 viewMatrix, glm::mat4 projectionMatrix, double mouseXPos, double mouseYPos);
	bool planeCollision(glm::vec3 planeCenter, glm::vec3 planeNormal, float* hitDist);
	bool planeCollision(float planeDist, glm::vec3 planeNormal, float* hitDist);
	bool boxCollision(glm::vec3 min, glm::vec3 max);
	bool lineCollision(glm::vec3 l2Origin, glm::vec3 l2Dir, float* hitDist);
	bool circleCollision(glm::vec3 center, glm::vec3 dir, float r , float* hitDist);
	glm::vec3 calculateRayPoint(float hitDist);
};

