#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main()
{
    FragPos = vec3(vModel * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(vModel))) * aNormal;
    TexCoords = aTexCoords;
    gl_Position = vProjection * vView * vec4(FragPos, 1.0);
}