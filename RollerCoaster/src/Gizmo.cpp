#include "Gizmo.h"
//This Class was made with help from:
//https://nelari.us/post/gizmos/

Gizmo::Gizmo() {

}

Gizmo::Gizmo(glm::vec3 pos)
{
    transform.pos = pos;

    setupBuffers();
}

void Gizmo::init(glm::vec3 pos) {
    transform.pos = pos;
    setupBuffers();
    previousCursorPos = glm::vec2(0, 0);
}

void Gizmo::setupBuffers()
{
    traVB.init();
    traVA.init();
    float vertices[] = { 0, 0, 0,
                         axisLength ,0, 0,
                         0 ,0, 0,
                         0, axisLength, 0,
                         0 ,0, 0,
                         0, 0, axisLength };
    traVB.addData(vertices, sizeof(vertices));
    traBL.addAttribute <float>(3);
    traVA.addBufferWithLayout(traVB, traBL);

    std::vector<glm::vec3> rotVertices;
    float angleInc = (2 * M_PI) / rotGizmoSegs;
    rotVertices.push_back(glm::vec3(0, 0, 0));
    for (int i = 0; i < rotGizmoSegs + 1; i++)
    {
        float segmentAngle = i * angleInc;
        glm::vec3 pos;
        pos.x = rotGizmoRadius * sin(segmentAngle);

        pos.y = rotGizmoRadius * cos(segmentAngle);

        pos.z = rotGizmoRadius * sin(segmentAngle);
        rotVertices.push_back(pos);
    }

    rotVB.init();
    rotVA.init();

    rotVB.addData(&rotVertices[0], rotVertices.size() * sizeof(glm::vec3));
    rotBL.addAttribute <float>(3);
    rotVA.addBufferWithLayout(rotVB, rotBL);
}

void Gizmo::draw(Shader shader, const Camera &camera) {
    if (mode == Mode::translate) {
        drawTranslationGizmo(shader, camera);
    }
    else if (mode == Mode::rotate) {
        drawRotationGizmo(shader, camera);
    }
}

void Gizmo::drawTranslationGizmo(Shader shader, const Camera &camera)
{
    shader.bind();
    shader.setUniformMatrix4fv("model", glm::mat4(1.0f));
    shader.setUniformMatrix4fv("projectionView", camera.getProjectionViewMatrix());
    for (int i = 0; i < 3; i++)
    {
        glm::vec3 axis_end = glm::vec3(0.f, 0.f, 0.f);
        axis_end[i] = .25f;

        glm::vec4 axis_color = glm::vec4(0.f, 0.f, 0.f, 1.f);
        axis_color[i] = 1.f;

        //if !isActive then no axis is selected
        if (i == selectedAxis && isActive)
        {
            axis_color = glm::vec4(1.f, 0.65f, 0.f, 1.f);
        }
        traVB.bind();
        traVA.bind();


        shader.setUniform4fv("colour", axis_color);

        shader.setUniform3fv("d", transform.pos);
        shader.setUniform3fv("coeff", glm::vec3(1,1,1));
        glDrawArrays(GL_LINES, i * 2, 2);
    }
    shader.unbind();
}

void Gizmo::drawRotationGizmo(Shader shader, const Camera &camera) {
    ////circles
    shader.bind();
    shader.setUniformMatrix4fv("model", glm::mat4(1.0f));
    shader.setUniformMatrix4fv("projectionView", camera.getProjectionViewMatrix());

    float angleInc = (2 * M_PI) / rotGizmoSegs;


    float directionAngle = atan(-transform.dir.z / transform.dir.x);
    if (transform.dir.x < 0)
    {
        directionAngle = M_PI + directionAngle;
    }


    for (int i = 0; i < rotGizmoSegs; i++)
    {
        float segmentAngle = i * angleInc;
        glm::vec3 pos;

        glm::vec3 coeff = glm::vec3(sin(directionAngle), 1, cos(directionAngle));

        //glm::vec3 coeff = glm::vec3(
        //    sin(directionAngle) * cos(transform.rotation * M_PI / 180),
        //    cos(transform.rotation * M_PI / 180),
        //    cos(directionAngle) * sin(transform.rotation * M_PI / 180));

        shader.setUniform3fv("d", transform.pos);
        shader.setUniform3fv("coeff", coeff);
        shader.setUniform4fv("colour", glm::vec4(0, 1, 0, 1));
        if (isActive) {
            shader.setUniform4fv("colour", glm::vec4(1, 1, 0, 1));
        }
        rotVA.bind();
        rotVB.bind();
        glDrawArrays(GL_LINES, i, 2);
    }

    shader.unbind();
}

