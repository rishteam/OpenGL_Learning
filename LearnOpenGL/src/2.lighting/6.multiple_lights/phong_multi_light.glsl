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

uniform vec3 fViewPos;

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material material;

struct DirectionalLight
{
    vec3 dir;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirectionalLight dirLight;
vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.dir);
    vec3 diffuseTex = vec3(texture(material.diffuse, TexCoords));
    // ambient
    vec3 ambientColor = light.ambient * diffuseTex;
    // diffuse
    float diffuseVal = max(dot(lightDir, normal), 0.0);
    vec3 diffuseColor = light.diffuse * diffuseVal * diffuseTex;
    // specular
    vec3 specularTex = vec3(texture(material.specular, TexCoords));
    float specularVal = pow(max(dot(reflect(-lightDir, normal), viewDir), 0.0), material.shininess);
    vec3 specularColor = light.specular * specularVal * specularTex;
    //
    return ambientColor + diffuseColor + specularColor;
}

struct PointLight {
    vec3 pos;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define MAX_POINT_LIGHT 4
uniform PointLight pointLights[MAX_POINT_LIGHT];
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    float distance = length(light.pos - fragPos);
    float attenuation = 1.0 / (light.constant + distance * light.linear + distance * distance * light.quadratic);

    vec3 ambient = vec3(texture(material.diffuse, TexCoords));
    vec3 ambientColor = light.ambient * ambient;
    ambientColor *= attenuation;

    // Diffuse
    vec3 lightDir = normalize(light.pos - fragPos);
    float diffuseVal = max(dot(normal, lightDir), 0.0);
    vec3 diffuseColor = light.diffuse * (diffuseVal * ambient/*diffuse=ambient*/);
    diffuseColor *= attenuation;

    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularVal = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specularColor = light.specular * (specularVal *  texture(material.specular, TexCoords).rgb);
    specularColor *= attenuation;

    return ambientColor + diffuseColor + specularColor;
}

struct SpotLight {
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
#define MAX_SPOT_LIGHT 1
uniform SpotLight spotLights[MAX_SPOT_LIGHT];

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    float distance = length(light.pos - fragPos);
    float attenuation = 1.0 / (light.constant + distance * light.linear + distance * distance * light.quadratic);

    vec3 ambient = vec3(texture(material.diffuse, TexCoords));
    vec3 ambientColor = light.ambient * ambient;

    // Diffuse
    vec3 lightDir = normalize(light.pos - fragPos);
    float diffuseVal = max(dot(normal, lightDir), 0.0);
    vec3 diffuseColor = light.diffuse * (diffuseVal * ambient/*diffuse=ambient*/);

    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularVal = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specularColor = light.specular * (specularVal *  texture(material.specular, TexCoords).rgb);

    // Emission
    //    vec3 emission = texture(material.emission, TexCoords).rgb;

    // spotlight
    float theta = dot(lightDir, normalize(-light.dir));
    float epsilon = light.cutOff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

    ambientColor *= intensity * attenuation;
    diffuseColor *= intensity * attenuation;
    specularColor *= intensity * attenuation;

    return ambientColor + diffuseColor + specularColor /*+ emission*/;
}

void main()
{
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(fViewPos - FragPos);
    vec3 color = CalcDirectionalLight(dirLight, normal, viewDir);

    for(int i = 0; i < MAX_POINT_LIGHT; i++)
        color += CalcPointLight(pointLights[i], normal, FragPos, viewDir);

    for(int i = 0; i < MAX_SPOT_LIGHT; i++)
        color += CalcSpotLight(spotLights[i], normal, FragPos, viewDir);

    FragColor = vec4(color, 1.0);
}