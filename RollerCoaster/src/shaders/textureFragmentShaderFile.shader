//This Shader was made with help from:
//https://learnopengl.com/Lighting/Basic-Lighting
//https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
//https://learnopengl.com/Advanced-Lighting/Normal-Mapping

#version 330 core
out vec4 FragColor;

in vec2 texCoord;
in vec3 FragNormal;
in vec3 FragPos;
in vec4 FragPosLightProjectionView;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

uniform sampler2D baseTexture;
uniform sampler2D normalMap;
uniform sampler2D shadowMap;

uniform vec4 lightColor;
uniform vec3 lightOrigin;
uniform vec3 camOrigin;


float ShadowCalculation(vec4 fragPosLightProjectionView, vec3 lightDir, vec3 normal)
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
    float bias = max(0.03 * (1.0 - dot(normal, lightDir)), 0.005);

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
    // obtain normalized normal
    vec3 normal = texture(normalMap, texCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    //light direction
    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);

    //diffuse 
    vec4 diff = max(dot(normal, lightDir), 0.0) * lightColor;

    //ambient 
    vec4 ambient = lightColor * 0.5;

    //specular 
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(0.0, dot(viewDir, reflectDir)), 8);
    vec4 specular = 1 * spec * lightColor;

    //shadow
    float shadow = ShadowCalculation(FragPosLightProjectionView, lightDir, normal);
    
    FragColor = (ambient + (1.0 - shadow) * (diff + specular)) * texture(baseTexture, texCoord);
    // ## no shadows
    //FragColor = (ambient + diff + specular) * texture(baseTexture, texCoord);//no shadows
    // ## no lighting
    //FragColor = 1.5 *texture(baseTexture, texCoord);

}
