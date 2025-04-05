#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <GL/glew.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


class Shader
{
    private:
        std::string readShaderFromFile(std::string filename);
        unsigned int compileShader(unsigned int type, const char* shaderSource);

    public:
        unsigned int id;

        Shader();

        Shader(const char* _vertexShaderFName, const char* _fragmentShaderFName);
        void init(const char* _vertexShaderFName, const char* _fragmentShaderFName);
        //~Shader();

        void setUniformMatrix4fv(std::string name, glm::mat4 value);
        void setUniformf(std::string name, float value);
        void setUniform4fv(std::string name, glm::vec4 value);
        void setUniform3fv(std::string name, glm::vec3 value);
        void setInt(std::string name, int value);

        void bind();
        void unbind();
};

