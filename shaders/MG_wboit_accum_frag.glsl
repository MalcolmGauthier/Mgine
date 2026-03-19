#version 460 core

layout(location = 0) out vec4 oAccum;
layout(location = 1) out float oReveal;

in vec2 vTexCoord;

uniform sampler2D uAlbedo;
uniform float     uAlpha;   // per-object alpha, or bake into texture alpha

void main()
{
    vec4 color = texture(uAlbedo, vTexCoord);
    color.a   *= uAlpha;

    // Weight function from the original McGuire & Bavoil paper.
    // Balances near/far contributions; tweak the constants if needed.
    float weight = clamp(
        pow(min(1.0, color.a * 10.0) + 0.01, 3.0) * 1e8 *
        pow(1.0 - gl_FragCoord.z * 0.9, 3.0),
        1e-2, 3e3
    );

    oAccum  = vec4(color.rgb * color.a, color.a) * weight;
    oReveal = color.a;
}
