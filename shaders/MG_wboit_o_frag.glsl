#version 460 core

in vec2 TexCoord;

uniform sampler2D uAccumTex;
uniform sampler2D uRevealTex;

out vec4 FragColor;

void main()
{
    vec4 accum = texture(uAccumTex, TexCoord);
    float revealage = texture(uRevealTex, TexCoord).r;

    // Avoid divide-by-zero
    float weight = max(accum.a, 1e-5);

    vec3 finalColor = accum.rgb / weight;
    float finalAlpha = 1.0 - revealage;

    FragColor = vec4(finalColor, finalAlpha);
}