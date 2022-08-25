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

void showMat(const std::string &name, const glm::mat4 &m)
{
    ImGui::Text("%s", name.c_str());
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            ImGui::PushID(std::to_string(i*10+j).c_str());
            ImGui::Text("%.2f", m[i][j]);
            ImGui::SameLine();
            ImGui::PopID();
        }
        ImGui::NewLine();
    }
}

glm::mat4 glmLookAt(glm::vec3 const &eye, glm::vec3 const &center, glm::vec3 const &up)
{
    ImGui::Begin("glmLookAt");
    glm::vec3 const f(normalize(center - eye)); // front
    glm::vec3 const s(normalize(cross(f, up))); // right
    glm::vec3 const u(cross(s, f));             // up

    glm::mat4 res(1.f);
    // right
    res[0][0] = s.x;
    res[1][0] = s.y;
    res[2][0] = s.z;
    // up
    res[0][1] = u.x;
    res[1][1] = u.y;
    res[2][1] = u.z;
    // dir
    res[0][2] = -f.x;
    res[1][2] = -f.y;
    res[2][2] = -f.z;
    //
    showMat("left", res);
    //
    glm::mat4 b(1.f);
    b[3][0] = -dot(s, eye);
    b[3][1] = -dot(u, eye);
    b[3][2] = dot(f, eye);
    showMat("right", b);
    showMat("res", res * b);
    ImGui::End();
    return res * b;
}

