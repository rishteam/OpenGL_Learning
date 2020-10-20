#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;

out vec3 FragPos;
out vec3 Normal;

void main()
{
    gl_Position = vProjection * vView * vModel * vec4(aPos, 1.0);
    FragPos = vec3(vModel * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(vModel))) * aNormal;
}