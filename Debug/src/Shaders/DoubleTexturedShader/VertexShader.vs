#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aColor;

out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 transform;
uniform mat4 view;       
uniform mat4 projection; 

void main()
{
    gl_Position = projection * view * transform * vec4(aPos, 1.0);
    ourColor = aColor;
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}