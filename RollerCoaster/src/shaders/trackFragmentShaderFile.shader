//This Shader was made with help from:
//https://learnopengl.com/Lighting/Basic-Lighting
//https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping

#version 330 core
out vec4 FragColor;

in vec3 FragNormal;
in vec3 FragPos;
in vec3 Colour;
in vec4 FragPosLightProjectionView;


//uniform vec4 colour;
uniform vec4 lightColor;
uniform vec3 lightOrigin;
uniform vec3 camOrigin;
uniform sampler2D shadowMap;

float ShadowCalculation(vec4 fragPosLightProjectionView, vec3 lightDir)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightProjectionView.xyz / fragPosLightProjectionView.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(0.03 * (1.0 - dot(FragNormal, lightDir)), 0.005);
    //float bias = 0.005;

    //float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    if (projCoords.z > 1.0)
        shadow = 0.0;
    return shadow;
}


void main()
{
    //vec3 FragNormal = normalize(FragNormal);

    //light direction
    vec3 lightDir = normalize(lightOrigin - FragPos);
    
    //diffuse component
    vec4 diff = max(dot(FragNormal, lightDir), 0.0) * lightColor;
    
    //ambient component
    vec4 ambient = lightColor * 0.5;
    //vec3 camOrigin;
    //camOrigin.xyz = vec3(0.0, 1.0, 1.0)
    //specular component
    vec3 viewDir = normalize(camOrigin - FragPos);
    vec3 reflectDir = reflect(-lightDir, FragNormal);
    //float spec = pow(dot(viewDir, reflectDir), 256);
    float spec = pow(max(0.0, dot(viewDir, reflectDir)), 32);
    vec4 specular = 1 * spec * lightColor;

    //Shadows
    //float shadow = 0;
    float shadow = ShadowCalculation(FragPosLightProjectionView, lightDir);
   
    //FragColor = texture(texture_diffuse1, texCoord);
    FragColor = (ambient + (1.0 - shadow) * (diff + specular)) * (vec4(Colour, 1.0));
    // ## no shadows
    //FragColor = (ambient + diff + specular) * (vec4(Colour, 1.0));
    // ## no lighting
    //FragColor = (vec4(Colour, 1.0));
}