#version 330 core
layout(location = 0) in vec3 pos;



uniform mat4 model;
uniform mat4 projectionView;

uniform vec3 d;
uniform vec3 coeff;


void main()
{
    gl_Position = projectionView * model * vec4(d + (coeff * pos), 1.0);

}