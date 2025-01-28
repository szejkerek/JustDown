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

out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;
uniform Light light[4];

void main()
{

   

    vec3 norm = normalize(ourColor);
    vec3 result = vec3(0.0);


    for(int i = 0; i < 4; i++)
    {
        vec3 ambient = light[i].ambient* material.ambient;
        vec3 lightDir = normalize(light[i].position - ourPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light[i].diffuse * (diff * material.ambient);
        
        // specular
        vec3 viewDir = normalize(viewPos - ourPos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light[i].specular * (spec * material.specular); 
        
        float distance = length(light[i].position - ourPos);
        float attenuation  =1.0 / (1.0 + 0.09*distance + 0.032*distance*distance);
        result+= (ambient+ diffuse + specular) * attenuation;
     }
    FragColor = vec4(result, 1.0);

}

