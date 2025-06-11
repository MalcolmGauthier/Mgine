#version 460 core

in vec2 TexCoord;

uniform sampler2D uAlbedoTexture;
uniform float uAlphaScale = 1.0; // optional override

layout (location = 0) out vec4 outAccum;     // RGB * alpha, alpha
layout (location = 1) out float outReveal;   // 1 - alpha

void main()
{
    vec4 color = texture(uAlbedoTexture, TexCoord);
    float alpha = clamp(color.a * uAlphaScale, 0.0, 1.0);

    // Premultiplied color
    outAccum = vec4(color.rgb * alpha, alpha);
    outReveal = (1.0 - alpha);
}