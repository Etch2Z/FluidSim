#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D densityTex;

void main()
{
    float density = texture(densityTex, TexCoord).r;
    
    // Enhanced color mapping with multiple gradient stops
    vec3 color = mix(
        mix(
            vec3(0.0, 0.2, 0.8),  // Deep blue for low density
            vec3(0.0, 0.8, 1.0),  // Cyan for medium-low density
            smoothstep(0.0, 0.4, density)
        ),
        mix(
            vec3(0.0, 1.0, 0.0),  // Bright green for medium-high density
            vec3(1.0, 1.0, 0.0),  // Yellow for high density
            smoothstep(0.4, 1.0, density)
        ),
        smoothstep(0.0, 1.0, density)
    );

    // Add brightness boost for higher densities
    color += vec3(1.0, 0.8, 0.5) * pow(density, 3.0) * 0.5;

    // Soft alpha falloff at edges
    float alpha = smoothstep(0.01, 0.2, density);

    FragColor = vec4(color, alpha);
}