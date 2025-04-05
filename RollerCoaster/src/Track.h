#pragma once
#include <vector>
#include <list>
#include "Shader.h"
#include "Camera.h"
#include "RayCast.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "Gizmo.h"
#include "Object.h"
#include "Model.h"

#include "glm/glm.hpp"
#include"glm/gtc/quaternion.hpp" 
#include"glm/gtx/quaternion.hpp"
#include "glm/ext.hpp"

#include "glm/gtx/perpendicular.hpp"
#define _USE_MATH_DEFINES
#include <math.h>


struct tPoint {
	//float x;
	//float y;
	//float z;
	glm::vec3 pos;
	float distFromO;
	float orientation = 0;
	glm::vec3 dir;
	bool selected = false;
}typedef tPoint;

class Track
{
private:
	std::vector<glm::vec3> upVertices;
	std::vector<glm::vec3> rightVertices;
	std::vector<glm::vec3> tangentVertices;
	glm::vec3 upPrev;
	glm::vec3 pNext;
	glm::vec3 dirPrev;
	glm::vec3 tPos;
	float cartDirAngle;
	float cartPitch;
	glm::mat4 cartModel;
	glm::mat4 camModel;
	glm::vec3 up;
	glm::vec3 rightV;
	glm::vec3 firstUp;
	//glm::vec3 rightV;
	int lastI = 0;
	bool movingPoint = false;
	bool loopedTrack = false;
	double t0;
	double tNow;
	double tPrev = 0;
	const float g = 9.81 / 9;
	double splineInterval = 0.1f;
	int circleSegments = 10;
	float trackRadius = 0.01f;

	float trackDist;
	float distBtwTrackRails = 0.12f;
	float distBtwConnections = 0.1f;

	float supportRadius = 0.05f;
	float distBtwSupports = 0.9f;


	int chainLinkStart = -1;
	int chainLinkEnd = -1;
	float chainLinkRadius = 0.005f;

	glm::vec3 trackColour = glm::vec3(0.66f, 0.19f, 0.24f);
	//glm::vec3 trackColour = glm::vec3(1.0f, 0.2f, 0.24f);
	glm::vec3 supportsColour = glm::vec3(0.2f, 0.2f, 0.2f);
	//glm::vec3 supportsColour = glm::vec3(0, 0, 0);

	//std::vector<std::vector<glm::vec3>> track;
	std::vector<std::pair<glm::vec3, glm::vec3>> trackL;
	std::list<glm::vec3> spline;
	std::vector<glm::vec3> trackVertices;
	std::vector<unsigned int> trackIndices;
	std::vector<glm::vec3> trackData;
	std::vector<glm::vec3> trackConnections;
	std::vector<glm::vec3> trackChainLink;
	std::vector<glm::vec3> trackChainLinkConnections;
	std::vector<glm::vec3> chainLinkData;
	std::vector<unsigned int> chainLinkIndices;
	std::vector<glm::vec3> trackSupportVertices;
	std::vector<float> trackDistL;
	//std::vector<glm::vec3> trackSupportIndices;

	std::vector<Gizmo> gizmos;

	VertexArray tPointsVA;
	VertexBuffer tPointsVB;
	VertexArray trackVA;
	VertexBuffer trackVB;
	IndexBuffer trackIB;
	BufferLayout BL;

	//lighting
	//Object light;
	VertexBuffer lightVBO;
	VertexArray lightVAO;
	BufferLayout lightBL;
	glm::vec3 lightPos = glm::vec3(0, 2, 0);
	glm::vec4 lightColor = glm::vec4(1, 1, 1, 1);

	//float cartT = 0;
	Model cart;
	glm::vec3 cartPos;
	glm::vec3 cartDir;
	int selectedPoint = -1;
	float maxT;
	std::vector<unsigned int> selectedPointsIndices;

	glm::mat4 model = glm::mat4(1.0f);
	std::vector<tPoint> points;
	glm::vec2 previousCursorPos;

	void reset();

