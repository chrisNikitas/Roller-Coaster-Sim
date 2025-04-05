#include "Track.h"
#include <glm/gtc/type_ptr.hpp>

Track::Track()
{

}
void Track::init() {
    t0 = glfwGetTime();



    tPointsVA.init();
    tPointsVB.init();
    BL.addAttribute<float>(3);
    tPointsVA.addBufferWithLayout(tPointsVB, BL);
    float tPoint[] = { 0 ,0 ,0 };
    tPointsVB.addData(tPoint, sizeof(tPoint));

    trackVB.init();
    trackVA.init();
    trackIB.init();
    BL.addAttribute<float>(3);
    BL.addAttribute<float>(3);
    trackVA.addBufferWithLayout(trackVB, BL);

    cart.init("C:\\Users\\chris\\OneDrive\\Documents\\UniStuff\\3rd Year Proj\\cart.obj");
    //cart.init("C:\\Users\\chris\\OneDrive\\Documents\\UniStuff\\cart.obj");
    
    points  =  {{ glm::vec3(-1, 0.5, 0), 0 },
                { glm::vec3(-0.375, 0.5, 0), 0 },
                { glm::vec3(0.375, 0.5, 0), 0 },
                { glm::vec3(1, 0.5, 0.001), 0 },
                /*{  glm::vec3(-1, 0, 0), 0 }*/ };

    for (unsigned int i = 0; i < points.size(); i++)
    {
        gizmos.push_back(Gizmo(points[i].pos));
    }

    createTrack();

    firstUp = glm::vec3(0, 1, 0);
    upPrev = firstUp;
    cartPosPrev = trackOrigin;
}
void Track::reset()
{
    trackL.clear();
    spline.clear();
    trackVertices.clear();
    trackIndices.clear();
    trackData.clear();
    trackConnections.clear();
    trackChainLink.clear();
    trackChainLinkConnections.clear();
    trackSupportVertices.clear();
    trackDistL.clear();
    upVertices.clear();
    rightVertices.clear();
    tangentVertices.clear();
}


void Track::getTrackVertices(std::pair<glm::vec3, glm::vec3> cur_track, glm::vec3 n, float circleRadius)
{
    getRailVertices(trackVertices, cur_track, n, circleRadius);
}
void Track::getChainLinkVertices(std::pair<glm::vec3, glm::vec3> cur_track, glm::vec3 n, float circleRadius)
{
    getRailVertices(trackChainLink, cur_track, n, circleRadius);
}
void Track::getRailVertices(std::vector<glm::vec3> &vec, std::pair<glm::vec3, glm::vec3> cur_track, glm::vec3 n, float circleRadius) 
{
    glm::vec3 p1 = cur_track.first;
    glm::vec3 p2 = cur_track.second;

    float directionAngle = getCurrentDirectionAngle(n);
    
    float inclineAngleXY = getCurrentInclineAngleWRTX(n);
    //float inclineAngleXY = atan(n.y / n.x);
    float inclineAngleZY = getCurrentInclineAngleWRTZ(n);
    //float inclineAngleZY = atan(n.y / -n.z);

    float angleInc = (2 * M_PI) / circleSegments;
    //for (float angle = 0; angle < 2 * M_PI; angle += angleInc) {
    for (float i = 0; i < circleSegments; i++) {
        float segmentAngle = i * angleInc;

        glm::vec3 vertex1 = getRailCrossSectionVertices(p1, segmentAngle, inclineAngleXY, inclineAngleZY, directionAngle, circleRadius);
        glm::vec3 vertex2 = getRailCrossSectionVertices(p2, segmentAngle, inclineAngleXY, inclineAngleZY, directionAngle, circleRadius);

        glm::vec3 normal1 = glm::normalize(vertex1 - p1);
        glm::vec3 normal2 = glm::normalize(vertex2 - p2);

        vec.push_back(vertex1);
        vec.push_back(normal1);
        vec.push_back(trackColour);

        vec.push_back(vertex2);
        vec.push_back(normal2);
        vec.push_back(trackColour);
    }
}
void Track::getSupportVertices(float ourT, glm::vec3 dir, float circleRadius)
{
    //create 2 circles
    //one at track height, other at ground level
    glm::vec3 p = calculateSplinePoint(ourT, points);

    float angleInc = (2 * M_PI) / circleSegments;
    //Create vertices
    for (float i = 0; i < circleSegments; i++)
    {
        float segmentAngle = i * angleInc;
        glm::vec3 vertex1;
        vertex1.x = p.x +
            (circleRadius)*sin(segmentAngle);
        vertex1.y = p.y;
        vertex1.z = p.z +
            (circleRadius)*cos(segmentAngle);


        glm::vec3 vertex2;
        vertex2.x = p.x +
            (circleRadius)*sin(segmentAngle);
        vertex2.y = 0;
        vertex2.z = p.z +
            (circleRadius)*cos(segmentAngle);

        glm::vec3 normal1 = glm::normalize(vertex1 - p);
        glm::vec3 normal2 = glm::normalize(vertex2 - glm::vec3(p.x, 0.1, p.z));
        trackSupportVertices.push_back(vertex1);
        trackSupportVertices.push_back(normal1);
        trackSupportVertices.push_back(supportsColour);

        trackSupportVertices.push_back(vertex2);
        trackSupportVertices.push_back(normal2);
        trackSupportVertices.push_back(supportsColour);

    }
}



void Track::createTrackConnectorVertices(float ourT, glm::vec3 dir, float orientationAngle, float circleRadius)
{
    createConnectorVertices(trackConnections, ourT, dir, orientationAngle, circleRadius);
}
void Track::createChainLinkConnectorVertices(float ourT, glm::vec3 dir, float orientationAngle, float circleRadius)
{
    createConnectorVertices(trackChainLinkConnections, ourT, dir, orientationAngle, circleRadius);
}
void Track::createConnectorVertices(std::vector<glm::vec3> &vec, float ourT, glm::vec3 dir, float orientationAngle, float circleRadius)
{
    std::pair<glm::vec3, glm::vec3> conns = getRailsFromSpline(ourT, dir, orientationAngle, distBtwTrackRails);
    float dirAngle = getCurrentDirectionAngle(dir);
    glm::vec3 conn1 = conns.first;
    glm::vec3 conn2 = conns.second;
    float angleInc = (2 * M_PI) / circleSegments;
    //Create vertices
    for (float i = 0; i < circleSegments; i++)
    {
        float segmentAngle = i * angleInc;
        glm::vec3 vertex1;
        vertex1.x = conn1.x +
            (circleRadius / 3) * sin(segmentAngle) * cos(-dirAngle);
        vertex1.y = conn1.y +
            (circleRadius / 3) * cos(segmentAngle);
        vertex1.z = conn1.z +
            (circleRadius / 3) * sin(segmentAngle) * sin(-dirAngle);
        glm::vec3 vertex2;
        vertex2.x = conn2.x +
            (circleRadius / 3) * sin(segmentAngle) * cos(-dirAngle);
        vertex2.y = conn2.y +
            (circleRadius / 3) * cos(segmentAngle);
        vertex2.z = conn2.z +
            (circleRadius / 3) * sin(segmentAngle) * sin(-dirAngle);

        glm::vec3 normal1 = glm::normalize(vertex1 - conn1);
        glm::vec3 normal2 = glm::normalize(vertex2 - conn2);
        vec.push_back(vertex1);
        vec.push_back(normal1);
        vec.push_back(trackColour);

        vec.push_back(vertex2);
        vec.push_back(normal2);
        vec.push_back(trackColour);

    }
}


