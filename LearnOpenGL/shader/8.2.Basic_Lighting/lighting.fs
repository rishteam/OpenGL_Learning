#version 450 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
//in vec3 LightPos;

uniform float ambientStrength;
uniform float specularStrength;

uniform vec3 LightPos;
uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(LightPos-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}