	glm::vec3 calculateSplinePoint(float t, std::vector<tPoint> splinePoints);
	//rails
	void getTrackVertices(std::pair<glm::vec3, glm::vec3> cur_track, glm::vec3 n, float circleRadius);
	void getChainLinkVertices(std::pair<glm::vec3, glm::vec3> cur_track, glm::vec3 n, float circleRadius);
	void getRailVertices(std::vector<glm::vec3> &vec, std::pair<glm::vec3, glm::vec3> cur_track, glm::vec3 n, float circleRadius);
	//connectors
	void createTrackConnectorVertices(float ourT, glm::vec3 dir, float orientationAngle, float circleRadius);
	void createChainLinkConnectorVertices(float ourT, glm::vec3 dir, float orientationAngle, float circleRadius);
	void createConnectorVertices(std::vector<glm::vec3>& vec, float ourT, glm::vec3 dir, float orientationAngle, float circleRadius);

	void getSupportVertices(float ourT, glm::vec3 dir, float circleRadius);

	void triangulateVertices();
	int triangulateMainTrack(int startingIndex);
	int triangulateChainLink(int startingIndex);
	int triangulateSupports(int startingIndex);

	void getConnectors(float deltaDist, float t, float orientationAngle, glm::vec3 dir, float& distanceToNextPoint);
	void getSupports(float deltaDist, float t, float orientationAngle, glm::vec3 dir, float &distanceToNextPoint);

	//helpers
	std::pair<glm::vec3, glm::vec3> getRailsFromSpline(float ourT, glm::vec3 dir, float orientationAngle, float distBtwLines);
	glm::vec3 getRailCrossSectionVertices(glm::vec3 centerPoint, float segmentAngle, float inclineAngleXY, float inclineAngleZY, float directionAngle, float circleRadius);
	float getCurrentOrientation(float t);
	float getCurrentDirectionAngle(glm::vec3 dir);
	float getCurrentInclineAngleWRTX(glm::vec3 dir);
	float getCurrentInclineAngleWRTZ(glm::vec3 dir);

	glm::vec3 trackOrigin;
	float prevCartT = 0;
	float preDockedCartVel = 0;
	float preChainLinkCartVel = 0;
	float curDist = 0;
	float cartVel = 1;
	float cartFriction= 0.02f;
	float cartAccel;
	bool atInitialPos = true;
	bool docking = false;
	bool onChailLink = false;
	glm::vec3 cartPosPrev;
	float cartOrientationAngle;
	glm::vec3 cartPosPrev2;

	void updateCart(double deltaTime, float cartT);
	float calcCartT(float d);

	void deselectPoints();
public:
	glm::vec3 axis;

	Track();
	void init();
	void createTrack();
	void selectRight();
	void selectLeft();
	void moveCubeUp();
	void moveCubeDown();
	void moveCubeLeft();
	void moveCubeRight();
	void moveCubeIn();
	void moveCubeOut();
	void changeOrientation(int inc);

	void draw(Shader trackShader, glm::mat4 projectionViewMatrix);
	void draw(Shader splinesShader, Shader trackShader, Shader gizmosShader, const Camera &camera, Object light);
	void drawLightCube(Shader primitivesShader);

	void drawGizmos(Shader gizmosShader, Camera camera);
	void drawCart(Shader& shader);
	void drawTrack(Shader& shader);
	void drawTPoints(Shader& shader);
	void drawTrackSplines(Shader& shader);
	void drawTBN(Shader shader, glm::mat4 projectionViewMatrix);
	void addTrackPoint(glm::vec3 point);
	bool selectTrackPoint(RayCast ray, bool multiPoint);
	bool selectGizmo(RayCast rayCast);

	void cursorMovement(GLFWwindow* window, RayCast rayCast);
	bool leftMouseClick(RayCast rayCast, bool multiPoint);
	bool leftMouseRelease(RayCast rayCast);
	void moveTrackPoint(RayCast rayCast);
	void rotateTrackPoint(RayCast rayCast);
	void removeLastPoint();
	void resetTrack();

	void addChainLink();
	void loopTrack();
	void restartCart();
	void changeGizmoMode(Mode mode);

	glm::vec3 getCartPos();
	std::tuple<float, float, float> getCartDir();
	float getCartVel();
};