int Track::triangulateChainLink(int startingIndex)
{
    int i = startingIndex;
    int startingI = i;
    if (chainLinkStart == -1) {return i;}
    for (i; i < startingI + (trackChainLink.size() / BL.getNumOfAttr()) - 2 * circleSegments; i += 2 * circleSegments)
        for (unsigned int j = 0; j < circleSegments; j++)
        {
            glm::uvec3 t1Triangle1Indices = { (i + (j * 2)),
                                    (i + (j + 1) * 2),
                                    (i + (j * 2) + (circleSegments * 2)) };
            glm::uvec3 t1Triangle2Indices = { (i + (j + 1) * 2),
                                              ((i + (j + 1) * 2) + (circleSegments * 2)),
                                               (i + (j * 2) + (circleSegments * 2)) };
            glm::uvec3 t2Triangle1Indices = { (i + (j * 2) + 1),
                                              ((i + (j + 1) * 2) + 1)         ,
                                              ((i + (j * 2) + (circleSegments * 2)) + 1) };
            glm::uvec3 t2Triangle2Indices = { ((i + (j + 1) * 2) + 1),
                                             (((i + (j + 1) * 2) + (circleSegments * 2)) + 1),
                                              ((i + (j * 2) + (circleSegments * 2)) + 1) };
            if (j == circleSegments - 1)
            {
                t1Triangle1Indices[1] -= circleSegments * 2;
                t1Triangle2Indices[0] -= circleSegments * 2;
                t1Triangle2Indices[1] -= circleSegments * 2;
                t2Triangle1Indices[1] -= circleSegments * 2;
                t2Triangle2Indices[0] -= circleSegments * 2;
                t2Triangle2Indices[1] -= circleSegments * 2;
            }
            //x,y,z for each triangle
            trackIndices.insert(trackIndices.end(), { t1Triangle1Indices[0], t1Triangle1Indices[1],t1Triangle1Indices[2] });
            trackIndices.insert(trackIndices.end(), { t1Triangle2Indices[0], t1Triangle2Indices[1],t1Triangle2Indices[2] });
            trackIndices.insert(trackIndices.end(), { t2Triangle1Indices[0], t2Triangle1Indices[1],t2Triangle1Indices[2] });
            trackIndices.insert(trackIndices.end(), { t2Triangle2Indices[0], t2Triangle2Indices[1],t2Triangle2Indices[2] });
        }
    startingI = i;

    i += 2 * circleSegments;

    //for (i; i < startingI + (trackChainLinkConnections.size() / BL.getNumOfAttr()); i += 2 * circleSegments)
    //    for (unsigned int j = 0; j < circleSegments; j++)
    //    {
    //        glm::uvec3 t1Triangle1Indices = { (i + (j * 2)),
    //                                           (i + (j * 2 + 2)),
    //                                           (i + (j * 2 + 1)) };
    //        glm::uvec3 t1Triangle2Indices = { (i + (j * 2 + 1)),
    //                                           (i + (j * 2 + 2)) ,
    //                                          ((i + (j * 2 + 3))) };
    //        if (j == circleSegments - 1)
    //        {
    //            t1Triangle1Indices[1] -= circleSegments * 2;
    //            t1Triangle2Indices[1] -= circleSegments * 2;
    //            t1Triangle2Indices[2] -= circleSegments * 2;
    //        }
    //        trackIndices.insert(trackIndices.end(), { t1Triangle1Indices[0], t1Triangle1Indices[1],t1Triangle1Indices[2] });
    //        trackIndices.insert(trackIndices.end(), { t1Triangle2Indices[0], t1Triangle2Indices[1],t1Triangle2Indices[2] });
    //    }
    //i += 2 * circleSegments;

    return i;
}
int Track::triangulateMainTrack(int startingIndex)
{
    //will return the end index of vector
    int i = startingIndex;
    int startingI = i;
    for (i; i < (trackVertices.size() / BL.getNumOfAttr()) - 2 * circleSegments; i += 2 * circleSegments)
        for (unsigned int j = 0; j < circleSegments; j++)
        {
            glm::uvec3 t1Triangle1Indices = { (i + (j * 2)),
                                               (i + (j + 1) * 2),
                                               (i + (j * 2) + (circleSegments * 2)) };
            glm::uvec3 t1Triangle2Indices = { (i + (j + 1) * 2),
                                              ((i + (j + 1) * 2) + (circleSegments * 2)),
                                               (i + (j * 2) + (circleSegments * 2))};
            glm::uvec3 t2Triangle1Indices = { (i + (j * 2) + 1),
                                              ((i + (j + 1) * 2) + 1)         ,
                                              ((i + (j * 2) + (circleSegments * 2)) + 1) };
            glm::uvec3 t2Triangle2Indices = { ((i + (j + 1) * 2) + 1),
                                             (((i + (j + 1) * 2) + (circleSegments * 2)) + 1),
                                              ((i + (j * 2) + (circleSegments * 2)) + 1)};
            if (j == circleSegments - 1)
            {
                t1Triangle1Indices[1] -= circleSegments * 2;
                t1Triangle2Indices[0] -= circleSegments * 2;
                t1Triangle2Indices[1] -= circleSegments * 2;
                t2Triangle1Indices[1] -= circleSegments * 2;
                t2Triangle2Indices[0] -= circleSegments * 2;
                t2Triangle2Indices[1] -= circleSegments * 2;
            }
            //x,y,z for each triangle
            trackIndices.insert(trackIndices.end(), { t1Triangle1Indices[0], t1Triangle1Indices[1],t1Triangle1Indices[2] });
            trackIndices.insert(trackIndices.end(), { t1Triangle2Indices[0], t1Triangle2Indices[1],t1Triangle2Indices[2] });
            trackIndices.insert(trackIndices.end(), { t2Triangle1Indices[0], t2Triangle1Indices[1],t2Triangle1Indices[2] });
            trackIndices.insert(trackIndices.end(), { t2Triangle2Indices[0], t2Triangle2Indices[1],t2Triangle2Indices[2] });
        }
    startingI = i;
    i += 2 * circleSegments;
    //track connectors
    for (i; i < startingI + trackConnections.size() / BL.getNumOfAttr(); i += 2 * circleSegments)
        for (unsigned int j = 0; j < circleSegments; j++)
        {
            glm::uvec3 t1Triangle1Indices = { (i + (j * 2)),
                                               (i + (j * 2 + 2)),
                                               (i + (j * 2 + 1)) };
            glm::uvec3 t1Triangle2Indices = { (i + (j * 2 + 1)),
                                               (i + (j * 2 + 2)) ,
                                              ((i + (j * 2 + 3))) };
            if (j == circleSegments - 1)
            {
                t1Triangle1Indices[1] -= circleSegments * 2;
                t1Triangle2Indices[1] -= circleSegments * 2;
                t1Triangle2Indices[2] -= circleSegments * 2;
            }
            trackIndices.insert(trackIndices.end(), { t1Triangle1Indices[0], t1Triangle1Indices[1],t1Triangle1Indices[2] });
            trackIndices.insert(trackIndices.end(), { t1Triangle2Indices[0], t1Triangle2Indices[1],t1Triangle2Indices[2] });
        }
    i += 2 * circleSegments;

    return i;
}
int Track::triangulateSupports(int startingIndex)
{
    int i = startingIndex;
    int startingI = i;
    for (i; i < startingI + trackSupportVertices.size() / BL.getNumOfAttr(); i += 2 * circleSegments)
        for (unsigned int j = 0; j < circleSegments; j++)
        {
            glm::uvec3 t1Triangle1Indices = { (i + (j * 2 + 1)),
                                               (i + (j * 2 + 2)),
                                                (i + (j * 2)) };
            glm::uvec3 t1Triangle2Indices = { ((i + (j * 2 + 3))),
                                               (i + (j * 2 + 2)) ,
                                              (i + (j * 2 + 1)) };
            if (j == circleSegments - 1)
            {
                t1Triangle1Indices[1] -= circleSegments * 2;
                t1Triangle2Indices[1] -= circleSegments * 2;
                t1Triangle2Indices[0] -= circleSegments * 2;
            }
            trackIndices.insert(trackIndices.end(), { t1Triangle1Indices[0], t1Triangle1Indices[1],t1Triangle1Indices[2] });
            trackIndices.insert(trackIndices.end(), { t1Triangle2Indices[0], t1Triangle2Indices[1],t1Triangle2Indices[2] });
        }
    //i += 2 * circleSegments;
    return i;
}

