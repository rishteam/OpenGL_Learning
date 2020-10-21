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
#include <filesystem>

#include <imgui.h>
#include <imgui-SFML.h>
#include "shader.h"
#include "VertexArray.h"
#include "camera.h"
#include "player.h"
#include "Texture2D.h"

#define WIRE_MODE 0

void init()
{
    setvbuf(stdin, nullptr, _IONBF, 0);
    setvbuf(stdout, nullptr, _IONBF, 0);
}

#include "vertices.h"

glm::vec3 lightPos(1.2f, 1.0f, 2.f);

const uint32_t screenWidth = 1280, screenHeight = 720;

sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "OpenGL", sf::Style::Default, sf::ContextSettings(24, 8, 4, 4, 4));

glm::vec3 lightColor(1.f, 1.f, 1.f);
glm::vec3 objectColor(1, 0.5, 0.31);
glm::vec3 lightDir(1.f, -2.f, 1.f);

glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
};

int main()
{
    std::cout << std::filesystem::current_path();
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

    glEnable(GL_DEPTH_TEST);

    // Load shader
    Shader shader("shader/8.5.Lighting_casters/lighting.vs", "shader/8.5.Lighting_casters/lighting.fs");
    Shader shader2("shader/8.5.Lighting_casters/light_cube.vs", "shader/8.5.Lighting_casters/light_cube.fs");
    VertexArray vertexArray;
    BufferLayout layout = {
            {ShaderDataType::Float3, "aPos"},
            {ShaderDataType::Float3, "aNormal"},
            {ShaderDataType::Float2, "aTexCoords"}
    };
    VertexBuffer vertex(vertices, sizeof(vertices));
    vertex.setLayout(layout);
    vertexArray.addVertexBuffer(&vertex);
    const std::string path = "assets/box.png";
    Texture2D t1("assets/box.png");
    Texture2D t2("assets/boxOutline.png");
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

    uint32_t texture = LoadTexture("assets/box.png");
    uint32_t texture2 = LoadTexture("assets/boxOutline.png");

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
                glViewport(0, 0, event.size.width, event.size.height);

            fpsView.processEvent(event);
        }
        float dt = stClk.restart().asSeconds();
        // ImGui Update
        ImGui::SFML::Update(window, deltaClock.restart());
        ImGui::Begin("6.2.cube");
        // background color
        static float bgColor[4];
        ImGui::ColorEdit4("BG", bgColor, ImGuiColorEditFlags_Float);
        ImGui::End();

        ImGui::Begin("Matrix");
        //
        glm::mat4 model(1.f), view(1.f), projection(1.f);
//        ImGui::Text("Model");
        static float rotate_x = 0.0f, rotate_y = 0.f, rotate_z = 0.f;
        static glm::vec3 obTrans = {-1.f, 1.f, 1.f};
        float step = 100 * stClk.getElapsedTime().asSeconds();
