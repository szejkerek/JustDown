#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture; // Texture input
uniform float threshold;         // Brightness threshold

void main() {
    // Fetch the color from the texture
    vec3 color = texture(screenTexture, TexCoords).rgb;

    // Calculate brightness using luminance coefficients
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));

    // Output color if brightness exceeds the threshold, else output black
    if (brightness > threshold) {
        FragColor = vec4(color, 1.0);
    } else {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