void Track::triangulateVertices()
{

    //track rails
    int index = 0;
    index = triangulateMainTrack(index);
    index = triangulateChainLink(index);
    //index = triangulateSupports(index);


}

void Track::createTrack()
{

    reset();

    glm::vec3 pLast;
    int curPoint = loopedTrack ? 1 : 2;
    glm::vec3 dir;
    float dist = 0;
    if (loopedTrack)
    {
        maxT = points.size() - 1;
    }
    else 
    {
        maxT = points.size() - 3;
    }

    float distanceToNextConnector = 0;
    float distanceToNextSupport= 0;
    points[0].distFromO = 0;
    float interval = (movingPoint) ? 0.1 : splineInterval;


    upPrev = glm::vec3(0, 1, 0);

    for (float t = 0; t <= ((loopedTrack) ? points.size() - 1: points.size() - 3); t += interval)
    {
        glm::vec3 p = calculateSplinePoint(t, points);
        spline.push_back(p);


        if (t == 0) {

            if (loopedTrack) {
                pLast = calculateSplinePoint(maxT - splineInterval, points);
            }
            else {
                trackDistL.push_back(0);
                pLast = p;
                points[t].distFromO = 0;
                continue;
            }
        }

        float deltaDist = glm::distance(p, pLast);
        //discard if dist is less than 0.02 and get dist with next point
        ////////if (deltaDist < 0.03) {
        ////////    continue;
        ////////}
        //std::cout << deltaDist << std::endl;
        dist += deltaDist;
        if (t!= 0)
            trackDistL.push_back(dist);

        //dir with pnext
        if (t >= maxT - interval) {
            continue;
        }
        glm::vec3 pNext = calculateSplinePoint(t + splineInterval, points);
        dir = glm::normalize(pNext - p);
        dirPrev = glm::normalize(p - pLast);

        //dir = p - pLast;
        //dir = (dir);

        //Behaves weird without this:
        //No first part vertices
        if (t == 0 + splineInterval) {
            float orientationAngle = getCurrentOrientation(t);
            std::pair<glm::vec3, glm::vec3> splinePoints = getRailsFromSpline(0, dir, orientationAngle, distBtwTrackRails);
            getTrackVertices(splinePoints, dir, trackRadius);
            pLast = p;
            points[t].distFromO = 0;

            //up vector stuff
            glm::vec3 temp = glm::normalize(glm::cross(upPrev, dirPrev));
            upPrev = glm::normalize(glm::cross(dirPrev, temp));
            continue;
        }
        else
        {

            //float dirAngleDegrees = glm::degrees(dirAngle);
            //float dirAngle = getCurrentDirectionAngle(dir);
            //std::cout << dirAngle << std::endl;


            //float incX = getCurrentInclineAngleWRTX(dir);
            //float incZ = getCurrentInclineAngleWRTZ(dir);
            //std::cout << "INC X:" << glm::degrees(incX) << std::endl;
            //std::cout << "INC Z:" << glm::degrees(incZ) << std::endl;
            //std::cout << std::endl;



            //parallel transport
            rightV = (glm::cross(dirPrev, dir));
            if (glm::length(rightV) == 0) {
                std::cout << "HERE" << std::endl;
            }
            else
            {
                rightV = glm::normalize(rightV);
                //std::cout << glm::to_string(rightV) << std::endl;

                float angle = acos(glm::dot(dirPrev, dir));
                //std::cout << glm::degrees(angle) << std::endl;

                glm::mat4 rot(1);
                rot = glm::rotate(rot, angle, rightV);

                glm::vec4 tUp = rot * glm::vec4(upPrev, 1);

                up = glm::normalize(glm::vec3(tUp));
                float angle2 = acos(glm::dot(up, glm::vec3(0, 1, 0)));
                //std::cout << glm::degrees(angle2) << std::endl;

                upVertices.push_back(p);
                upVertices.push_back(up / 2 + p);

                rightV = glm::normalize(glm::cross(up, dir));
                rightVertices.push_back(p);
                rightVertices.push_back(rightV / 2 + p);

                tangentVertices.push_back(p);
                tangentVertices.push_back(dir / 2 + p);
            }
            upPrev = up;
            //up = glm::normalize(glm::cross(glm::vec3(rightV.x, rightV.y, rightV.z), dir));
        }
        float orientationAngle = getCurrentOrientation(t);
        std::pair<glm::vec3, glm::vec3> splinePoints = getRailsFromSpline(t, dir, orientationAngle, distBtwTrackRails);

        //glm::vec3 rightV = splinePoints.first - splinePoints.second;
        //rightV = glm::normalize(rightV);
        //up = glm::cross(cartDir, rightV);
        //up = glm::normalize(up);

        //std::cout << "RV:" << glm::to_string(rightV) << std::endl;
        //std::cout <<"UP:" << glm::to_string(up) << std::endl;

        trackL.push_back(splinePoints);

        if (loopedTrack && (floorf(t) + 1) > curPoint) {
            int p = curPoint % points.size();
            points[p].distFromO = dist;
            points[p].dir = dir;
            gizmos[p].setDir(dir);
            curPoint++;
            //curPoint = curPoint % points.size();
        }
        else if (!loopedTrack && (floorf(t) + 1) >= curPoint) {
            points[curPoint].distFromO = dist;
            points[curPoint].dir = dir;
            gizmos[curPoint].setDir(dir);
            curPoint++;
        }
        pLast = p;

        if (!movingPoint) {
            getTrackVertices(splinePoints, dir, trackRadius);
            getConnectors(deltaDist, t, orientationAngle, dir, distanceToNextConnector);
            getSupports(deltaDist, t, orientationAngle, dir, distanceToNextSupport);
            if (t > chainLinkStart and t < chainLinkEnd)
            {
                //create chainlink
                std::pair<glm::vec3, glm::vec3> chainLinkPoints = getRailsFromSpline(t, dir, orientationAngle, distBtwTrackRails/3);
                getChainLinkVertices(chainLinkPoints, dir, chainLinkRadius);

            }

        }
        
    }

    if (!movingPoint)
    {
        trackData.insert(trackData.end(), trackVertices.begin(), trackVertices.end());
        trackData.insert(trackData.end(), trackConnections.begin(), trackConnections.end());
        trackData.insert(trackData.end(), trackChainLink.begin(), trackChainLink.end());
        trackData.insert(trackData.end(), trackChainLinkConnections.begin(), trackChainLinkConnections.end());
        //trackData.insert(trackData.end(), trackSupportVertices.begin(), trackSupportVertices.end());
        
        triangulateVertices();
        trackVB.addData(&trackData[0], trackData.size() * sizeof(glm::vec3));
        trackIB.addData(&trackIndices[0], trackIndices.size());
    }
    //points[curPoint % points.size()].distFromO = dist;
    trackDist = dist;
    if (loopedTrack)
    {
        points[points.size() - 1] = points[0];
        points[points.size() - 1].dir = dir;
        points[points.size() - 1].distFromO = trackDist;
        //maxT = points.size() - 1;
    }
    else {
        points[curPoint].distFromO = trackDist;
        //maxT = points.size() - 3;
    }
    //std::cout <<   std::endl;
    trackOrigin = calculateSplinePoint(0, points);
    //std::cout <<"END" << std::endl;
    //std::cout <<  std::endl;
}

