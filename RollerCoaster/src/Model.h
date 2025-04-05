//taken from https://learnopengl.com/Model-Loading/Model
#pragma once

#include <GL/glew.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext.hpp"

//#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "stb_image/stb_image.h"

#include "Mesh.h"
#include "Shader.h"


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;
class Model
{
public:
    vector<Texture> textures_loaded;
    Model();
    void init(std::string path);
    void Draw(Shader& shader);
private:
    // model data
    vector<Mesh> meshes;
    string directory;

    void loadModel(string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);
};

