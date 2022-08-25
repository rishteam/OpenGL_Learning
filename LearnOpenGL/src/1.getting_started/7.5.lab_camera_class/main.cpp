#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <functional>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <glad/glad.h>
#include <SFML/OpenGL.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui-SFML.h>

#include <fmt/core.h>

#include "camera.h"
#include "player.h"

void init()
{
    setvbuf(stdin, nullptr, _IONBF, 0);
    setvbuf(stdout, nullptr, _IONBF, 0);
}
// Vertex Shader
const char *vertexShaderSource = R"glsl(
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;

void main()
{
    gl_Position = vProjection * vView * vModel * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)glsl";
// Fragment Shader
const char *fragmentShaderSoucre = R"glsl(
#version 450 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D tex1;
uniform sampler2D tex2;

uniform bool fIsTint;
uniform vec4 fColor;
uniform float fMix;

void main()
{
    vec4 resultColor = mix(texture(tex1, TexCoord), texture(tex2, TexCoord), fMix);
    if(fIsTint)
        resultColor *= fColor;
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

#include "cube_vertices.h"

std::vector<glm::vec3> cubePositions = {
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(2.0f, 5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f, 3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f, 2.0f, -2.5f),
    glm::vec3(1.5f, 0.2f, -1.5f),
    glm::vec3(-1.3f, 1.0f, -1.5f)
};

uint32_t screenWidth = 1280, screenHeight = 720;
sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "OpenGL", sf::Style::Default, sf::ContextSettings(24, 8, 4, 4, 4));

int main()
{
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
    window.setMouseCursorVisible(false);
    init();

    // Blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Depth
    glEnable(GL_DEPTH_TEST);

    // Load m_ShaderID
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

    // Specify Vertex Attribute
    uint32_t aPos = glGetAttribLocation(program, "aPos");
    glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(aPos);
    uint32_t aTexCoord = glGetAttribLocation(program, "aTexCoord");
    glVertexAttribPointer(aTexCoord, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
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

    uint32_t wall_tex = LoadTexture("assets/1.jpg");
    uint32_t lambda_tex = LoadTexture("assets/1.png");

    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, "tex1"), 0);
    glUniform1i(glGetUniformLocation(program, "tex2"), 1);

    FirstPersonView fpsView;

    sf::Clock stClk;
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
            {
                glViewport(0, 0, event.size.width, event.size.height);
                screenWidth = event.size.width;
                screenHeight = event.size.height;
            }
            fpsView.processEvent(event);
        }
        float dt = stClk.restart().asSeconds();
        // ImGui Update
        ImGui::SFML::Update(window, deltaClock.restart());
        ImGui::Begin("7.5.lab_camera_class");
        static bool wire_mode = false;
        ImGui::Checkbox("Wire Mode", &wire_mode);
        // tint
        static float tintColor[4] = {1.f, 1.f, 1.f, 1.f};
        static bool isTint = false;
        ImGui::ColorEdit4("##Tint", tintColor, ImGuiColorEditFlags_Float); ImGui::SameLine();
        ImGui::Checkbox("Tint", &isTint);
        // mix
        static float mix = 0.2f;
        ImGui::SliderFloat("Mix", &mix, 0.f, 1.f);
        // background color
        static float bgColor[4];
        ImGui::ColorEdit4("BG", bgColor, ImGuiColorEditFlags_Float);
        ImGui::End();

        ImGui::Begin("Help", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        ImGui::Text("WASD - Move the camera");
        ImGui::Text("Space/Ctrl - Go up / down");
        ImGui::Text("Left Alt - Enable/Disable mouse capture");
        ImGui::Text("Esc - quit");
        ImGui::End();

        ImGui::Begin("Matrix");
        // Model matrix
        glm::mat4 model(1.f), view(1.f), projection(1.f);
        ImGui::Text("Model");
        static float rotate_x = -55.0f, rotate_y = 0.f, rotate_z = 0.f;
        ImGui::DragFloat("Rotate x", &rotate_x, 1.0, -360, 360);
        ImGui::DragFloat("Rotate y", &rotate_y, 1.0 , - 360, 360);
        ImGui::DragFloat("Rotate z", &rotate_z, 1.0, - 360, 360);
        //
        ImGui::Separator();
        // View matrix
        ImGui::Text("View");
        view = fpsView.getViewMatrix();
        ImGui::DragFloat3("Camera", glm::value_ptr(fpsView.m_camera.m_pos));
        ImGui::DragFloat("Speed", &fpsView.cameraSpeed);
        ImGui::DragFloat("Sensitivity", &fpsView.sensitivity);
        // Projection matrix
        ImGui::Text("Projection");
        ImGui::DragFloat("FOV", &fpsView.m_camera.m_fov, 1.0f, 0.f, 360.f);
        ImGui::DragFloat("Near", &fpsView.m_camera.m_near, 0.01);
        ImGui::DragFloat("Far", &fpsView.m_camera.m_far, 0.1);
        ImGui::Text("fpsView.m_camera.m_screenRatio = %f", fpsView.m_camera.m_screenRatio);
        projection = fpsView.getProjectionMatrix();
        ImGui::End();

        // Update
        fpsView.update(dt);

        // Render
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
        static int isTintLoc = glGetUniformLocation(program, "fIsTint");
        glUniform1i(isTintLoc, isTint);
        auto UploadMat4 = [&](const std::string &name, glm::mat4 &mat4) {
            int uniformLoc = glGetUniformLocation(program, name.c_str());
            glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(mat4));
        };
        UploadMat4("vView", view);
        UploadMat4("vProjection", projection);
        //
        glBindVertexArray(vao);
        for(int i = 0; i < cubePositions.size(); i++)
        {
            model = glm::translate(model, glm::vec3(cubePositions[i]));
            model = glm::rotate(model, glm::radians(rotate_x), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(rotate_y), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(rotate_z), glm::vec3(0.0f, 0.0f, 1.0f));
            UploadMat4("vModel", model);
            model = glm::mat4(1.f);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
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
    glDeleteProgram(program);

    return 0;
}