//        ImGui::DragFloat("Rotate x", &rotate_x, 1.0, -360, 360);
//        ImGui::DragFloat("Rotate y", &rotate_y, 1.0 , - 360, 360);
//        ImGui::DragFloat("Rotate z", &rotate_z, 1.0, - 360, 360);
//        ImGui::DragFloat("Pos x", &obTrans.x, .1);
//        ImGui::DragFloat("Pos y", &obTrans.y, .1);
//        ImGui::DragFloat("pos z", &obTrans.z, .1);
//        model = glm::translate(model, obTrans);
//        model = glm::rotate(model, glm::radians(rotate_x), glm::vec3(1.0f, 0.0f, 0.0f));
//        model = glm::rotate(model, glm::radians(rotate_y), glm::vec3(0.0f, 1.0f, 0.0f));
//        model = glm::rotate(model, glm::radians(rotate_z), glm::vec3(0.0f, 0.0f, 1.0f));
        ImGui::Separator();
        static float trans[3] = {0.0f, 0.0f, 0.0f};
        ImGui::Text("View");
        ImGui::SliderFloat3("Translate", trans, -100.f, 100.f);
        view = glm::translate(view, glm::vec3(trans[0], trans[1], trans[2]));

        ImGui::PushItemWidth(100);
        static glm::vec3 materialSpecular(0.5, 0.5, 0.5);
        ImGui::Text("materialSpecular");
        ImGui::DragFloat("x##materialSpecular", &materialSpecular.x, 0.1); ImGui::SameLine();
        ImGui::DragFloat("y##materialSpecular", &materialSpecular.y, 0.1); ImGui::SameLine();
        ImGui::DragFloat("z##materialSpecular", &materialSpecular.z, 0.1);

        ImGui::PopItemWidth();

        static float materialShininess = 32.f;
        ImGui::Text("materialShininess");
        ImGui::DragFloat("materialShininess", &materialShininess, 0.5f, 0, 256);

        ImGui::End();
        ImGui::Begin("Camera");
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

        ImGui::Begin("Light Attribute");

        ImGui::PushItemWidth(50);
        ImGui::Text("Light Pos");
        ImGui::DragFloat("x", &lightPos.x, 0.1f);
        ImGui::SameLine();
        ImGui::DragFloat("y", &lightPos.y, 0.1f);
        ImGui::SameLine();
        ImGui::DragFloat("z", &lightPos.z, 0.1f);
        ImGui::PopItemWidth();

        static float cutOff = 12.5f;
        ImGui::Text("Flash Light cutOff");
        ImGui::DragFloat("cutOff##Light", &cutOff, 0.1, 0, 60);

        static float outerCutOff = 17.5f;
        ImGui::Text("Flash Light outerCutOff");
        ImGui::DragFloat("outerCutOff##Light", &outerCutOff, 0.1, 0, 60);

        ImGui::PushItemWidth(100);
        static glm::vec3 lightAmbient(0.2f, 0.2f, 0.2f);
        ImGui::Text("lightAmbient");
        ImGui::DragFloat("x##lightAmbient", &lightAmbient.x, 0.1); ImGui::SameLine();
        ImGui::DragFloat("y##lightAmbient", &lightAmbient.y, 0.1); ImGui::SameLine();
        ImGui::DragFloat("z##lightAmbient", &lightAmbient.z, 0.1);

        static glm::vec3 lightDiffuse(0.8f, 0.8f, 0.8f);
        ImGui::Text("lightDiffuse");
        ImGui::DragFloat("x##lightDiffuse", &lightDiffuse.x, 0.1); ImGui::SameLine();
        ImGui::DragFloat("y##lightDiffuse", &lightDiffuse.y, 0.1); ImGui::SameLine();
        ImGui::DragFloat("z##lightDiffuse", &lightDiffuse.z, 0.1);

        static glm::vec3 lightSpecular(1.0f, 1.0f, 1.0f);
        ImGui::Text("lightSpecular");
        ImGui::DragFloat("x##lightSpecular", &lightSpecular.x, 0.1); ImGui::SameLine();
        ImGui::DragFloat("y##lightSpecular", &lightSpecular.y, 0.1); ImGui::SameLine();
        ImGui::DragFloat("z##lightSpecular", &lightSpecular.z, 0.1);
        ImGui::PopItemWidth();
        ImGui::End();

        glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        texture.bind();
        fpsView.update(dt);
        // Upload uniforms
        shader.bind();
//        shader.setMat4("vModel", model);
        shader.setMat4("vView", view);
        shader.setMat4("vProjection", projection);
//        shader.setFloat3("lightColor", lightColor);
        shader.setFloat3("objectColor", objectColor);
        shader.setFloat3("viewPos", fpsView.m_camera.m_pos);
        shader.setInt("material.diffuse", 0);
        shader.setInt("material.specular", 1);
        shader.setFloat("material.shininess", materialShininess);
        shader.setFloat3("light.ambient", lightAmbient);
        shader.setFloat3("light.diffuse", lightDiffuse);
        shader.setFloat3("light.specular", lightSpecular);
        shader.setFloat3("light.position", fpsView.m_camera.m_pos);
        shader.setFloat3("light.direction", fpsView.m_camera.m_front);
        shader.setFloat("light.cutOff", glm::cos(glm::radians(cutOff)));
        shader.setFloat("light.outerCutOff", glm::cos(glm::radians(outerCutOff)));
        shader.setFloat("light.constant", 1.f);
        shader.setFloat("light.linear", 0.09f);
        shader.setFloat("light.quadratic", 0.032f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        vertexArray.bind();

        for(int i = 0 ; i < 10 ; i++) {

            glm::mat4 model(1.f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setMat4("vModel", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        glDrawArrays(GL_TRIANGLES, 0, 36);
        shader.unbind();

//        shader2.bind();
//        shader2.setMat4("vView", view);
//        shader2.setMat4("vProjection", projection);
//        model = glm::mat4(1.f);
//        model = glm::translate(model, lightPos);
//        model = glm::scale(model, glm::vec3(0.2f));
//        shader2.setMat4("vModel", model);
//        shader2.setFloat3("lightColor", lightColor);
//        glDrawArrays(GL_TRIANGLES, 0, 36);
        vertexArray.unbind();
//        shader2.unbind();

        window.pushGLStates();
        {
            ImGui::SFML::Render(window);
        }
        window.popGLStates();
        window.display();
    }

    return 0;
}