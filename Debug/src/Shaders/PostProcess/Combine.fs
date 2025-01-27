#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D sceneTexture;        // The base scene texture
uniform sampler2D bloomTexture;    // The blurred bloom texture
uniform float bloomStrength;    // Strength multiplier for bloom effect

void main() {
    // Fetch colors from scene and bloom textures
    vec3 sceneColor = texture(sceneTexture, TexCoords).rgb;
    vec3 bloomColor = texture(bloomTexture, TexCoords).rgb;

    // Combine the scene color and bloom effect
    vec3 finalColor = sceneColor + bloomStrength * bloomColor;

    // Output the final color with full alpha
    FragColor = vec4(finalColor, 1.0);
}