//############################
//######### Helpers ##########
//############################

std::pair<glm::vec3, glm::vec3> Track::getRailsFromSpline(float ourT, glm::vec3 dir, float orientationAngle, float distBtwLines) {
    glm::vec3 point = calculateSplinePoint(ourT, points);
    float dirAngle = getCurrentDirectionAngle(dir);
    float inclineAngleXY = getCurrentInclineAngleWRTX(dir);
    float inclineAngleZY = getCurrentInclineAngleWRTX(dir);

    // 
    //if (dir.x < 0)
    //    dirAngle += M_PI;
    // 
    //glm::vec3 r1;
    //r1.x = point.x +
    //    (distBtwLines)*sin(dirAngle) * cos(orientationAngle);
    //r1.y = point.y +
    //    (distBtwLines)*sin(orientationAngle);
    //r1.z = point.z +
    //    (distBtwLines)*cos(dirAngle) * cos(orientationAngle);
    //glm::vec3 r2;
    //r2.x = point.x -
    //    (distBtwLines)*sin(dirAngle) * cos(orientationAngle);
    //r2.y = point.y -
    //    (distBtwLines)*sin(orientationAngle);
    //r2.z = point.z -
    //    (distBtwLines)*cos(dirAngle) * cos(orientationAngle);
  


    glm::vec3 here = distBtwLines / glm::length(rightV) * rightV;
    glm::vec3 r1;
    r1.x = point.x + (here.x);
    r1.y = point.y + (here.y);
    r1.z = point.z + (here.z);
    glm::vec3 r2;
    r2.x = point.x - (here.x);
    r2.y = point.y - (here.y);
    r2.z = point.z - (here.z);
    //if (inclineAngleXY > M_PI / 2 or inclineAngleZY > M_PI / 2)
    //{
    //    return std::pair< glm::vec3, glm::vec3 >(r2, r1);

    //}
    return std::pair< glm::vec3, glm::vec3 >(r1, r2);
}


glm::vec3 Track::getRailCrossSectionVertices(glm::vec3 centerPoint, float segmentAngle, float inclineAngleXY, float inclineAngleZY, float directionAngle, float circleRadius) {
    glm::vec3 vertex;
    vertex.x = centerPoint.x +
        circleRadius * sin(segmentAngle) * sin(directionAngle) +
        circleRadius * cos(segmentAngle) * sin(-inclineAngleXY) * pow(cos(directionAngle), 2);

    vertex.y = centerPoint.y +
        circleRadius * cos(segmentAngle) * cos(inclineAngleZY) * pow(sin(directionAngle), 2) +
        circleRadius * cos(segmentAngle) * cos(-inclineAngleXY) * pow(cos(directionAngle), 2);

    vertex.z = centerPoint.z +
        circleRadius * sin(segmentAngle) * cos(directionAngle) +
        circleRadius * cos(segmentAngle) * sin(inclineAngleZY) * pow(sin(directionAngle), 2);
    return vertex;
}

void Track::getSupports(float deltaDist, float t, float orientationAngle, glm::vec3 dir, float& distanceToNextPoint) {
    while (deltaDist > distanceToNextPoint) {
        //Calc connector position
        float relatee = deltaDist / distanceToNextPoint;
        float ourT = t - splineInterval + (splineInterval / relatee);
        float orient = getCurrentOrientation(ourT);
        if (abs(orient) < (M_PI / 2) or abs(orient) > (3 * M_PI / 2))
            getSupportVertices(ourT, dir, supportRadius);

        distanceToNextPoint += distBtwSupports;
    }
    distanceToNextPoint -= deltaDist;
}

void Track::getConnectors(float deltaDist, float t, float orientationAngle, glm::vec3 dir, float &distanceToNextPoint) {
    while (deltaDist > distanceToNextPoint) {
        //Calc connector position
        float relatee = deltaDist / distanceToNextPoint;
        float ourT = t - splineInterval + (splineInterval / relatee);
        
        createTrackConnectorVertices(ourT, dir, orientationAngle, trackRadius);

        distanceToNextPoint += distBtwConnections;
    }
    distanceToNextPoint -= deltaDist;
}


float Track::getCurrentOrientation(float t)
{
    int p1;
    int p2;
    if (!loopedTrack) {
        p1 = floor(t);
        p2 = p1 + 1;
    }
    else {
        p1 = floor(t) - 1;
        p2 = (p1 + 1);
    }
    float interp = t - floor(t);
    float sigmoid = 1 / (1 + exp(-2 * (6 * interp - 3)));
    int p1i = (p1 + 1) % points.size();
    int p2i = (p2 + 1) % points.size();

    //cos interp
    float cosInterp = (1 - cos(interp * M_PI)) / 2;

    if (points[p1i].orientation > (M_PI/2))
        return((points[p1i].orientation + (M_PI / 2)) * (1 - cosInterp) + (points[p2i].orientation + 2 * M_PI) * cosInterp);
    else if (points[p2i].orientation > (M_PI / 2))
        return(points[p1i].orientation * (1 - cosInterp) + (points[p2i].orientation + (M_PI / 2)) * cosInterp);
    
    else if (points[p1i].orientation < -(M_PI / 2))
        return((points[p1i].orientation - (M_PI / 2)) * (1 - cosInterp) + (points[p2i].orientation - 2 * M_PI) * cosInterp);
    else if (points[p2i].orientation < -(M_PI / 2))
        return(points[p1i].orientation * (1 - cosInterp) + (points[p2i].orientation - (M_PI / 2)) * cosInterp);
    
    
    else
        return(points[p1i].orientation * (1 - cosInterp) + points[p2i].orientation * cosInterp);

    //sigmoid interp
    //if (points[p1i].orientation >_PI / 2))
    //    return (points[p1i].orientation + (M_PI / 2)) + interp * ((points[p2i].orientation + 2 * M_PI) - (points[p1i].orientation + (M_PI / 2)));
    //else if (points[p2i].orientation > (M_PI / 2))
    //    return points[p1i].orientation + interp * ((points[p2i].orientation + (M_PI / 2)) - points[p1i].orientation);
    //else
    //    return points[p1i].orientation + sigmoid * (points[p2i].orientation - points[p1i].orientation);

    //linear interp
    //v0 + t * (v1 - v0);
    //return points[p1i].orientation + interp * (points[p2i].orientation - points[p1i].orientation);
}

