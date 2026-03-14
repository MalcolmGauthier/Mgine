#version 460 core

out vec4 oColor;

uniform sampler2D uAccumTex;
uniform sampler2D uRevealTex;

void main()
{
    ivec2 coords = ivec2(gl_FragCoord.xy);

    vec4  accum  = texelFetch(uAccumTex,  coords, 0);
    float reveal = texelFetch(uRevealTex, coords, 0).r;

    // Discard fully opaque or empty pixels to avoid touching the opaque scene
    if (reveal == 1.0) discard;

    // Weighted average color, blended over the opaque background by (1 - reveal)
    vec3 avgColor = accum.rgb / max(accum.a, 1e-5);
    oColor = vec4(avgColor, 1.0 - reveal);
}