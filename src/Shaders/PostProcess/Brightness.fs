#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;
uniform float threshold;

void main() {
    vec3 color = texture(screenTexture, TexCoords).rgb;

    // Calculate brightness using luminance (weighted average)
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));

    // Keep only the bright parts
    vec3 result = brightness > threshold ? color : vec3(0.0);
    FragColor = vec4(result, 1.0);
}