float Track::getCurrentDirectionAngle(glm::vec3 dir)
{
    float inclineAngleXY = getCurrentInclineAngleWRTX(dir);
    float inclineAngleZY = getCurrentInclineAngleWRTX(dir);
    float result = atan(-dir.z / dir.x);
    if (dir.x < 0)
    {
        result = M_PI + result;
    }
    if (result < 0)
    {
        result = (3 * M_PI / 2) + ((M_PI / 2) + result);
    }
    
    return result;
}

float Track::getCurrentInclineAngleWRTX(glm::vec3 dir) 
{
    float result = atan(dir.y / dir.x);
    //if (dir.x < 0) {
    //    result = M_PI + result;
    //}
    return result;

}
    
float Track::getCurrentInclineAngleWRTZ(glm::vec3 dir) 
{
    float result = atan(dir.y / (-dir.z));
    //if ((-dir.z) < 0) {
    //    result = M_PI + result;
    //}
    return result;
}
    


//this function was made with help from https://www.youtube.com/watch?v=9_aJGUTePYo&t=397s
glm::vec3 Track::calculateSplinePoint(float t, std::vector<tPoint> splinePoints)
{
    int p0, p1, p2, p3;
    if (!loopedTrack)
    {
        p1 = int(t) + 1;
        p2 = p1 + 1;    
        p3 = p2 + 1;    
        p0 = p1 - 1;    
    }
    if (loopedTrack)
    {
        p1 = (int(t)) % points.size();
        p2 = (p1 + 1) % points.size();    
        p3 = (p2 + 1) % points.size();   
        p0 = p1 >= 1 ? p1 - 1 : points.size() - 1;
        if ((p3 == 0)) {
            p3 = 1;
        }
        if ((p1 == 0)) {
            p0 = points.size() - 2;
        }
    }

    t -= (int)t;

    float tt = t * t;
    float ttt = tt * t;

    float y1 = -ttt + 2 * tt - t;
    float y2 = 3.0f * ttt - 5.0f * tt + 2.0f;
    float y3 = -3.0f * ttt + 4.0f * tt + t;
    float y0 = ttt - tt;

    glm::vec3 result;
    result.x = 0.5 * (splinePoints[p0].pos.x * y1 + splinePoints[p1].pos.x * y2 + splinePoints[p2].pos.x * y3 + splinePoints[p3].pos.x * y0);
    result.y = 0.5 * (splinePoints[p0].pos.y * y1 + splinePoints[p1].pos.y * y2 + splinePoints[p2].pos.y * y3 + splinePoints[p3].pos.y * y0);
    result.z = 0.5 * (splinePoints[p0].pos.z * y1 + splinePoints[p1].pos.z * y2 + splinePoints[p2].pos.z * y3 + splinePoints[p3].pos.z * y0);
    return result;
}

//##################################
//######## Cart Mechanics ##########
//##################################
void Track::updateCart(double deltaTime, float cartT)
{ 
    if (atInitialPos)
    {
        cartPosPrev = trackOrigin;
        //cartDirPrev = glm::normalize(calculateSplinePoint(0.01, points) - trackOrigin);
        //upPrev = firstUp;
        //glm::vec3 temp = glm::normalize(glm::cross(cartDirPrev, upPrev));
        //upPrev = glm::normalize(glm::cross(cartDirPrev, temp));

        cartVel = 1;
        prevCartT = 0;
        atInitialPos = false;
        return;
    }
    if (docking)
    {
        float interp = cartT - floor(cartT);
        cartVel = preDockedCartVel + interp *(0.2 - preDockedCartVel);
    }
    
    if (onChailLink)
    {
        float interp = cartT - floor(cartT);
        cartVel = preChainLinkCartVel + interp * (0.5 - preChainLinkCartVel);
    }

    float tPrev = cartT - splineInterval;
    float tNext = cartT + splineInterval;
    if (tPrev < 0)
    {
        tPrev = maxT + tPrev;
    }
    if (tNext > maxT) {
        tNext = tNext - int(tNext);

    }

    glm::vec3 cartPosPrev = calculateSplinePoint(tPrev, points);
    glm::vec3 cartPosNext = calculateSplinePoint(tNext, points);

    float dist = glm::distance(cartPosPrev, cartPosNext);
    float y = cartPosNext.y - cartPosPrev.y;
    float incline = asin(y / dist);
    float a = -g * (y / dist);
    //float a = -g * sin(incline);
    float u = cartVel;
    double t = deltaTime;
    //float interp = t - floor(t);
    cartVel = (u + a * t);
    cartVel -= (cartFriction * cartVel) * deltaTime;
    
    if ( !docking && cartT > (maxT - 1)) 
    {
        preDockedCartVel = cartVel;
        docking = true;
    }
    if (!onChailLink and cartT > chainLinkStart -1  and cartT < chainLinkEnd)
    {
        preChainLinkCartVel= cartVel;
        onChailLink = true;
    }
    else if ((onChailLink and !(cartT > chainLinkStart and cartT < chainLinkEnd)))
    {
        onChailLink = false;
    }
    if (prevCartT > cartT) {
        atInitialPos = true;
        docking = false;
    }
    prevCartT = cartT;
    
    //cartVel = 1;
    //std::cout << cartVel  << std::endl;

}

float Track::calcCartT(float d)
{
    
    //float result = (loopedTrack) ? 0: 0;
    //int i = (loopedTrack) ? 0 : 1;
    ////float result = 0;
    ////int i = 0;
    //for (i; i <= ((loopedTrack) ? (points.size() - 1) : (points.size() - 2)); i++) {
    //    if (d == 0) { result = 0; break; }
    //    else if (points[i].distFromO >= d) {
    //        //get proportion of the way
    //        float p1 = points[i - 1].distFromO;
    //        float p2 = points[i].distFromO;
    //        result -= 1 - ((d - p1) / (p2 - p1));
    //        break;
    //    }
    //    else
    //    {
    //        result++;
    //    }
    //}
    //return result;
    //std::cout << result<<std::endl;

    for (lastI; lastI < trackDistL.size()-1; lastI++)
    {
        if (trackDistL[lastI] >= d )
        {
            float offset = (d - trackDistL[lastI-1]) / (trackDistL[lastI] - trackDistL[lastI-1]);
            //std::cout << (lastI + offset) * splineInterval << std::endl;
            float result = (lastI + offset) * splineInterval;
            cartPos = calculateSplinePoint(result, points);
            return result;
        }
    }
    cartPos = trackOrigin;

    return 0;
}


