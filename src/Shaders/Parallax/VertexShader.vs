#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    mat3 TBN;
} vs_out;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 viewPos;

void main()
{   
    vs_out.FragPos = vec3(transform * vec4(aPos, 1.0));   
    vs_out.TexCoords = aTexCoords;   
    
    vec3 T = normalize(mat3(transform) * aTangent);
    vec3 B = normalize(mat3(transform) * aBitangent);
    vec3 N = normalize(mat3(transform) * aNormal);
    mat3 TBN = transpose(mat3(T, B, N));

    vs_out.TangentViewPos  = TBN * viewPos;
    vs_out.TangentFragPos  = TBN * vs_out.FragPos;
    vs_out.TBN = TBN;
    
    gl_Position = projection * view * transform * vec4(aPos, 1.0);
}
