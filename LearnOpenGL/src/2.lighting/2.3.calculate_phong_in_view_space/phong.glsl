//!vertex
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out vec3 LightPos;  // Convert lightPos to view space

uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;

uniform vec3 vLightPos; // for calculating

void main()
{
    gl_Position = vProjection * vView * vModel * vec4(aPos, 1.0);
    FragPos = vec3(vView * vModel * vec4(aPos, 1.0)); // in View Space

    // Use normal matrix to fix Normal
    Normal = mat3(transpose(inverse(vView * vModel))) * aNormal;

    LightPos = vec3(vView * vec4(vLightPos, 1.0));
}

//!fragment
#version 450 core
out vec4 FragColor;

in vec3 FragPos;  // in View Space
in vec3 Normal;   // in View Space
in vec3 LightPos; // in View Space

uniform float fAmbientStrength;
uniform float fSpecularStrength;
uniform vec3 fObjectColor;
uniform vec3 fLightColor;
//uniform vec3 fViewPos;  We don't need view pos here because in View Space the position of viewer is (0,0,0)
uniform float fShininess;

void main()
{
    vec3 ambientColor = fAmbientStrength * fLightColor;

    // Diffuse
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diffuse = max(dot(normal, lightDir), 0.0);
    vec3 diffuseColor = diffuse * fLightColor;

    // Specular
    vec3 viewDir = normalize(/*(0.0.0.)*/ - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specular = pow(max(dot(viewDir, reflectDir), 0.0), fShininess);
    vec3 specularColor = fSpecularStrength * specular * fLightColor;

    vec3 color = (ambientColor + diffuseColor + specularColor) * fObjectColor;
    FragColor = vec4(color, 1.0);
}