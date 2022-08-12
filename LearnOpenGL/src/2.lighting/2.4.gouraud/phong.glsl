//!vertex
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;

void main()
{
    gl_Position = vProjection * vView * vModel * vec4(aPos, 1.0);
    FragPos = vec3(vModel * vec4(aPos, 1.0)); // in World Space

    // handle rotation and scale(in ratio)
    // The 0 in w axis is to cancel the translate in Model Matrix
    Normal = vec3(vModel * vec4(aNormal, 0));
}

//!fragment
#version 450 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform float fAmbientStrength;
uniform float fSpecularStrength;
uniform vec3 fObjectColor;
uniform vec3 fLightColor;
uniform vec3 fLightPos;
uniform vec3 fViewPos;
uniform float fShininess;

void main()
{
    vec3 ambientColor = fAmbientStrength * fLightColor;

    // Diffuse
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(fLightPos - FragPos);
    float diffuse = max(dot(normal, lightDir), 0.0);
    vec3 diffuseColor = diffuse * fLightColor;

    // Specular
    vec3 viewDir = normalize(fViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specular = pow(max(dot(viewDir, reflectDir), 0.0), fShininess);
    vec3 specularColor = fSpecularStrength * specular * fLightColor;

    vec3 color = (ambientColor + diffuseColor + specularColor) * fObjectColor;
    FragColor = vec4(color, 1.0);
}