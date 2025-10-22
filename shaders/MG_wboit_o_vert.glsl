#version 460 core

out vec2 vTexCoord;

const vec2 verts[4] = vec2[]
(
    vec2(-1.0, -1.0),
    vec2( 1.0, -1.0),
    vec2(-1.0,  1.0),
    vec2( 1.0,  1.0)
);

void main()
{
    gl_Position = vec4(verts[gl_VertexID], 0.0, 1.0);
    vTexCoord = verts[gl_VertexID] * 0.5 + 0.5;
}