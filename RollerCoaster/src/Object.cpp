#include "Object.h"

Object::Object(bool movable, float scale) {
	this->movable = movable;
	this->scale = scale;
}

//Object::Object(std::vector<glm::vec3> vertices, int attributesNum, int attributes[])
//{
//	this->verticesCount = vertices.size();
//	VBO.init();
//	VAO.init();
//	VBO.addData(&vertices[0], vertices.size() * sizeof(glm::vec3));
//	for (unsigned int i = 0; i < attributesNum; i++)
//		BL.addAttribute<float>(attributes[i]);
//	VAO.addBufferWithLayout(VBO, BL);
//
//}

void Object::addData(glm::vec3 objPos, std::vector<glm::vec3> vertices, int attributesNum, int attributes[])
{
	this->objPos = objPos;
	this->verticesCount = vertices.size();
	gizmo.init(objPos);
	VBO.init();
	VAO.init();
	VBO.addData(&vertices[0], vertices.size() * sizeof(glm::vec3));
	for (unsigned int i = 0; i < attributesNum; i++)
		BL.addAttribute<float>(attributes[i]);
	VAO.addBufferWithLayout(VBO, BL);
}

void Object::addData(glm::vec3 objPos, float vertices[], int verticesCount, int attributesNum, int attributes[])
{
	this->objPos = objPos;
	gizmo.init(objPos);

	VBO.init();
	VAO.init();

	//size of array is verticesCount / sum of attributes
	int vertexSize = 0;
	for (unsigned int i = 0; i < attributesNum; i++) {
		vertexSize += attributes[i];
	}
	this->verticesCount = verticesCount / vertexSize;
	VBO.addData(vertices, verticesCount*sizeof(float));
	for (unsigned int i = 0; i < attributesNum; i++)
		BL.addAttribute<float>(attributes[i]);
	VAO.addBufferWithLayout(VBO, BL);
}

void Object::moveObject(RayCast rayCast) 
{
	objPos = gizmo.movePoint(rayCast);
	model = glm::translate(model, objPos);
}

void Object::cursorMovement(GLFWwindow* window, RayCast rayCast) {
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (gizmo.getIsActive())
		{
			moveObject(rayCast);			
		}
	}
}

bool Object::leftMouseClick(RayCast rayCast)
{
	return selectGizmo(rayCast);
}

bool Object::leftMouseRelease(RayCast rayCast)
{

	gizmo.disableGizmo();
	//freezeCam = false;
	
	return true;
}


bool Object::selectGizmo(RayCast rayCast)
{

	if (gizmo.selectTool(rayCast) && movable) {
		//freezeCam = true;
		return true;
	}
	return false;
}

void Object::draw(Shader objShader, Shader gizmoShader, const Camera& camera)
{
	model = glm::mat4(1.0f);

	model = glm::translate(model, objPos);
	model = glm::scale(model, glm::vec3(scale));

	objShader.bind();
	objShader.setUniformMatrix4fv("projectionView", camera.getProjectionViewMatrix());
	objShader.setUniformMatrix4fv("model", model);
	//objShader.setUniform3fv("displacement", getObjPos());


	VAO.bind();
	VBO.bind();
	if (indexBuffer) {
		IBO.bind();
		glDrawElements(shape, verticesCount, GL_UNSIGNED_INT, nullptr);
		IBO.unbind();

	}
	else {
		glDrawArrays(shape, 0, verticesCount);
	}
	VAO.unbind();
	VBO.unbind();

	objShader.unbind();

	if (movable)
		gizmo.draw(gizmoShader, camera);
}
void Object::draw(Shader objShader, Shader gizmoShader, glm::mat4 projectionViewMatrix)
{
	model = glm::mat4(1.0f);
	model = glm::translate(model, objPos);
	model = glm::scale(model, glm::vec3(scale));

	objShader.bind();
	objShader.setUniformMatrix4fv("projectionView", projectionViewMatrix);
	objShader.setUniformMatrix4fv("model", model);
	//objShader.setUniform3fv("displacement", getObjPos());


	VAO.bind();
	VBO.bind();
	if (indexBuffer) {
		IBO.bind();
		glDrawElements(shape, verticesCount, GL_UNSIGNED_INT, nullptr);
		IBO.unbind();

	}
	else {
		glDrawArrays(shape, 0, verticesCount);
	}
	VAO.unbind();
	VBO.unbind();

	objShader.unbind();

	//if (movable)
		//gizmo.draw(gizmoShader, camera);
}

void Object::addIndexBuffer(unsigned int indices[], unsigned int count) {
	IBO.init();
	IBO.addData(indices, count);
	verticesCount = count;
	indexBuffer = true;
}


void Object::setShape(GLenum shape)
{
	this->shape = shape;
}

void Object::setShapeCount(GLsizei count)
{
	this->shapeCount = count;
}


glm::vec4 Object::getObjColor()
{
	return objColor;
}

void Object::setObjColor(glm::vec4 color)
{
	objColor = color;
}

glm::vec3 Object::getObjPos() {
	return objPos;
}

void Object::setObjPos(glm::vec3 pos)
{
	objPos = pos;
}

void Object::setScale(float scale) {
	this->scale = scale;
}