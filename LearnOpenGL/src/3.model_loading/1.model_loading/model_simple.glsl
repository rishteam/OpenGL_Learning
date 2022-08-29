//!vertex
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;

void main()
{
    gl_Position = vProjection * vView * vModel * vec4(aPos, 1.0);
    FragPos = vec3(vModel * vec4(aPos, 1.0)); // in World Space
    Normal = mat3(transpose(inverse(vModel))) * aNormal;  // fix Normal
    TexCoords = aTexCoords;
}

//!fragment
#version 450 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{
    FragColor = texture(texture_diffuse1, TexCoords);
}