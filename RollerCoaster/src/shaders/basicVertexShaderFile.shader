#version 330 core
layout(location = 0) in vec3 i_pos;
layout(location = 1) in vec3 i_norm;
layout(location = 2) in vec3 i_col;

uniform mat4 model;
uniform mat4 projectionView;


void main()
{
    //gl_Position = transform * vec4(i_pos.x, i_pos.y, i_pos.z, 1.0);
    gl_Position = projectionView * model *vec4(i_pos, 1.0);
    //color = i_colour;

}
