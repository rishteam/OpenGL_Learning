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

struct Material
{
// vec3 ambient; // in most siuation ambient = diffuse
    sampler2D diffuse;
    sampler2D specular;
//    sampler2D emission;
    float shininess;
};
uniform Material material;

struct Light
{
    vec3 pos;
    vec3 dir;
    float cutOff;
    float outerCutoff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
uniform Light light;

uniform vec3 fViewPos;

void main()
{
    float distance = length(light.pos - FragPos);
    float attenuation = 1.0 / (light.constant + distance * light.linear + distance * distance * light.quadratic);

    vec3 ambient = vec3(texture(material.diffuse, TexCoords));
    vec3 ambientColor = light.ambient * ambient;

    // Diffuse
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(light.pos - FragPos);
    float diffuseVal = max(dot(normal, lightDir), 0.0);
    vec3 diffuseColor = light.diffuse * (diffuseVal * ambient/*diffuse=ambient*/);

    // Specular
    vec3 viewDir = normalize(fViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularVal = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specularColor = light.specular * (specularVal *  texture(material.specular, TexCoords).rgb);

    // Emission
//    vec3 emission = texture(material.emission, TexCoords).rgb;

    // spotlight
    float theta = dot(lightDir, normalize(-light.dir));
    float epsilon = light.cutOff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

    diffuseColor *= intensity;
    specularColor *= intensity;

    vec3 color = ambientColor + diffuseColor + specularColor /*+ emission*/;
    color *= attenuation;
    FragColor = vec4(color, 1.0);
}