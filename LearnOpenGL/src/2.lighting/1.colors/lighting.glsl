//!vertex
#version 450 core
layout (location = 0) in vec3 aPos;

uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;

void main()
{
    gl_Position = vProjection * vView * vModel * vec4(aPos, 1.0);
}

//!fragment
#version 450 core
out vec4 FragColor;

uniform vec3 fObjectColor;
uniform vec3 fLightColor;

void main()
{
    FragColor = vec4(fObjectColor * fLightColor, 1.0);
}