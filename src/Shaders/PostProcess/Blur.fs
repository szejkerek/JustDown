#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D inputTexture; // Renamed for consistency with your C++ code
uniform bool horizontal;

const float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main() {
    vec2 tex_offset = 1.0 / textureSize(inputTexture, 0); // Size of a single texel
    vec3 color = texture(inputTexture, TexCoords).rgb * weights[0];

    for (int i = 1; i < 5; ++i) {
        vec2 offset = horizontal
            ? vec2(tex_offset.x * i, 0.0) 
            : vec2(0.0, tex_offset.y * i);
            
        color += texture(inputTexture, TexCoords + offset).rgb * weights[i];
        color += texture(inputTexture, TexCoords - offset).rgb * weights[i];
    }

    FragColor = vec4(color, 1.0);
}