glm::vec3 Track::getCartPos()
{
    double tStart = glfwGetTime();
    camModel = glm::mat4(1);
    camModel = glm::translate(camModel, cartPos);
    //camModel = glm::rotate(camModel, float(glm::radians(-90.0f)), glm::vec3(0, 1, 0));
    //camModel = glm::translate(camModel, glm::vec3(10, 1, 0));
    camModel = glm::rotate(camModel, cartDirAngle, glm::vec3(0, 1, 0));
    camModel = glm::rotate(camModel, cartPitch, glm::normalize(glm::vec3(0, 0, 1)));
    camModel = glm::translate(camModel, glm::vec3(0, 0.065, 0));
    //camModel = glm::translate(camModel, glm::vec3(-abs(axis.x) / 15, -abs(axis.y) / 15,- abs(axis.z) / 15));
    camModel = glm::translate(camModel, glm::vec3(0, -0.065, 0));
    camModel = glm::rotate(camModel, -cartOrientationAngle, glm::vec3(1, 0, 0));
    //camModel = glm::translate(camModel, glm::vec3(abs(axis.x) / 15, abs(axis.y) / 15, abs(axis.z) / 15));
    camModel = glm::translate(camModel, glm::vec3(0, 0.065, 0));
    camModel = glm::translate(camModel, glm::vec3(0.4, -0.3, 0));
    //camModel = glm::translate(camModel, glm::vec3(0, 0.065, 0));
    //camModel = glm::scale(camModel, glm::vec3(0.08f));
    //camModel = glm::scale(camModel, glm::vec3(1, -1, 1));


    glm::vec3 pos = calculateSplinePoint(loopedTrack?  1: 0, points);
    tPos = camModel * glm::vec4(pos, 1);

    double tEnd = glfwGetTime();
    //std::cout << tEnd-tStart << std::endl;
    return tPos;
    //return cartPos - (glm::normalize(axis) * glm::vec3(0.25, 0.25, 0.25));
    //return tPos;
}
std::tuple<float, float, float> Track::getCartDir()
{
    float cartYaw = getCurrentDirectionAngle(cartDir);
    float cartPitch = asin(cartDir.y / glm::length(cartDir));
    return std::tuple<float, float, float>(cartYaw, cartPitch, cartOrientationAngle);

}

float Track::getCartVel()
{
    return cartVel;
}


//###########################
//#######   Draws   #########
//###########################
void Track::drawCart(Shader& shader)
{
    
    double deltaTime = glfwGetTime() - tPrev;
    tNow += deltaTime;

    float d = cartVel * deltaTime;
    curDist += d;
    //Normalise curDist to track size
    while (curDist > trackDist) { 
        curDist -= trackDist; 
        lastI = 1; 
        //reset cart to start at t = 0
        docking = false;
        atInitialPos = true;

    }
    while (curDist < 0) { curDist += trackDist; lastI = 1;}

    //get cartPos based on its velocity
    float cartT = calcCartT(curDist);
    //bool same = false;
    //if (cartPos == cartPosPrev)
    //{
    //    same = true;
    //    cartPosPrev = cartPosPrev2;
    //}
    //get cartAngles
    cartDir = glm::normalize((cartPos - cartPosPrev));
    cartDirAngle = getCurrentDirectionAngle(cartDir);
    cartOrientationAngle = getCurrentOrientation(cartT);
    
    float inclineAngleXY = getCurrentInclineAngleWRTX(cartDir);
    float inclineAngleZY = getCurrentInclineAngleWRTZ(cartDir);
    //std::cout << glm::degrees(inclineAngleXY) << std::endl;
    cartDir = glm::normalize(cartDir);
    cartPitch = asin(cartDir.y / glm::length(cartDir));
    //std::cout << glm::degrees(cartPitch) << std::endl;

    //Apply cart transformations
    cartModel = glm::mat4(1.0f);

    cartModel = glm::translate(cartModel, cartPos);
    cartModel = glm::rotate(cartModel, float(glm::radians(-90.0f)), glm::vec3(0, 1, 0));
    //cartModel = glm::translate(cartModel, glm::vec3(10, 1, 0));

    cartModel = glm::rotate(cartModel, cartDirAngle, glm::vec3(0, 1, 0));
    cartModel = glm::rotate(cartModel, cartPitch, glm::normalize(glm::vec3(1, 0, 0)));
    cartModel = glm::rotate(cartModel, -cartOrientationAngle, glm::vec3(0,0,-1)); 

    //////around 
    //float angle1 = acos(glm::dot(up, glm::vec3(0, 1, 0)));
    //glm::vec3 aor1 = glm::cross(up, glm::vec3(0, 1, 0));
    ////std::cout << glm::degrees(angle2) << std::endl;
    //cartModel = glm::rotate(cartModel, angle1, aor1);

    //float angle2 = acos(glm::dot(rightV, glm::vec3(0, 0, -1)));
    //glm::vec3 aor2 = glm::cross(rightV, glm::vec3(0, 0, -1));
    ////std::cout << glm::degrees(angle2) << std::endl;
    //cartModel = glm::rotate(cartModel, angle2, aor2);

    //float angle3 = acos(glm::dot(up, glm::vec3(1, 0, 0)));
    //glm::vec3 aor3 = glm::cross(up, glm::vec3(1, 0, 0));
    ////std::cout << glm::degrees(angle2) << std::endl;
    //cartModel = glm::rotate(cartModel, angle1, aor1);



    cartModel = glm::translate(cartModel, glm::vec3(0, 0.065, 0));
    cartModel = glm::scale(cartModel, glm::vec3(0.08f));
    cartModel = glm::scale(cartModel, glm::vec3(1, -1, 1));
    //axis = glm::vec3(&cartModel[0][0], &cartModel[0][0], &cartModel[0][0]);


    //cartModel = glm::translate(cartModel, glm::vec3(0, -1, 0));

    //cartModel = glm::translate(cartModel, glm::vec3(0, -0.065, 0));

    //cartModel = glm::translate(cartModel, cartPos + glm::vec3(0, 0, 0));
    //cartModel = glm::translate(cartModel, cartPos + glm::vec3(0, 0.065, 0));

    shader.setUniformMatrix4fv("model", cartModel);
    shader.setUniform4fv("colour", glm::vec4(0.5, 0.5, 0.5, 1));
    cart.Draw(shader);

    tPrev = tNow;
    //if (!same)
    //    cartPosPrev2 = cartPosPrev;
    cartPosPrev = cartPos;
    //cartDirPrev = cartDir;
    updateCart(deltaTime, cartT);
}

