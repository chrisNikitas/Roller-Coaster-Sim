//taken from https://learnopengl.com/Model-Loading/Mesh
#pragma once

#include <GL/glew.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "IndexBuffer.h"

#include <string>
#include <vector>
using namespace std;

#define MAX_BONE_INFLUENCE 4
struct Vertex {
    //position
    glm::vec3 Position;
    //normal
    glm::vec3 Normal;
    //colour
    glm::vec3 Colour;

};

struct Texture {
    unsigned int id;
    string type;
    string path;
};
struct MatColour {
    unsigned int id;
    glm::vec4 colour;
};
class Mesh
{
private:
    VertexBuffer VBO;
    IndexBuffer IBO;
    void setupMesh();
public:
    //mesh data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;

    VertexArray VAO;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices);
    void Draw(Shader& shader);
};

