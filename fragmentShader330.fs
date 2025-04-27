#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D densityTex;

void main()
{
    float density = texture(densityTex, TexCoord).r;
    FragColor = vec4(0.0, density, 0.0, 1.0);
}
