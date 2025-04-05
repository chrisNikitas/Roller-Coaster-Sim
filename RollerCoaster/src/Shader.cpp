#include "Shader.h"
//This Class was made with help from:
//The Cherno opengl youtube series
Shader::Shader() {

}

Shader::Shader(const char* _vertexShaderFName, const char* _fragmentShaderFName)
{
    std::string vertexShaderSource = readShaderFromFile(_vertexShaderFName);
    std::string fragmentShaderSource = readShaderFromFile(_fragmentShaderFName);

    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource.c_str());
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource.c_str());

    this->id = glCreateProgram();
    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);
    glValidateProgram(id);
    glUseProgram(id);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::init(const char* _vertexShaderFName, const char* _fragmentShaderFName)
{
    std::string vertexShaderSource = readShaderFromFile(_vertexShaderFName);
    std::string fragmentShaderSource = readShaderFromFile(_fragmentShaderFName);

    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource.c_str());
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource.c_str());

    this->id = glCreateProgram();
    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);
    glValidateProgram(id);
    glUseProgram(id);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}


void Shader::setUniformMatrix4fv(std::string name, glm::mat4 value)
{
    int loc = glGetUniformLocation(id, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setUniformf(std::string name, float value)
{
    int loc = glGetUniformLocation(id, name.c_str());
    glUniform1f(loc, value);
}

void Shader::setUniform4fv(std::string name, glm::vec4 value)
{
    int loc = glGetUniformLocation(id, name.c_str());
    glUniform4fv(loc, 1, glm::value_ptr(value));
}

void Shader::setUniform3fv(std::string name, glm::vec3 value)
{
    int loc = glGetUniformLocation(id, name.c_str());
    glUniform3fv(loc, 1, glm::value_ptr(value));
}

void Shader::setInt(std::string name, int value)
{
    int loc = glGetUniformLocation(id, name.c_str());
    glUniform1i(loc, value);
}

void Shader::bind()
{
    glUseProgram(id);
}

void Shader::unbind()
{
    glUseProgram(0);
}


//helper methods
std::string Shader::readShaderFromFile(std::string filename)
{
    std::ifstream shaderFile(filename);
    if (!shaderFile)
    {
        std::cout << "Unable to open file " << filename << std::endl;
        exit(1);   // call system to stop
    }

    std::string shaderSourceString;

    std::string line;
    while (std::getline(shaderFile, line))
    {
        shaderSourceString.append(line + "\n");
    }
    shaderFile.close();
    return shaderSourceString;

}

unsigned int Shader::compileShader(unsigned int type, const char* shaderSource)
{
    unsigned int shader;
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    return shader;
}


