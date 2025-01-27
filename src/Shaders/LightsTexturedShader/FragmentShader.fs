#version 330 core
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



in vec3 ourColor;
in vec3 ourPos;
in vec2 TexCoord;


out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform sampler2D texture1;

void main()
{

    vec3 texColor = vec3(texture(texture1, TexCoord));
    vec3 ambient = light.ambient * texColor;
    vec3 norm = normalize(ourColor);
    vec3 lightDir = normalize(light.position - ourPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
    // specular
    vec3 viewDir = normalize(viewPos - ourPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);  
        
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);

}

