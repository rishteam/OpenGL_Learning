#version 450 core

in vec3 color;
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D tex0;

void main()
{
    FragColor = texture(tex0, texCoord);
//    FragColor = vec4(color, 1.f);
}