#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    mat3 TBN;
} fs_in;

uniform sampler2D texture1; // diffuseMap
uniform sampler2D texture2; // normalMap
uniform sampler2D texture3; // depthMap

uniform float heightScale;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light[4];
uniform vec3 viewPos;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;
    
    vec2 P = viewDir.xy / viewDir.z * heightScale; 
    vec2 deltaTexCoords = P / numLayers;

    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = texture(texture3, currentTexCoords).r;

    while (currentLayerDepth < currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture(texture3, currentTexCoords).r;  
        currentLayerDepth += layerDepth;  
    }

    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(texture3, prevTexCoords).r - currentLayerDepth + layerDepth;

    float weight = abs(afterDepth) / (abs(afterDepth) + abs(beforeDepth));
    return mix(prevTexCoords, currentTexCoords, weight);
}

void main()
{           
    // offset texture coordinates with Parallax Mapping
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoords = fs_in.TexCoords;
    
    texCoords = ParallaxMapping(fs_in.TexCoords, viewDir);       
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;

    // obtain normal from normal map
    vec3 norm = texture(texture2, texCoords).rgb;
    norm = normalize(norm * 2.0 - 1.0);   
   


    vec3 result= vec3(0.0);


    for(int i = 0; i < 4; i++)
    {
        vec3 ambient =  light[i].ambient * texture(texture1, texCoords).rgb;
        // calculate diffuse lighting
        vec3 TanLightPos = fs_in.TBN * light[i].position;
        vec3 lightDir = normalize(TanLightPos - fs_in.TangentFragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light[i].diffuse * (diff * texture(texture1, texCoords).rgb);

        // calculate specular lighting
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light[i].specular * (spec * material.specular);
        
        float distance = length(light[i].position - fs_in.FragPos);
        float attenuation  =1.0 / (1.0 + 0.09*distance + 0.032*distance*distance);
        result+= (diffuse + specular+ ambient) * attenuation;
     }




    FragColor = vec4(result, 1.0);
}
