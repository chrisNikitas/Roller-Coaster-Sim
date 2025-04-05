#include "RayCast.h"
//This Class was made with help from:
//https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection
//https://antongerdelan.net/opengl/raycasting.html
//https://nelari.us/post/gizmos/
//https://stackoverflow.com/questions/23975555/how-to-do-ray-plane-intersection

RayCast::RayCast(glm::vec3 const &rayOrigin, glm::mat4 viewMatrix, glm::mat4 projectionMatrix, double mouseXPos, double mouseYPos)
{
    this->rayOrigin = rayOrigin;
    this->mouseX = mouseXPos;
    this->mouseY = mouseYPos;
    glm::mat4 transfMat = viewMatrix * projectionMatrix;
    glm::mat4 transfMatInv = glm::inverse(transfMat);
    this->rayDir = calculateRay(mouseXPos, mouseYPos, projectionMatrix, viewMatrix);
}

bool RayCast::planeCollision(glm::vec3 planeCenter, glm::vec3 planeNormal, float* hitDist)
{

    float denom = glm::dot(rayDir, planeNormal);
    if (fabs(denom) <= 1e-6) {
        std::cout << "Nope" << std::endl;
        return false;
    }
    glm::vec3 p0l0 = planeCenter - rayOrigin;
    *hitDist = glm::dot(p0l0, planeNormal) / denom;
    
    return true;
}

bool RayCast::planeCollision(float planeDistFromO, glm::vec3 planeNormal, float *hitDist)
{
    float denom = glm::dot(rayDir, planeNormal);
    if (abs(denom) <= 1e-6)
    {
        return false;
    }
    *hitDist = glm::dot(planeDistFromO - rayOrigin, planeNormal) / denom;
    if (*hitDist >= 0) return true; 
}

bool RayCast::boxCollision(glm::vec3 min, glm::vec3 max)
{
    float tMinX = (min.x - rayOrigin.x) / rayDir.x;
    float tMaxX = (max.x - rayOrigin.x) / rayDir.x;

    if (tMinX > tMaxX)
    {
        float temp = tMinX;
        tMinX = tMaxX;
        tMaxX = temp;
    }

    float tMinY = (min.y - rayOrigin.y) / rayDir.y;
    float tMaxY = (max.y - rayOrigin.y) / rayDir.y;

    if (tMinY > tMaxY)
    {
        float temp = tMinY;
        tMinY = tMaxY;
        tMaxY = temp;
    }
     
    float tMinZ = (min.z - rayOrigin.z) / rayDir.z;
    float tMaxZ = (max.z - rayOrigin.z) / rayDir.z;

    if (tMinZ > tMaxZ)
    {
        float temp = tMinZ;
        tMinZ = tMaxZ;
        tMaxZ = temp;
    }


    if (tMinX > tMaxY or tMaxX < tMinY) { return false; }
    //tMin is the greatest from tMinX, Y and Z
    //tMax is the smallest from tMaxX, Y and Z
    float tMin = tMinX;
    float tMax = tMaxX;

    if (tMin < tMinY) { tMin = tMinY; }
    if (tMax > tMaxY) { tMax = tMaxY; }

    if (tMin > tMaxZ or tMax < tMinZ) { return false; }

    if (tMin < tMinZ) { tMin = tMinZ; }
    if (tMax > tMaxZ) { tMax = tMaxZ; }


    return true;
}

bool RayCast::lineCollision(glm::vec3 l2Origin, glm::vec3 l2Dir, float* hitDist)
{
  
    glm:: vec3 dp = l2Origin - rayOrigin;
    float v1v1 = glm::dot(rayDir, rayDir);
    float v2v2 = glm::dot(l2Dir, l2Dir);
    float v1v2 = glm::dot(rayDir, l2Dir);

    float det = v1v2 * v1v2 - v1v1 * v2v2;
    float dist;

    if (std::abs(det) > FLT_MIN)
    {
        float inv_det = 1.f / det;

        float dpv1 = glm::dot(dp, rayDir);
        float dpv2 = glm::dot(dp, l2Dir);

        float t1 = inv_det * (v2v2 * dpv1 - v1v2 * dpv2);
        //t2 must be neg
        float t2 = inv_det * (v1v2 * dpv1 - v1v1 * dpv2);
        if (t2 > 0) return false;

        *hitDist = -t1;
        dist = glm::length(dp + l2Dir * (-t2) - rayDir * (-t1));
        //return glm::length(dp + dir * (-t2) - rayDir * (-t1));
    }
    else
    {
        glm::vec3 a = cross(dp, rayDir);
        dist = std::sqrt(glm::dot(a, a) / v1v1);
        //return std::sqrt(glm::dot(a, a) / v1v1);
    }
    return (dist < 0.05);
}

bool RayCast::circleCollision(glm::vec3 center, glm::vec3 dir, float r, float* hitDist)
{
    if (planeCollision(center, dir, hitDist)) {
        if (glm::distance(center, calculateRayPoint(*hitDist)) <= r) {

            return true;
        }
    }
    return false;
}


glm::vec3 RayCast::calculateRayPoint(float hitDist) {
    return rayOrigin + rayDir * hitDist;
}

glm::vec3 RayCast::calculateRay(float xpos, float ypos, glm::mat4 transfMatInv)
{
    // normalised device space
    glm::vec3 ray_nds = glm::vec3(xpos, ypos, 1);
    // clip space
    glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);
    glm::vec3 ray_wor = transfMatInv * ray_clip;
    return ray_wor = glm::normalize(ray_wor);
}

glm::vec3 RayCast::calculateRay(float xpos, float ypos, glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
    // normalised device space
    glm::vec3 ray_nds = glm::vec3(xpos, ypos, 1);
    // clip space
    glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);
    glm::vec4 ray_eye = glm::inverse(projectionMatrix) * ray_clip;
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1, 0);
    glm::vec3 ray_wor = glm::inverse(viewMatrix) * ray_eye;
    return ray_wor = glm::normalize(ray_wor);
}