#pragma once
#include <mutex>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/ext.hpp"
#include "Shader.h"
#include "Camera.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "BufferLayout.h"
#include "RayCast.h"
#define _USE_MATH_DEFINES
#include <math.h>

enum class Mode {
	none,
	translate,
	rotate
};

struct Transform {
	glm::vec3 pos;
	float rotation;
	glm::vec3 dir;
} typedef Transform;

class Gizmo
{
	float axisLength = 0.5;
	Shader gizmosShaderProgram;
	
	VertexBuffer traVB;
	VertexArray traVA;
	BufferLayout traBL;
	
	VertexBuffer rotVB;
	VertexArray rotVA;
	BufferLayout rotBL;

	bool isActive = false; 
	unsigned int selectedAxis = -1;
	Transform transform;

	unsigned int rotGizmoSegs = 20;
	float rotGizmoRadius = .10f;

	float offsetFromO = 0;
	glm::vec2 previousCursorPos;
	
	void setupBuffers();
	
	void calculateOffsetFromO(RayCast rayCast, float hit);

	void drawTranslationGizmo(Shader shader, const Camera &camera);
	void drawRotationGizmo(Shader shader, const Camera &camera);

	bool selectRotWheel(RayCast rayCast);
	bool selectTranslationTool(RayCast rayCast);
	void selectAxis(unsigned int axis);


public:
	Gizmo();
	Gizmo(glm::vec3 pos);
	void init(glm::vec3 pos);
	Mode mode = Mode::translate;
	glm::vec3 previousIntersect;
	//bool selectAxis(RayCast rayCast);
	bool selectTool(RayCast rayCast);
	//glm::vec3 manipulate(RayCast rayCast);
	void draw(Shader shader, const Camera &camera);

	glm::vec3 movePoint(RayCast rayCast);
	float rotatePoint(RayCast rayCast);

	void enableGizmo();
	void disableGizmo();

	void setGizmoMode(Mode m);

	unsigned int getSelectedAxis();
	bool getIsActive();

	glm::vec3 getPos();
	void setPos(glm::vec3 pos);

	glm::vec3 getDir(); 
	void setDir(glm::vec3 dir);

	void setOffsetFromO(float o) { offsetFromO = o; }
	float getOffsetFromO() { return offsetFromO; }
	
	void KeyPressed(int key, int action);
};