#version 330 core
in vec3 ourColor;
in vec3 ourPos;

out vec4 FragColor;

uniform vec3 viewPos;

void main()
{

    FragColor = vec4(ourColor, 1.0);

}

