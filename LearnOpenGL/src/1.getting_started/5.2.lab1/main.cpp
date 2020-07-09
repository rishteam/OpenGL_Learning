#include <iostream>
#include <cmath>
#include <functional>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <glad/glad.h>
#include <SFML/OpenGL.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui-SFML.h>

#define WIRE_MODE 0

void init()
{
    setvbuf(stdin, nullptr, _IONBF, 0);
    setvbuf(stdout, nullptr, _IONBF, 0);
}
// Vertex Shader
const char *vertexShaderSource = R"glsl(
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aVertColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 VertColor;
out vec2 TexCoord;

uniform mat4 vTransform;

void main()
{
    gl_Position = vTransform * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    VertColor = aVertColor;
}
)glsl";
// Fragment Shader
const char *fragmentShaderSoucre = R"glsl(
#version 450 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 VertColor;

uniform sampler2D tex1;
uniform sampler2D tex2;

uniform vec4 fColor;
uniform float fMix;

void main()
{
    vec4 resultColor = mix(texture(tex1, TexCoord), texture(tex2, TexCoord), fMix) * fColor;
    // vec4 resultColor = texture(tex1, TexCoord) * fColor;
    FragColor = resultColor;
}
)glsl";

uint32_t LoadShader(GLenum type, const char* src)
{
    uint32_t shader;
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    char log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shader, 512, nullptr, log);
        printf("Error Shader %s compile error\n%s\n", type == GL_VERTEX_SHADER ? "Vertex" : "Fragment", log);
        return 0;
    }
    return shader;
}

uint32_t LinkShaderProgram(uint32_t vertex, uint32_t fragment)
{
    uint32_t program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    //
    int success;
    char log[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(program, 512, nullptr, log);
        printf("Error Shader Linking error\n%s\n", log);
        return 0;
    }
    return program;
}

float vertices[] = {
//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
};

uint32_t indices[] = {
    0, 1, 3, // 第一個三角形
    1, 2, 3  // 第二個三角形
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, sf::ContextSettings(24, 8, 4, 4, 4));
    window.setVerticalSyncEnabled(true);
    // Load OpenGL functions using glad
    if (!gladLoadGL())
    {
        printf("Something went wrong!\n");
        exit(-1);
    }
    printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
    ImGui::SFML::Init(window);
    window.setActive(true);
    init();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Load shader
    uint32_t vertexShader = LoadShader(GL_VERTEX_SHADER, vertexShaderSource);
    uint32_t fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fragmentShaderSoucre);
    uint32_t program;
    if(vertexShader && fragmentShader)
    {
        program = LinkShaderProgram(vertexShader, fragmentShader);
        glUseProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }
    else
    {
        return 1;
    }
    // Load vertices
    // VAO
    uint32_t vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // VBO
    uint32_t vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // EBO
    uint32_t ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Specify Vertex Attribute
    uint32_t aPos = glGetAttribLocation(program, "aPos");
    glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(aPos);
    uint32_t aVertColor = glGetAttribLocation(program, "aVertColor");
    glVertexAttribPointer(aVertColor, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(aVertColor);
    uint32_t aTexCoord = glGetAttribLocation(program, "aTexCoord");
    glVertexAttribPointer(aTexCoord, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(aTexCoord);

    // Texture
    auto LoadTexture = [&](std::string path) {
        sf::Image img;
        img.loadFromFile(path);
        img.flipVertically();
        // generate a texture
        uint32_t texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        // texture parameter
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getSize().x, img.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, (const void*)img.getPixelsPtr());
        glGenerateMipmap(GL_TEXTURE_2D);
        return texture;
    };

    uint32_t wall_tex = LoadTexture("wall.jpg");
    uint32_t lambda_tex = LoadTexture("lambda.png");

    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, "tex1"), 0);
    glUniform1i(glGetUniformLocation(program, "tex2"), 1);

    sf::Clock deltaClock;
    bool running = true;
    while (running)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
                running = false;
            else if (event.type == sf::Event::Resized)
                glViewport(0, 0, event.size.width, event.size.height);
        }
        // ImGui Update
        ImGui::SFML::Update(window, deltaClock.restart());
        ImGui::Begin("5.2.lab1");
        static bool wire_mode = false;
        ImGui::Checkbox("Wire Mode", &wire_mode);
        // tint
        static float tintColor[4] = {1.f, 0.f, 0.f, 1.f};
        ImGui::ColorEdit4("Tint", tintColor, ImGuiColorEditFlags_Float);
        // mix
        static float mix;
        static sf::Clock mixClk;
        mix = (sin(2*mixClk.getElapsedTime().asSeconds()) / 2.f) + 0.5f;
        ImGui::SliderFloat("Mix", &mix, 0.f, 1.f);
        // background color
        static float bgColor[4];
        ImGui::ColorEdit4("BG", bgColor, ImGuiColorEditFlags_Float);
        // transform
        glm::mat4 trans(1.0f); // identity matrix
        static float rotate_degree = 90.f;
        rotate_degree = 360.f * ((sin(mixClk.getElapsedTime().asSeconds()) / 2.f) + 0.5f);
        ImGui::SliderFloat("Rotate", &rotate_degree, 0, 360);
        trans = glm::rotate(trans, glm::radians(rotate_degree), glm::vec3(0.0, 0.0, 1.0));

        static glm::vec3 scaler(0.5, 0.5, 0.5);
        scaler = glm::vec3(((sin(mixClk.getElapsedTime().asSeconds()) / 2.f) + 0.5f));
        ImGui::SliderFloat3("Scale", glm::value_ptr(scaler), 0.0f, 1.0f);
        trans = glm::scale(trans, scaler);

        trans = glm::translate(trans, glm::vec3(0.5f, 0.5f, 0.f));

        ImGui::End();

        glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (wire_mode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // 將 wall_tex 綁到 GL_TEXTURE0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wall_tex);
        // 將 lambda_tex 綁到 GL_TEXTURE1
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, lambda_tex);
        // Upload uniforms
        glUseProgram(program);
        static int colorLoc = glGetUniformLocation(program, "fColor");
        glUniform4f(colorLoc, tintColor[0], tintColor[1], tintColor[2], tintColor[3]);
        static int mixLoc = glGetUniformLocation(program, "fMix");
        glUniform1f(mixLoc, mix);
        static int transformLoc = glGetUniformLocation(program, "vTransform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
        //
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        if (wire_mode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        window.pushGLStates();
        {
            ImGui::SFML::Render(window);
        }
        window.popGLStates();
        window.display();
    }

    // release resources...
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(program);

    return 0;
}