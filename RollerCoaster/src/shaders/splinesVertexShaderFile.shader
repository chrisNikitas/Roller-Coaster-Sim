#version 330 core

uniform mat4 model;
uniform mat4 projectionView;


uniform vec3 pos;

void main()
{
    vec4 pos = vec4(pos, 1.0);
    gl_Position = projectionView * model * pos;
}