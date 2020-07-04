#version 450 core
layout (location = 0) in vec3 aPos;

uniform vec4 posOffset;
uniform float rotate;

void main()
{
    float rotateDeg = rotate * 0.0174532925;
    mat2 rot = mat2(
        cos(rotateDeg), -sin(rotateDeg),
        sin(rotateDeg),  cos(rotateDeg)
    );
    vec2 pos = rot * aPos.xy;
    gl_Position = vec4(pos.x, pos.y, aPos.z, 1.0) + posOffset;
}