//This Shader was made with help from:
//https://learnopengl.com/Lighting/Basic-Lighting
//https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
//https://learnopengl.com/Advanced-Lighting/Normal-Mapping

#version 330 core
layout(location = 0) in vec3 i_pos;
layout(location = 1) in vec3 i_norm;
layout(location = 2) in vec2 i_texcoord;
layout(location = 3) in vec3 i_tangent;
layout(location = 4) in vec3 i_bitangent;

out vec2 texCoord;
out vec3 FragNormal;
out vec3 FragPos;
out vec4 FragPosLightProjectionView;
out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;

uniform mat4 projectionView;
uniform mat4 model;
uniform mat4 lightProjectionViewMatrix;
uniform vec3 lightOrigin;
uniform vec3 camOrigin;

void main()
{
    texCoord = vec2(i_texcoord.x, i_texcoord.y);
    FragNormal = normalize(mat3(transpose(inverse(model))) * i_norm);
    FragPos = vec3(model * vec4(i_pos, 1.0));
    FragPosLightProjectionView = lightProjectionViewMatrix * vec4(FragPos, 1.0);

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * i_tangent);
    vec3 N = normalize(normalMatrix * i_norm);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N));    
    TangentLightPos = TBN * lightOrigin;
    TangentViewPos  = TBN * camOrigin;
    TangentFragPos  = TBN * FragPos;

    gl_Position = projectionView * model * vec4(i_pos, 1.0);
}
