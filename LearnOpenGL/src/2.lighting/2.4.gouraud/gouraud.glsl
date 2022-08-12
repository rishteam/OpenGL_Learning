//!vertex
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Color;

uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;
//
uniform vec3 vLightColor;
// for ambient
uniform float vAmbientStrength;
// for diffuse
uniform vec3 vLightPos;
// for specular
uniform vec3 vViewPos;
uniform float vShininess;
uniform float vSpecularStrength;


void main()
{
    gl_Position = vProjection * vView * vModel * vec4(aPos, 1.0);
    vec3 pos = vec3(vModel * vec4(aPos, 1.0)); // in World Space

    // handle rotation and scale(in ratio)
    // The 0 in w axis is to cancel the translate in Model Matrix
    vec3 normal = normalize(vec3(vModel * vec4(aNormal, 0)));

    // ambient
    vec3 ambientColor = vAmbientStrength * vLightColor;

    // diffuse
    vec3 lightDir = normalize(vLightPos - pos);
    float diffuse = max(dot(lightDir, normal), 0.0);
    vec3 diffuseColor = diffuse * vLightColor;

    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 viewDir = normalize(vViewPos - pos);
    float specular = max(dot(reflectDir, viewDir), 0.0);
    specular = pow(specular, vShininess);
    vec3 specularColor = vSpecularStrength * specular * vLightColor;

    Color = ambientColor + diffuseColor + specularColor;
}

//!fragment
#version 450 core
out vec4 FragColor;

in vec3 Color;

uniform vec3 fObjectColor;

void main()
{
    FragColor = vec4(Color * fObjectColor, 1.0);
}