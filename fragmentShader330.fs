#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D densityTex;

void main()
{
    float density = texture(densityTex, TexCoord).r;

    // mix(a, b, c) -> a mix of colors between [a, b] based on value interpolated from c
    vec3 color = mix(
        mix(
            vec3(0.0, 0.0, 0.2),
            vec3(0.0, 0.3, 1.0),
            smoothstep(0.0, 0.5, density)
        ),
        mix(
            vec3(0.0, 0.4, 0.0),
            vec3(0.0, 1.0, 0.0),
            smoothstep(0.0, 1.0, density)
        ),
        smoothstep(0.0, 1.0, density)
    );

    float alpha = smoothstep(0.0, 0.8, density);
    FragColor = vec4(color, alpha);
}