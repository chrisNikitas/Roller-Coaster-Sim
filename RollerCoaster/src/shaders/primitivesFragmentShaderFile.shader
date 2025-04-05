//out vec4 FragColor;
//  
//in vec3 ourColor;
//in vec2 texCoord;
//void main()
//{
//    fragColor = texture(ourTexture, TexCoord);
//    fragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
//} ;

#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);

    //FragColor = texture(texture_diffuse1, texCoord);
}