//This Shader was made with help from:
//https://learnopengl.com/Lighting/Basic-Lighting
//https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping

#version 330 core
layout(location = 0) in vec3 i_pos;
layout(location = 1) in vec3 i_norm;
layout(location = 2) in vec3 i_col;

uniform mat4 model;
uniform mat4 projectionView;
uniform mat4 lightProjectionViewMatrix;

out vec3 FragNormal;
out vec3 FragPos;
out vec3 Colour;
out vec4 FragPosLightProjectionView;

void main()
{
    FragPos = vec3(model * vec4(i_pos, 1.0));
    FragNormal = normalize(mat3(transpose(inverse(model))) * i_norm);
    Colour = i_col;
    FragPosLightProjectionView = lightProjectionViewMatrix * vec4(FragPos, 1.0);

    gl_Position = projectionView * vec4(FragPos, 1.0);
}