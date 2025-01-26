
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aColor;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightColor;

out vec3 ourColor;
out vec3 ourPos;
void main()
{   
    ourPos = vec3(transform * vec4(aPos, 1.0));
    ourColor = lightColor;  
    
    gl_Position = projection * view * vec4(ourPos, 1.0);
}