glm::mat4 MyLookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up)
{
    ImGui::Begin("MyLookAt");
    glm::vec3 front = glm::normalize(center - eye);           // 從眼睛射出去
    glm::vec3 right = glm::normalize(glm::cross(front, up));  // x 軸  (right vector)
    glm::vec3 u = glm::normalize(glm::cross(right, front));   // y 軸  (up vector)
    glm::vec3 dir = -front;                                   // z 軸

    glm::mat4 l(1.f), r(1.f);
    l[0][0] = right.x;
    l[1][0] = right.y;
    l[2][0] = right.z;
    //
    l[0][1] = u.x;
    l[1][1] = u.y;
    l[2][1] = u.z;
    //
    l[0][2] = dir.x;
    l[1][2] = dir.y;
    l[2][2] = dir.z;
    //
    showMat("left", l);
    //
    r[3][0] = -eye.x;
    r[3][1] = -eye.y;
    r[3][2] = -eye.z;
    //
    showMat("right", r);
    //
    auto res = l * r;
    showMat("res", res);
    ImGui::End();
    return res;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "OpenGL", sf::Style::Default, sf::ContextSettings(24, 8, 4, 4, 4));
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

    uint32_t wall_tex = LoadTexture("wall.jpg");
    uint32_t lambda_tex = LoadTexture("lambda.png");

    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, "tex1"), 0);
    glUniform1i(glGetUniformLocation(program, "tex2"), 1);

    sf::Clock stClk;
    sf::Clock deltaClock;
    bool isMouseCaptured = false;
    float mouseWheelDelta = 0.f;
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
            else if(event.type == sf::Event::KeyPressed)
            {
                switch(event.key.code)
                {
                    case sf::Keyboard::Escape:
                        running = false;
                        continue;
                    break;

                    case sf::Keyboard::LAlt:
                        isMouseCaptured = !isMouseCaptured;
                        sf::Mouse::setPosition({window.getSize().x / 2, window.getSize().y / 2} , window);
                    break;
                }
            }
            else if(event.type == sf::Event::MouseWheelScrolled)
            {
                mouseWheelDelta = event.mouseWheelScroll.delta;
            }
        }
        float dt = stClk.restart().asSeconds();
        // ImGui Update
        ImGui::SFML::Update(window, deltaClock.restart());
        ImGui::Begin("7.4.make ur own LookAt Matrix");
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

        //
        ImGui::Begin("Matrix");

        glm::mat4 model(1.f), view(1.f), projection(1.f);
        ImGui::Text("Model");
        static float rotate_x = -55.0f, rotate_y = 0.f, rotate_z = 0.f;
        ImGui::DragFloat("Rotate x", &rotate_x, 1.0, -360, 360);
        ImGui::DragFloat("Rotate y", &rotate_y, 1.0 , - 360, 360);
        ImGui::DragFloat("Rotate z", &rotate_z, 1.0, - 360, 360);

        ImGui::Separator();
        ImGui::Text("View");
        static float cameraSpeed = 0.05f;
        static glm::vec3 cameraPos(0.f, 0.f, 3.f);
        static glm::vec3 cameraFront(0.f, 0.f, -1.f);
        static glm::vec3 cameraUp(0.f, 1.f, 0.f);
        static float pitch  = 0.f;
        static float yaw = -90.f;
        static float sensitivity = 0.05f;
        glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));
        ImGui::DragFloat3("Camera", glm::value_ptr(cameraPos));
        view = MyLookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glmLookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        ImGui::DragFloat("Speed", &cameraSpeed);
        ImGui::DragFloat("Sensitivity", &sensitivity);

        auto frontFloat = glm::value_ptr(cameraFront);
        ImGui::Text("Front %.2f %.2f %.2f", frontFloat[0], frontFloat[1], frontFloat[2]);
        auto UpFloat = glm::value_ptr(cameraUp);
        ImGui::Text("Up    %.2f %.2f %.2f", UpFloat[0], UpFloat[1], UpFloat[2]);
        auto RightFloat = glm::value_ptr(cameraRight);
        ImGui::Text("Right %.2f %.2f %.2f", RightFloat[0], RightFloat[1], RightFloat[2]);
        //
        ImGui::Text("Projection");
        static float fov = 45.f, near = 0.1, far = 100.f;
        if(mouseWheelDelta != 0.f)
        {
            fov += mouseWheelDelta;
            if(fov < 1.f) fov = 1.f;
            if(fov > 45.f) fov = 45.f;
            mouseWheelDelta = 0.f;
        }
        ImGui::DragFloat("FOV", &fov, 1.0f, 0.f, 360.f);
        ImGui::DragFloat("Near", &near, 0.01);
        ImGui::DragFloat("Far", &far, 0.1);
        projection = glm::perspective(glm::radians(fov), (float)screenWidth / screenHeight, near, far);
        // Mouse move
        if(isMouseCaptured)
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_None);
            window.setMouseCursorGrabbed(true);
            //
            auto [x, y] = sf::Mouse::getPosition(window);
            ImGui::Text("mpos = %d %d", x, y);
            ImGui::Text("window size = %d %d", window.getSize().x, window.getSize().y);
            glm::vec2 nowCursorPos(x, window.getSize().y - y);
            glm::vec2 screenCenter = glm::vec2(window.getSize().x / 2, window.getSize().y / 2);
            if (window.getSize().y % 2 != 0)
                screenCenter.y += 1;
            sf::Mouse::setPosition({window.getSize().x / 2, window.getSize().y / 2}, window);
            glm::vec2 off = nowCursorPos - screenCenter;
            ImGui::Text("%.2f %.2f", off.x, off.y);

            off *= sensitivity;
            pitch += off.y;
            yaw += off.x;

            if(pitch > 89.f) pitch = 89.f;
            if(pitch < -89.f) pitch = -89.f;

            glm::vec3 newCameraFront;
            newCameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            newCameraFront.y = sin(glm::radians(pitch));
            newCameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            cameraFront = glm::normalize(newCameraFront);
        }
        else
            window.setMouseCursorGrabbed(false);
        ImGui::End();

        // Update
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            cameraSpeed = dt * 5;
        else
            cameraSpeed = dt * 3;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            cameraPos += cameraSpeed * cameraFront;
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            cameraPos -= cameraSpeed * cameraFront;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            cameraPos -= cameraSpeed * cameraRight;
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            cameraPos += cameraSpeed * cameraRight;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            cameraPos += cameraSpeed * cameraUp;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
            cameraPos -= cameraSpeed * cameraUp;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            yaw -= 1.f;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            yaw += 1.f;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            pitch += 1.f;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            pitch -= 1.f;
        if(pitch > 89.f) pitch = 89.f;
        if(pitch < -89.f) pitch = -89.f;

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