void Track::draw(Shader splinesShader, Shader trackShader, Shader gizmosShader,const Camera& camera, Object light)
{


    trackShader.bind();
    trackShader.setUniformMatrix4fv("projectionView", camera.getProjectionViewMatrix());

    trackShader.setUniform4fv("lightColor", light.getObjColor());
    trackShader.setUniform3fv("lightOrigin", light.getObjPos());
    trackShader.setUniform3fv("camOrigin", camera.getPos());

    //drawCart(trackShader);
    if(!movingPoint)
    {
        drawTrack(trackShader);
    }
    trackShader.unbind();

    drawTBN(gizmosShader, camera.getProjectionViewMatrix());

    splinesShader.bind();
    splinesShader.setUniformMatrix4fv("model", model);
    splinesShader.setUniformMatrix4fv("projectionView", camera.getProjectionViewMatrix());
    splinesShader.setUniform4fv("colour", glm::vec4(0, 0, 0, 1));
    glPointSize(10);
    splinesShader.setUniform3fv("pos", tPos);
    //glDrawArrays(GL_POINTS, 0, 1); 

    splinesShader.setUniformMatrix4fv("model", model);
    splinesShader.setUniformMatrix4fv("projectionView", camera.getProjectionViewMatrix());
    splinesShader.setUniform4fv("colour", glm::vec4(1, 1, 1, 1));

    if (movingPoint)
        drawTrackSplines(splinesShader);
    drawTPoints(splinesShader);
    splinesShader.unbind();
    drawGizmos(gizmosShader, camera);
}

void Track::drawTBN(Shader shader, glm::mat4 projectionViewMatrix) {
    Object upLine;
    int upLineAttributes[] = { 3 };
    upLine.addData(glm::vec3(0), upVertices, 1, upLineAttributes);
    upLine.setShape(GL_LINES);
    upLine.setShapeCount(2);

    Object rightLine;
    int rightLineAttributes[] = { 3 };
    rightLine.addData(glm::vec3(0), rightVertices, 1, rightLineAttributes);
    rightLine.setShape(GL_LINES);
    rightLine.setShapeCount(2);

    Object tangentLine;
    int tangentLineAttributes[] = { 3 };
    tangentLine.addData(glm::vec3(0), tangentVertices, 1, tangentLineAttributes);
    tangentLine.setShape(GL_LINES);
    tangentLine.setShapeCount(2);

    shader.bind();
    shader.setUniform3fv("d", glm::vec3(0, 0, 0));
    shader.setUniform3fv("coeff", glm::vec3(1, 1, 1));

    shader.setUniform4fv("colour", glm::vec4(1, 0, 0, 1));
    upLine.draw(shader, shader, projectionViewMatrix);

    shader.bind();
    shader.setUniform4fv("colour", glm::vec4(0, 1, 0, 1));
    rightLine.draw(shader, shader, projectionViewMatrix);

    shader.bind();
    shader.setUniform4fv("colour", glm::vec4(0, 0, 1, 1));
    tangentLine.draw(shader, shader, projectionViewMatrix);

    shader.unbind();
}

void Track::draw(Shader trackShader, glm::mat4 projectionViewMatrix)
{
    trackShader.bind();
    trackShader.setUniformMatrix4fv("projectionView", projectionViewMatrix);
    drawCart(trackShader);
    drawTrack(trackShader);
    trackShader.unbind();


}

void Track::drawLightCube(Shader primitivesShader)
{

    lightVAO.bind();
    lightVBO.bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    lightVAO.unbind();
    lightVBO.unbind();
}

void Track::drawGizmos(Shader gizmosShader, Camera camera) {
    for (unsigned int i = 0; i < selectedPointsIndices.size(); i++) {
        gizmos[selectedPointsIndices[i]].draw(gizmosShader, camera);
    }
}

void Track::drawTrack(Shader& shader)
{
    trackVA.bind();
    trackVB.bind();
    trackIB.bind();

    model = glm::mat4(1.0f);
    shader.setUniformMatrix4fv("model", model);

    glDrawElements(GL_TRIANGLES, trackIndices.size(), GL_UNSIGNED_INT, NULL);
    trackVA.unbind();
    trackVB.unbind();
    trackIB.unbind();
}



void Track::drawTPoints(Shader& shader)
{
    /* For Debugging 
    ////circles
    //for (int i = 0; i < trackVertices.size(); i++)
    //{
    //    shader.setUniform4fv("colour", glm::vec4(0, 1, 0, 1));
    //    shader.setUniform3fv("pos", trackVertices[i]);
    //    glDrawArrays(GL_POINTS, 0, 1);
    //}
    //connections
    //for (int i = 0; i < trackConnections.size(); i++)
    //{
    //    shader.setUniform4fv("colour", glm::vec4(0, 1, 0, 1));
    //    shader.setUniform3fv("pos", trackConnections[i]);
    //    glDrawArrays(GL_POINTS, 0, 1);
    //}    */

    //squares
    glPointSize(5);
    for (unsigned int i = 0; i < ((loopedTrack) ? points.size() - 1 : points.size()); i++) {
        if (points[i].selected)
        {
            shader.setUniform4fv("colour", glm::vec4(0, 1, 0, 1));
        }
        else
            shader.setUniform4fv("colour", glm::vec4(1, 0, 0, 1));

        shader.setUniform3fv("pos", points[i].pos);
        shader.setUniform3fv("model", points[i].pos);

        glDrawArrays(GL_POINTS, 0, 1);
    }
    glClear(GL_DEPTH_BUFFER_BIT);
}

void Track::drawTrackSplines(Shader& shader)
{
    //track
    glPointSize(3);
    shader.setUniform4fv("colour", glm::vec4(0, 0, 0, 1));
    for (auto i = trackL.begin(); i != trackL.end(); i++)
    {
        if (i == trackL.begin()) { continue; }
        std::pair<glm::vec3, glm::vec3> p = *(i);
        shader.setUniform3fv("pos", p.first);
        glDrawArrays(GL_POINTS, 0, 1);
        shader.setUniform3fv("pos", p.second);
        glDrawArrays(GL_POINTS, 0, 1);
    }
    //for (auto i = spline.begin(); i != spline.end(); i++)
    //{
    //    if (i == spline.begin()) { continue; }
    //    glm::vec3 p = *(i);

    //    shader.setUniform3fv("pos", p);
    //    glDrawArrays(GL_POINTS, 0, 1);
    //}
}

void Track::addChainLink()
{
    //get start and end point of chainlink
    if (selectedPointsIndices.size() < 2) { return; }
    unsigned int start = *std::min_element(selectedPointsIndices.begin(), selectedPointsIndices.end());
    unsigned int end = *std::max_element(selectedPointsIndices.begin(), selectedPointsIndices.end());
    //need to do -1 to find t
    chainLinkStart = start - 1;
    chainLinkEnd = end - 1;
    if (loopedTrack) {
        chainLinkStart++;
        chainLinkEnd++;
    }
    createTrack();
}

void Track::loopTrack()
{
    loopedTrack = !loopedTrack;
    if (loopedTrack) {
        points.push_back(points[0]);
        chainLinkStart++;
        chainLinkEnd++;
        //selectedPoint++;
    }
    else {
        //selectedPoint--;

        points.pop_back();
        points[1].distFromO = 0;

        points[points.size() - 1].distFromO = 0;
        points[points.size() - 1].dir = glm::vec3(0);

        chainLinkStart--;
        chainLinkEnd--;
    }
    createTrack();
}

void Track::restartCart()
{
    //reset cart to start at t = 0
    docking = false;
    atInitialPos = true;
    //cartT = 0;
    lastI = 1;
    curDist = 0;
}


