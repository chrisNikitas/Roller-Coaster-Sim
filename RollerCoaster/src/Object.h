#pragma once
#include <vector>

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>


#include "Shader.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "BufferLayout.h"
#include "IndexBuffer.h"
#include "Gizmo.h"




class Object
{
	bool movable = false;
	bool indexBuffer = false;
	float scale;
	VertexBuffer VBO;
	VertexArray VAO;
	IndexBuffer IBO;
	BufferLayout BL;

	int verticesCount;

	GLenum shape = GL_POINTS;
	GLsizei shapeCount = 1;

	Gizmo gizmo;
	glm::mat4 model = glm::mat4(1.0f);

	glm::vec3 objPos;
	glm::vec4 objColor = glm::vec4(1.0, 1.0, 1.0, 1.0);
public:
	Object(bool movable = true, float scale = 1);
	//Object(std::vector<glm::vec3> vertices, int attributesNum, int attributes[]);
	void cursorMovement(GLFWwindow* window, RayCast rayCast);
	bool leftMouseClick(RayCast rayCast);
	bool selectGizmo(RayCast rayCast);
	bool leftMouseRelease(RayCast rayCast);

	void draw(Shader objShader, Shader gizmoShader, const Camera& camera);
	void draw(Shader objShader, Shader gizmoShader, glm::mat4 projectionViewMatrix);

	void moveObject(RayCast rayCast);

	void addData(glm::vec3 objPos, std::vector<glm::vec3> vertices, int attributesNum, int attributes[]);
	void addData(glm::vec3 objPos, float vertices[], int verticesCount, int attributesNum, int attributes[]);
	
	void setShape(GLenum shape);
	void setShapeCount(GLsizei count);

	void addIndexBuffer(unsigned int indices[], unsigned int count);

	glm::vec4 getObjColor();
	void setObjColor(glm::vec4 color);

	glm::vec3 getObjPos();
	void setObjPos(glm::vec3 pos);

	void setScale(float scale);

};

