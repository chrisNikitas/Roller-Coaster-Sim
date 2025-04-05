#version 330 core
layout (location = 0) in vec2 i_pos;
layout(location = 1) in vec2 i_texCoords;

out vec2 TexCoords;

void main()
{
    //gl_Position = transform * vec4(i_pos.x, i_pos.y, i_pos.z, 1.0);
    //color = i_colour;
    TexCoords = i_texCoords;
    gl_Position = vec4(i_pos.x, i_pos.y, 0,0 , 1.0);

}