float Gizmo::rotatePoint(RayCast rayCast)
{
    float hit;
    rayCast.planeCollision(getPos(), getDir(), &hit);
    glm::vec3 point = rayCast.calculateRayPoint(hit);

    //avoid cursor pos causing snap back movement
    if (previousCursorPos == glm::vec2(0, 0)) {
        previousCursorPos = glm::vec2(rayCast.mouseX, rayCast.mouseY);
    }

    float result = (rayCast.mouseX - previousCursorPos.x);
    previousCursorPos = glm::vec2(rayCast.mouseX, rayCast.mouseY);
    return result;
}

glm::vec3 Gizmo::movePoint(RayCast rayCast) {
    unsigned int axis = getSelectedAxis();
    float hitDist;
    if (axis == 0 || axis == 2) {
        //XZ plane
        glm::vec3 n = glm::vec3(0, 1, 0);
        glm::vec3 c = getPos();
        rayCast.planeCollision(c, n, &hitDist);
        glm::vec3 p = rayCast.calculateRayPoint(hitDist);

        float o = getOffsetFromO();
        if (axis == 0)
        {
            if (abs(getPos().y - rayCast.rayOrigin.y) < 1)
            {
                float hitDist2;
                glm::vec3 n = glm::vec3(0, 0, 1);
                glm::vec3 c = getPos();
                rayCast.planeCollision(c, n, &hitDist2);
                p = rayCast.calculateRayPoint(hitDist2);
            }
            transform.pos.x = p.x - o;
        }
        if (axis == 2)
        {
            if (abs(getPos().y - rayCast.rayOrigin.y) < 1)
            {
                float hitDist2;
                glm::vec3 n = glm::vec3(1, 0, 0);
                glm::vec3 c = getPos();
                rayCast.planeCollision(c, n, &hitDist2);
                p = rayCast.calculateRayPoint(hitDist2);
            }
            transform.pos.z = p.z - o;
        }
    }
    else if (axis == 1)
    {
        //YX & YZ plane
        //YX
        glm::vec3 n = glm::vec3(1, 0, 0);
        glm::vec3 c = getPos();
        rayCast.planeCollision(c, n, &hitDist);
        glm::vec3 p = rayCast.calculateRayPoint(hitDist);

        //YZ if necessary
        if (abs(getPos().x - rayCast.rayOrigin.x) < 1)
        {
            float hitDist2;
            glm::vec3 n = glm::vec3(0, 0, 1);
            glm::vec3 c = getPos();
            rayCast.planeCollision(c, n, &hitDist2);
            p = rayCast.calculateRayPoint(hitDist2);
        }

        transform.pos.y = p.y - getOffsetFromO();
    }
    return getPos();
}

void Gizmo::calculateOffsetFromO(RayCast rayCast, float hit)
{
    glm::vec3 rayPoint = rayCast.calculateRayPoint(hit);
    offsetFromO = glm::distance(getPos(), rayPoint);
}

bool Gizmo::selectTranslationTool(RayCast rayCast)
{
    for (unsigned int axis = 0; axis < 3; axis++)
    {
        glm::vec3 dir = glm::vec3(0, 0, 0);
        dir[axis] = 1;
        float hit;
        if (rayCast.lineCollision(getPos(), dir, &hit) && 
            (glm::distance(rayCast.calculateRayPoint(hit), getPos()) < 0.5 )) {

            selectAxis(axis);
            enableGizmo();
            calculateOffsetFromO(rayCast, hit);

            return true;
        }
    }
    return false;
}

bool Gizmo::selectTool(RayCast rayCast) 
{
    if (mode == Mode::translate) 
    {
        if (selectTranslationTool(rayCast))
            return true;
    }
    else if (mode == Mode::rotate) 
    {
        if (selectRotWheel(rayCast))
            return true;
    }
    return false;
}

bool Gizmo::selectRotWheel(RayCast rayCast)
{

    float hit;
    if (rayCast.circleCollision(transform.pos, transform.dir, rotGizmoRadius, &hit)) {
        enableGizmo();
        calculateOffsetFromO(rayCast, hit);
        return true;
    }
    return false;

}

glm::vec3 Gizmo::getPos() {
    return transform.pos;
}

void Gizmo::selectAxis(unsigned int axis)
{
    selectedAxis = axis;
}

unsigned int Gizmo::getSelectedAxis()
{
    return selectedAxis;
}

void Gizmo::enableGizmo() {
    isActive = true;
}


void Gizmo::disableGizmo(){
    isActive = false;
    offsetFromO = 0;
    previousCursorPos = glm::vec2(0, 0);
}

bool Gizmo::getIsActive()
{
    return isActive;
}

void Gizmo::setPos(glm::vec3 pos)
{
    transform.pos = pos;
}

glm::vec3 Gizmo::getDir()
{
    return transform.dir;
}

void Gizmo::setDir(glm::vec3 dir)
{
    transform.dir = dir;
}
void Gizmo::setGizmoMode(Mode m) {
    mode = m;
}

void Gizmo::KeyPressed(int key, int action) {
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        mode = Mode::rotate;
    }
    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        mode = Mode::translate;
    }
}