//################################
//#######   Input Output #########
//################################
void Track::addTrackPoint(glm::vec3 point)
{
    tPoint p;
    p.pos = point;
    if (loopedTrack)
        points.pop_back();
    points.push_back(p);
    Gizmo gizmo(p.pos);
    gizmos.push_back(gizmo);
    if (loopedTrack)
        points.push_back(points[0]);
    createTrack();
}

void Track::deselectPoints() {
    for (unsigned int i = 0; i < selectedPointsIndices.size(); i++) {
        points[selectedPointsIndices[i]].selected = false;
    }
    selectedPoint = (-1);
    selectedPointsIndices.clear();
}

bool Track::selectTrackPoint(RayCast ray, bool multiPoint)
{
    bool collision;
    for (unsigned int i = 0; i < ((loopedTrack) ? points.size() - 1 : points.size()); i++) {
        glm::vec3 min = points[i].pos - glm::vec3(0.1, 0.1, 0.1);
        glm::vec3 max = points[i].pos + glm::vec3(0.1, 0.1, 0.1);
        collision = ray.boxCollision(min, max);
        if (!multiPoint) {
            deselectPoints();
        }
        if (collision) {
            selectedPoint = i;
            points[i].selected = true;
            selectedPointsIndices.push_back(i);
            return true;
        }
    }
    //Print
    return false;
}

bool Track::selectGizmo(RayCast rayCast)
{

    for (unsigned int i = 0; i < selectedPointsIndices.size(); i++) 
    {
        if (gizmos[selectedPointsIndices[i]].selectTool(rayCast))
        {
            movingPoint = true;
            //broadcast selectedPoint
            selectedPoint = selectedPointsIndices[i];
            createTrack();
            return true;
        }
    }
    
    return false;
}

void Track::removeLastPoint() {
    if (points.size() > 4)
    {
        deselectPoints();
        points.pop_back();
        gizmos.pop_back();
        createTrack();
    }
}
void Track::resetTrack()
{
    deselectPoints();
    if (!loopedTrack)
    {
        points = { { glm::vec3(-1, 0.5, 0), 0 },
            { glm::vec3(-0.375, 0.5, 0), 0 },
            { glm::vec3(0.375, 0.5, 0), 0 },
            { glm::vec3(1, 0.5, 0.2), 0 }};
    }
    else
    {
        points = { { glm::vec3(-1, 0.5, 0), 0 },
                { glm::vec3(-0.375, 0.5, 0), 0 },
                { glm::vec3(0.375, 0.5, 0), 0 },
                { glm::vec3(1, 0.5, 1), 0 },
                {  glm::vec3(-1, 0.5, 0), 0 } };
    }

    gizmos.clear();
    for (unsigned int i = 0; i < points.size(); i++)
    {
        gizmos.push_back(Gizmo(points[i].pos));
    }
    createTrack();

}
void Track::moveTrackPoint(RayCast rayCast) 
{
    glm::vec3 originalP = points[selectedPoint].pos;
    //move the selected point
    points[selectedPoint].pos = gizmos[selectedPoint].movePoint(rayCast);
    if (points[selectedPoint].pos.y < 0.5) {
        points[selectedPoint].pos.y = 0.5;
        gizmos[selectedPoint].setPos(points[selectedPoint].pos);
    }
    if (points[selectedPoint].pos.z > 20) {
        points[selectedPoint].pos.z = 20;
        gizmos[selectedPoint].setPos(points[selectedPoint].pos);
    }
    if (points[selectedPoint].pos.z < -20) {
        points[selectedPoint].pos.z = -20;
        gizmos[selectedPoint].setPos(points[selectedPoint].pos);
    }
    if (points[selectedPoint].pos.x > 20) {
        points[selectedPoint].pos.x = 20;
        gizmos[selectedPoint].setPos(points[selectedPoint].pos);
    }
    if (points[selectedPoint].pos.x < -20) {
        points[selectedPoint].pos.x = -20;
        gizmos[selectedPoint].setPos(points[selectedPoint].pos);
    }
    glm::vec3 displacement = originalP - points[selectedPoint].pos;

    for (unsigned int i = 0; i < selectedPointsIndices.size(); i++)
    {
        //move the rest of the points
        if (selectedPointsIndices[i] != selectedPoint)
        {
            //set Point position
            points[selectedPointsIndices[i]].pos -= displacement;
            //Limit minimum height
            if (points[selectedPointsIndices[i]].pos.y < 0.5) {
                points[selectedPointsIndices[i]].pos.y = 0.5;
            }
            //set Gizmo position
            gizmos[selectedPointsIndices[i]].setPos(points[selectedPointsIndices[i]].pos);
        }
    }

    createTrack();
}

void Track::rotateTrackPoint(RayCast rayCast)
{
    float rotation = gizmos[selectedPoint].rotatePoint(rayCast);
    points[selectedPoint].orientation += rotation;

    for (unsigned int i = 0; i < selectedPointsIndices.size(); i++)
    {
        if (selectedPointsIndices[i] != selectedPoint)
        {
            points[selectedPointsIndices[i]].orientation += rotation;
        }
    }
    createTrack();
}

void Track::changeGizmoMode(Mode mode)
{
    for (unsigned int i = 0; i < gizmos.size(); i++)
        gizmos[i].setGizmoMode(mode);
}



void Track::cursorMovement(GLFWwindow* window, RayCast rayCast) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (selectedPoint == -1) { return;}
        else
        {
            if (gizmos[selectedPoint].getIsActive())
            {
                if (gizmos[selectedPoint].mode == Mode::translate)
                    moveTrackPoint(rayCast);
                else if (gizmos[selectedPoint].mode == Mode::rotate)
                    rotateTrackPoint(rayCast);
            }
        }

    }
}

bool Track::leftMouseClick(RayCast rayCast, bool multiPoint)
{
    //order matters here
    //return (selectTrackPoint(rayCast, multiPoint) or selectGizmo(rayCast));
    return (selectGizmo(rayCast) or selectTrackPoint(rayCast, multiPoint));
}


bool Track::leftMouseRelease(RayCast rayCast)
{

    if (selectedPoint >= 0) {
        gizmos[selectedPoint].disableGizmo();
        movingPoint = false;
        previousCursorPos = glm::vec2(0, 0);
        createTrack();
    }
    return true;
}

void Track::changeOrientation(int inc)
{
    if (selectedPoint != -1)
    {
        points[selectedPoint].orientation += (inc)*M_PI / 180;
        createTrack();
    }
}
void Track::selectRight() {
    selectedPoint++;
    if (selectedPoint > points.size())
        selectedPoint = 0;
}
void Track::selectLeft() {
    selectedPoint--;
    if (selectedPoint < 0)
        selectedPoint = points.size() - 1;
}
void Track::moveCubeUp() {
    points[selectedPoint].pos.y += 0.1f;
    createTrack();

}
void Track::moveCubeDown() {
    points[selectedPoint].pos.y -= 0.1f;
    createTrack();

}
void Track::moveCubeLeft() {
    points[selectedPoint].pos.x -= 0.1f;
    createTrack();

}
void Track::moveCubeRight() {
    points[selectedPoint].pos.x += 0.1f;
    createTrack();
}
void Track::moveCubeIn() {
    points[selectedPoint].pos.z += 0.1;
    createTrack();
}
void Track::moveCubeOut() {
    points[selectedPoint].pos.z -= 0.1;
    createTrack();
}